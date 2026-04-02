// TableHandler.h
#ifndef TABLEHANDLER_H
#define TABLEHANDLER_H

#include "Types.h"
#include <Arduino.h>

/**
 * @struct table_row
 * @brief Represents a single entry in the color lookup table.
 * 
 * Maps a specific set of RGB coordinates to a human-readable Italian name.
 */
struct table_row {
    int r;                ///< Red component (0-256)
    int g;                ///< Green component (0-256)
    int b;                ///< Blue component (0-256)
    const char* color_name; ///< String representation of the color (e.g., "Rosso", "Ciano Scuro")
};

/**
 * @brief Total number of rows available in the global color data table.
 */
extern const size_t MAX_ROW_DATA;

/**
 * @class TableHandler
 * @brief Provides utility functions to identify and name colors based on RGB values.
 * 
 * This class uses a static lookup table and a quantization algorithm to find the 
 * closest matching named color for any given RGB input.
 */
class TableHandler {
    public:
        /**
         * @brief Identifies a color and assigns its human-readable name.
         * 
         * The function performs the following steps:
         * 1. **Quantization**: Rounds the input R, G, B values to the nearest 
         *    multiple of 64 using the formula: $value = \text{round}(raw / 64) \times 64$.
         * 2. **Lookup**: Searches the internal `data` table for an exact match of 
         *    the quantized coordinates.
         * 3. **Naming**: If a match is found, updates the `color_name` field of the 
         *    returned RGB object.
         * 
         * @param rgb An RGB structure containing the raw color values to be identified.
         * @return RGB A new RGB structure with quantized values and the identified color name.
         */
        static RGB getColor(RGB rgb);
};

#endif 