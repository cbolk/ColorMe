// SensorHandler.h
#ifndef COLORSENSOR_H
#define COLORSENSOR_H

#include <Adafruit_TCS34725.h>
#include "DisplayHandler.h"
#include "Types.h"
#include "TableHandler.h"
#include "LittleFSHandler.h"

#pragma once

/**
 * @class SensorHandler
 * @brief Manages the TCS34725 Color Sensor, including calibration and color recognition.
 * 
 * This class handles the low-level communication with the RGB sensor, implements
 * a calibration routine to normalize readings against a white surface, and 
 * provides human-readable color names using a lookup table.
 */
class SensorHandler {
    private:
        DisplayHandler display;      ///< Instance for visual feedback during sensor operations.
        Adafruit_TCS34725 tcs;       ///< The Adafruit sensor driver instance.
        RGB_coef calibration_coef;   ///< Stored multipliers to balance R, G, and B channels.
        RGB rgb;                     ///< The last processed color data.

        /**
         * @brief Saves the current calibration coefficients to "/sensor_config.json".
         * 
         * Encapsulates the logic for writing r_coef, g_coef, b_coef, and real_white 
         * to the filesystem for persistence across reboots.
         */
        void calib_saving();

    public: 
        /**
         * @brief Constructor for SensorHandler.
         * Initializes the TCS34725 with 50ms integration time and 4X gain.
         */
        SensorHandler() : tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X) {}; 
        
        /**
         * @brief Initializes the sensor hardware and loads calibration data.
         * 
         * @return SENSOR_OK_AND_CALIBRATED If valid config is found in LittleFS.
         * @return SENSOR_NEED_CALIBRATION If no config file exists (first run).
         * @note Blocks execution if the physical sensor is not detected via I2C.
         */
        sensor_status begin();

        /**
         * @brief Executes the interactive white-balancing calibration.
         * 
         * Leads the user through a timed process (5-second countdown) where the 
         * sensor must be pointed at a white surface. It averages 50 readings to 
         * determine the channel offsets and calculates the `real_white` reference.
         * 
         * @return true When calibration is complete and saved.
         */
        const bool Calibration();

        /**
         * @brief Captures and processes the current color.
         * 
         * The process includes:
         * 1. Averaging 10 raw readings for stability.
         * 2. Applying calibration coefficients: 
         *    $final = (raw / cycles) * coef / real\_white * 255$
         * 3. Clipping values to 256.
         * 4. Identifying the color name via TableHandler.
         * 
         * @return RGB structure containing the calibrated R, G, B values and its name.
         */
        const RGB GetColor();
};

#endif 