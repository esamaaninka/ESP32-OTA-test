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
#include "SPIFFS.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
//#include <Serial.h>
#include "OTA-webserver-page.h"
#include "DHT-webpage.h"
#include <string.h>
#include <Adafruit_Sensor.h>
#include "mydht.h"
#include "myhx711.h"
#include <PubSubClient.h>

// MQTT Broker
//const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_broker = "192.168.164.62";
const char *topic = "esp32/test";
//const char *mqtt_username = "emqx";
//const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 
 String messageTemp;

 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
     messageTemp += (char)payload[i];
 }
 Serial.println();
 Serial.println("-----------------------");

 // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  /*
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
  */
}
// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

/*
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 21;
const int LOADCELL_SCK_PIN = 19;

HX711 scale;
*/
// Replace with your network credentials
const char* ssid = "Pajaranta";
const char* password = "Perttulintie7C";

bool ledState = 0;
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

/*
String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.print("DHT722 temperature read: ");
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.print("DHT22 humidity read:");
    Serial.println(h);
    return String(h);
  }
}
*/
/*String readHX711Scale(){
   if (scale.is_ready()) {
    long reading = scale.read();
    
    Serial.print("HX711 reading: ");
    Serial.println((float)reading);
    return String(reading);
  } else {
    Serial.println("HX711 not found.");
    return String("--");
  }
}

String setHX711Scale(){
  Serial.println("Set the scale calibration.");
  Serial.println("Set know weight on the scale.");
  Serial.println("Input the weight.");
  return "setHX711Scale";
}
*/
void send_measurements(){
  char message[80]; // tarkista pituus mikä max tarvitaan
  //char *message_chr;
  
  sprintf(message, "ESP32 temp %s, humidity %s, weight %s", readDHTTemperature(), readDHTHumidity(), readHX711Scale());
  Serial.println("Send measurements via MQTT");
  Serial.println(message);
  mqtt_client.publish(topic,"sending measurements");
  mqtt_client.publish(topic, &message[0]); // vai pelkkä message
}

void notifyClients() {
  ws.textAll(String(ledState));
}

/* Message callback of Serial */
void recvMsg(uint8_t *data, size_t len){
  Serial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  Serial.println(d);
  
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
      Serial.println("Switching led state");
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
      Serial.print("Hello ");
      Serial.println((char*)data);
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

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //dht.begin();
  setupDHT();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  setupScale();
  //scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // MQTT setup
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(callback);
  while (!mqtt_client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     //if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
       if (mqtt_client.connect(client_id.c_str())) {
     //if (mqtt_client.connect()) {
         Serial.println("mqtt broker connected");
         mqtt_client.publish(topic,"ESP32 here gossiping");
         mqtt_client.subscribe(topic);
         //Serial.println("mqtt broker connected");
     } else {
         //Serial.print("mqtt failed with state ");
         Serial.print("mqtt failed with state ");
         //Serial.print(mqtt_client.state());
         Serial.print(mqtt_client.state());
         delay(2000);
     }
 }


  // Route for root / web page index page in /include *.h
  server.on("/ota", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  // Route for root / web page index.html in SPIFFS 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("yritetään lukea SPIFSS:stä index sivu");
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
    // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // route for DHT22 temp and hum
  server.on("/dht", HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("Temp/hum via dht-processor");
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

   // Serial is accessible at "<IP Address>/Serial" in browser
    //Serial.begin(&server);
    /* Attach Message Callback */
    //Serial.msgCallback(recvMsg);

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
  mqtt_client.loop();
  mqtt_client.publish(topic,"esp32 loop message");
  send_measurements();
  delay(10000);
}