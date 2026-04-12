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
  Serial.println("Inizializzazione");
  Serial.println("Tasto rilevazione (verde)");
  pinMode(TRIGGER_DETECTION, INPUT_PULLUP);
  Serial.println("Tasto calibrazione (giallo)");
  pinMode(TRIGGER_CALIBRATION, INPUT_PULLUP);
  Serial.println("Pin disabilita led");
  pinMode(LED_PIN, OUTPUT);
  delay(2000);
  Serial.println("Inizializza display");
  //theDisplay.begin();
  delay(1000);
  Serial.println("Display inizializzato");

  Serial.println("Inizializza sensore e rileva presenza calibrazione");
  calibration_status =  sensor.begin();
  //theDisplay.print("Fase di setup terminata");
  delay(1000);  
}

// LOOP
void loop() {
  RGB col;
  if (digitalRead(TRIGGER_CALIBRATION) == LOW) {
    Serial.println("Calibrazione");
    if(!sensor.Calibration())
      Serial.println("Errore in fase di calibrazione");
    else{
      Serial.println("Calibrazione terminata");
      calibration_status = SENSOR_OK_AND_CALIBRATED;
    }
    delay(50); 
  }
  if (digitalRead(TRIGGER_DETECTION) == LOW) {
    Serial.println("Rilevazione");
    if(calibration_status == SENSOR_OK_AND_CALIBRATED){
      Serial.println("Procedo");
      Serial.println("Accendo il led");
      digitalWrite(LED_PIN, HIGH);
      col = sensor.GetColor();
//      theDisplay.displayColor(col.r, col.g, col.b);
      Serial.println("Spengo il led");
      digitalWrite(LED_PIN, LOW);
    } else {
      Serial.println("Prima calibrare il sensore");
      theDisplay.print("Calibrare il sensore");
    }
    delay(50);
  }
  
}
