#include <DHT.h>

#define DHTPIN 27     // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT22     // DHT 22 (AM2302)



String readDHTTemperature(void);
String readDHTHumidity(void);
void setupDHT(void);