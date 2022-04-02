#include "mymqtt.h"

WiFiClient espClient;
PubSubClient my_mqtt_client(espClient);

const char *mqtt_broker = "192.168.164.62";
const char *topic = "esp32/test";



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
void setup_my_mqtt(void){
    my_mqtt_client.setCallback(callback);
    while (!my_mqtt_client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s tries to connect to the public mqtt broker\n", client_id.c_str());
        //if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
        if (my_mqtt_client.connect(client_id.c_str())) {
        //if (mqtt_client.connect()) {
            Serial.println("\nmqtt broker connected\n");
            my_mqtt_client.publish(topic,"ESP32 here gossiping");
            my_mqtt_client.subscribe(topic);
         //Serial.println("mqtt broker connected");
        } else {
            //Serial.print("mqtt failed with state ");
            Serial.print("\nmqtt failed with state: ");
            //Serial.print(mqtt_client.state());
            Serial.print(my_mqtt_client.state());
            Serial.println();
            delay(2000);
        }
    }
}