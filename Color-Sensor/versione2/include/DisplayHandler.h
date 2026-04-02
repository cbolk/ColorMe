#ifndef DISPLAYHANDLER_H
#define DISPLAYHANDLER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#pragma once

/** 
 * @brief OLED Screen dimensions in pixels.
 */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 

/**
 * @class DisplayHandler
 * @brief Wrapper for the Adafruit SSD1306 library to simplify OLED management.
 * 
 * This class abstracts the complexity of the Adafruit_SSD1306 library, providing
 * simple methods to initialize and print messages on a 128x64 I2C display.
 */
class DisplayHandler {
    private:
        /** 
         * @brief Internal driver for the SSD1306 OLED display.
         */
        Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    public:
        /**
         * @brief Constructor for DisplayHandler.
         * Initializes the display object with defined dimensions and I2C interface.
         * The reset pin is set to -1 (unused).
         */
        DisplayHandler() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {};

        /**
         * @brief Initializes the OLED display hardware.
         * 
         * Configures the I2C address (0x3C), sets the default text size to 1,
         * white color, and clears the screen buffer.
         */
        void begin();

        /**
         * @brief Clears the screen and prints a single row of text.
         * 
         * The text is positioned at the top-left corner (0,0).
         * 
         * @param row1 The string to be displayed on the first line.
         */
        void print(String row1);

        /**
         * @brief Clears the screen and prints two rows of text.
         * 
         * - Row 1 is placed at coordinate (0,0).
         * - Row 2 is placed at coordinate (0,16), providing adequate spacing 
         *   between lines for readability.
         * 
         * @param row1 The string for the first line.
         * @param row2 The string for the second line.
         */
        void print(String row1, String row2);

        /**
         * @brief Clears the screen and prints two rows of text.
         * 
         * - Row 1 is placed at coordinate (0,0).
         * - Row 2 is placed at coordinate (0,16), providing adequate spacing 
         *   between lines for readability.
         * - Row 3 is placed at coordinate (0,32), providing adequate spacing 
         *   between lines for readability.
         * 
         * @param row1 The string for the first line.
         * @param row2 The string for the second line.
         * @param row3 The string for the second line.
         */
        void print(String row1, String row2, String row3);

};

#endif 