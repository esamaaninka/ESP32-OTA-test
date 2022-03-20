#include "myhx711.h"

HX711 scale;

void setupScale(void){
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

String readHX711Scale(){
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
