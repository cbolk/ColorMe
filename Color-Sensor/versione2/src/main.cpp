#include <Arduino.h>

//project library
#include "SensorHandler.h"
#include "CalibrationHandler.h"
#include "DisplayHandler.h"
#include "Types.h"


// SDA → pin A4
// SCL → pin A5
#define LED_PIN 4
#define TRIGGER_DETECTION   3 //verde
#define TRIGGER_CALIBRATION 2 //giallo
// 3.3V 
// --------------------------------------------------
//INITIALIZZATION
DisplayHandler        theDisplay;       //Display
SensorHandler         sensor;         //Sensor
CalibrationHandler    myCalibration;  

sensor_status calibration_status = SENSOR_NEED_CALIBRATION;

// SETUP
void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_DETECTION, INPUT_PULLUP);
  pinMode(TRIGGER_CALIBRATION, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  //Everything that need a .begin()
  theDisplay.begin();
  delay(1000);
  calibration_status =  sensor.begin();
  theDisplay.print("Setup");
  delay(1000);  
}

// LOOP
void loop() {
  RGB col;
  if (digitalRead(TRIGGER_CALIBRATION) == LOW) {
    if(!sensor.Calibration())
      Serial.println("Error while calibrating");
    else
      calibration_status = SENSOR_OK_AND_CALIBRATED;
    delay(50); 
  }
  if (digitalRead(TRIGGER_DETECTION) == LOW) {
    if(calibration_status == SENSOR_OK_AND_CALIBRATED){
      digitalWrite(LED_PIN, HIGH);
      col = sensor.GetColor();
      theDisplay.displayColor(col.r, col.g, col.b);
      digitalWrite(LED_PIN, LOW);
    } else {
      theDisplay.print("Calibrare il sensore");
      Serial.println("Calibrare il sensore");
    }
    delay(50);
  }
  
}

const char* getColorName(float hue, float sat, float val) {
  const char* cn = "sconosciuto";

  if (sat == 0) {
    if (val > 0.9)      cn = "bianco";
    else if (val > 0.1) cn = "grigio";
    else                cn = "nero";
  } else {
    if ((hue >= 0 && hue <= 21.5) || (hue > 325 && hue < 360)) {
      if (val > 0.5) {
        cn = (sat < 0.5) ? "rosa" : "rosso";
      } else {
        cn = (sat < 0.5) ? "rosso" : "marrone";
      }
    } else if (hue > 21.5 && hue <= 42) {
      if (val > 0.6) {
        cn = (sat < 0.5) ? "arancione chiaro" : "arancione";
      } else {
        cn = (sat < 0.5) ? "rosso" : "marrone";
      }
    } else if (hue > 42  && hue <= 65)  cn = "giallo";
    else if   (hue > 65  && hue <= 160) cn = "verde";
    else if   (hue > 160 && hue <= 220) cn = "ciano";
    else if   (hue > 220 && hue <= 255) cn = "blu";
    else if   (hue > 255 && hue <= 325) cn = "viola";
  }

  Serial.print(cn);
  Serial.print(": ");
  Serial.print(hue, 1);
  Serial.print(", ");
  Serial.print(sat, 1);
  Serial.print(", ");
  Serial.println(val, 1);
  return cn;
}
