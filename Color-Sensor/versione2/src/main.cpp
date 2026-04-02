#include <Arduino.h>

//project library
#include "SensorHandler.h"
#include "LittleFSHandler.h"
#include "DisplayHandler.h"
#include "Types.h"


// ESP32 Config
// --------------------------------------------------
#define SDA_PIN 2               // arancione
#define SCL_PIN 3               // bianco
#define LED_PIN 4               // grigio
#define TRIGGER_DETECT 2        // verde
#define TRIGGER_CALIBRATION 3   // giallo
// 3.3V 
// --------------------------------------------------
//INITIALIZZATION
DisplayHandler  theDisplay;     // Display
LittleFSHandler fs_handler;     // LittleFS_Handler
SensorHandler   sensor;         // Sensor

sensor_status calibration_status = SENSOR_NEED_CALIBRATION;

// SETUP
void setup() {

  Serial.begin(115200);

  // tasti e controllo spegnimento led
  pinMode(TRIGGER_DETECT, INPUT_PULLUP);
  pinMode(TRIGGER_CALIBRATION, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT_PULLDOWN);
  
  //Everything that need a .begin()
  Wire.begin(SDA_PIN, SCL_PIN);  // I2C
  theDisplay.begin();
  delay(1000);
  fs_handler.begin(); 
  delay(1000);
  calibration_status =  sensor.begin();

  theDisplay.print("Sistema in fase di setup");
  delay(1000);
}

// LOOP
void loop() {
  if (digitalRead(TRIGGER_CALIBRATION) == LOW) {
    if(sensor.Calibration())
      calibration_status = SENSOR_OK_AND_CALIBRATED;
    else
      Serial.println("Errore di calibrazione");
    delay(50); 
  }
  if (digitalRead(TRIGGER_DETECT) == LOW) {
    if(calibration_status == SENSOR_OK_AND_CALIBRATED)
      theDisplay.displayColor(sensor.GetColor());
    else
    {
      theDisplay.print("Calibrazione necessaria");
      Serial.println("Calibrazione necessaria");
    }
    delay(50);
  }
}
