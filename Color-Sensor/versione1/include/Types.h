#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

#pragma once

/**
 * @struct RGB_coef
 * @brief Calibration coefficients for the RGB sensor.
 * 
 * These values are used to normalize the raw sensor readings against a 
 * known white reference.
 */
struct RGB_coef {
    /** 
     * @brief Maximum raw value detected during calibration on a white surface.
     * Used as a baseline for scaling all color channels.
     */
    int realwhite = 0;

    /** 
     * @brief Multipliers for Red, Green, and Blue channels.
     * Calculated as: $coef = realwhite / raw\_channel\_average$.
     */
    float r = 1.0, g = 1.0, b = 1.0;
};

/**
 * @struct RGB
 * @brief Represents a processed color with its coordinates and name.
 */
struct RGB {
    int r = 0;              ///< Red component (normalized 0-255 or 256 for overflow)
    int g = 0;              ///< Green component (normalized 0-255 or 256 for overflow)
    int b = 0;              ///< Blue component (normalized 0-255 or 256 for overflow)
    String color_name = ""; ///< Human-readable name identified by TableHandler
};

/**
 * @enum sensor_status
 * @brief Status codes for the SensorHandler initialization and health check.
 */
typedef enum {
    SENSOR_OK_AND_CALIBRATED, ///< Sensor is active and valid config was loaded from FS.
    SENSOR_NOT_OK,            ///< Sensor hardware not detected or failed.
    SENSOR_NEED_CALIBRATION   ///< Sensor is active but no valid calibration file was found.
} sensor_status;

#endif // TYPES_H