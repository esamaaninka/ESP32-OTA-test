#include "HX711.h"

#ifndef HX711_H
#define HX711_H
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 21;
const int LOADCELL_SCK_PIN = 19;
// miksi tämän määritys tässä johtaa linkkausvirhe ?
//HX711 scale;

#endif

void setupScale(void);

String readHX711Scale(void);
String setHX711Scale(void);