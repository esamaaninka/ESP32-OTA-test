/*
  Rui Santos
  Complete project details
   - Arduino IDE: https://RandomNerdTutorials.com/esp32-ota-over-the-air-arduino/
   - VS Code: https://RandomNerdTutorials.com/esp32-ota-over-the-air-vs-code/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Import required libraries
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <WebSerial.h>
#include "OTA-webserver-page.h"
#include "DHT-webpage.h"
#include <string.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 21;
const int LOADCELL_SCK_PIN = 19;

HX711 scale;

#define DHTPIN 27     // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// Replace with your network credentials
const char* ssid = "Pajaranta";
const char* password = "Perttulintie7C";

bool ledState = 0;
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    WebSerial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    WebSerial.print("DHT722 temperature read: ");
    WebSerial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    WebSerial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    WebSerial.print("DHT22 humidity read:");
    WebSerial.println(h);
    return String(h);
  }
}

String readHX711Scale(){
   if (scale.is_ready()) {
    long reading = scale.read();
    
    WebSerial.print("HX711 reading: ");
    WebSerial.println((float)reading);
    return String(reading);
  } else {
    WebSerial.println("HX711 not found.");
    return String("--");
  }
}

String setHX711Scale(){
  WebSerial.println("Set the scale calibration.");
  WebSerial.println("Set know weight on the scale.");
  WebSerial.println("Input the weight.");
  return "setHX711Scale";
}

void notifyClients() {
  ws.textAll(String(ledState));
}

/* Message callback of WebSerial */
void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
  
  // TÄMÄ *EI TOIMI
  if(d == "ON"){
    digitalWrite( ledPin, HIGH);
  }
  if(d == "OFF"){
    digitalWrite( ledPin, LOW);
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      WebSerial.println("Switching led state");
      ledState = !ledState;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      WebSerial.print("Hello ");
      WebSerial.println((char*)data);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
   if(var == "TEMPERATURE"){
    return readDHTTemperature();
    //return "22";
  }
  if(var == "HUMIDITY"){
    return readDHTHumidity();
    //return "33";
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  dht.begin();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // route for DHT22 temp and hum
  server.on("/dht", HTTP_GET,[](AsyncWebServerRequest *request){
    WebSerial.println("Temp/hum via dht-processor");
    request->send_P(200, "text/html",  index_dht_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });

  server.on("/scale", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readHX711Scale().c_str());
  });

  server.on("/set-scale", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", setHX711Scale().c_str());
  });

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);

   // WebSerial is accessible at "<IP Address>/webserial" in browser
    WebSerial.begin(&server);
    /* Attach Message Callback */
    WebSerial.msgCallback(recvMsg);

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}