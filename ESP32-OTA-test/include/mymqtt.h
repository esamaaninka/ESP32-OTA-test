#include <WiFi.h>
#include <PubSubClient.h>

#ifndef MY_MQTT_H
#define MY_MQTT_H

// MQTT Broker
//const char *mqtt_broker = "broker.emqx.io";
//const char *mqtt_broker = "192.168.164.62";
//const char *topic = "esp32/test";
//const char *mqtt_username = "emqx";
//const char *mqtt_password = "public";
const int mqtt_port = 1883;

//WiFiClient espClient;
//PubSubClient my_mqtt_client(espClient);
#endif

void callback(char *topic, byte *payload, unsigned int length);

void setup_my_mqtt(void);

