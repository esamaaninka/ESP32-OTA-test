#include "my_wifi.h"

const char* ssid = "Pajaranta";
const char* password = "Perttulintie7C";

void setup_myWifi(void){
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    
    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());
}