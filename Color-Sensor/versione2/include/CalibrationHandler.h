#ifndef CALIBRATIONHANDLER_H
#define CALIBRATIONHANDLER_H

#include <Arduino.h>
#include <EEPROM.h>

class CalibrationHandler {
public:

    typedef struct {
        float r;
        float g;
        float b;
        int   realwhite;
    } CalibrationData;

    static bool loadCalibration(CalibrationData& data, const char* path);
    static void saveCalibration(const CalibrationData& data, const char* path);
};

#endif