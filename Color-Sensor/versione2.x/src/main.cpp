#include <Arduino.h>

//project library
#include "SensorHandler.h"
#include "LittleFSHandler.h"
#include "WebManager.h"
#include "WifiHandler.h"
#include "DisplayHandler.h"
#include "Types.h"


// ESP32 Config
// --------------------------------------------------
#define SDA_PIN 21
#define SCL_PIN 22
#define SCAN_TRIGGER 19 //gray-yellow
#define CALI_TRIGGER 18 //purple-green
// 3.3V 
// --------------------------------------------------
//INITIALIZZATION
DisplayHandler display;       //Display
LittleFSHandler fs_handler;   //LittleFS_Handler
WifiHandler wifi_handler;     //WifiHandler
WebManager web_manager;       //WebManager
SensorHandler sensor;         //Sensor

sensor_status calibration_status = SENSOR_NEED_CALIBRATION;

//wifi const
#define NETWORK_JSON_PATH "/wifi.json"
const std::vector<String> KEY_NAME_NETWORK_JSON = {"ssid_wifi","psw_wifi"};



// SETUP
void setup() {

  Serial.begin(115200);

  pinMode(SCAN_TRIGGER, INPUT_PULLUP);
  pinMode(CALI_TRIGGER, INPUT_PULLUP);
  
  //Everything that need a .begin()
  Wire.begin(SDA_PIN, SCL_PIN);  // I2C
  display.begin();
  delay(1000);
  fs_handler.begin(); 
  delay(1000);
  wifi_handler.begin(); 
  delay(1000);
  web_manager.begin();
  delay(1000);
  calibration_status =  sensor.begin();


  display.print("Welcome!");
  delay(1000);
  
}

// LOOP
void loop() {
  if(web_manager.new_credentials == true){
    std:: vector<String> value;
    value.reserve(KEY_NAME_NETWORK_JSON.size());
    if(!fs_handler.loadFS_json(value, KEY_NAME_NETWORK_JSON, NETWORK_JSON_PATH))
      display.print("Error while loading");
    wifi_handler.WifiConnect(value[0],value[1]);
    web_manager.new_credentials = false;
  }
  if (digitalRead(CALI_TRIGGER) == LOW) {
    if(!sensor.Calibration())
      Serial.println("Error while calibrating");
    else
      calibration_status = SENSOR_OK_AND_CALIBRATED;
    delay(50); 
  }
  if (digitalRead(SCAN_TRIGGER) == LOW) {
    if(calibration_status == SENSOR_OK_AND_CALIBRATED)
      web_manager.updateColor(sensor.GetColor());
    else
    {
      display.print("sensor is not calibrated, please do");
      Serial.println("sensor is not calibrated, please do");
    }
    delay(50);
  }
  
}
