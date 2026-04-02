#include "CalibrationHandler.h"

void CalibrationHandler::saveCalibration(const CalibrationData& data, const char* path) {
    EEPROM.put(0, data);
    Serial.println("Calibration saved.");
}

bool CalibrationHandler::loadCalibration(CalibrationData& data, const char* path) {
    EEPROM.get(0, data);

    if (isnan(data.r) || isnan(data.g) || isnan(data.b)) {
        Serial.println("Calibration data not valid!");
        return false;
    }

    Serial.println("Calibration loaded.");
    return true;
}