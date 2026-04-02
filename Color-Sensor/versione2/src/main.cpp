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
