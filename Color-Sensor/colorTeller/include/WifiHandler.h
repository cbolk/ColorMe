#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <ArduinoJson.h>
#include <WiFi.h>
#include "DisplayHandler.h"
#include "LittleFSHandler.h"

#pragma once

/**
 * @class WifiHandler
 * @brief Manages WiFi connectivity for ESP32, handling both Access Point (AP) and Station (STA) modes.
 * 
 * This class automates the process of loading credentials from the filesystem (LittleFS),
 * setting up a local configuration Access Point, and connecting to an external WiFi network.
 * It provides visual feedback via a DisplayHandler.
 */
class WifiHandler {
    private:
        DisplayHandler display; ///< Instance for visual feedback on the OLED/LCD.
        String ssid;            ///< Stores the current Access Point SSID.
        String psw;             ///< Stores the current Access Point password.

    public:
        /**
         * @brief Initializes the WiFi system and filesystem.
         * 
         * Performs the following steps:
         * 1. Starts the display.
         * 2. Loads credentials from "/wifi.json" using LittleFS.
         * 3. Creates default credentials if the file is missing or invalid.
         * 4. Sets WiFi mode to AP+STA.
         * 5. Activates the local Access Point and attempts to connect to saved WiFi.
         */
        void begin();
        
        /**
         * @brief Activates the ESP32 Access Point mode.
         * 
         * @param ssid The SSID for the created network.
         * @param psw The password for the created network.
         * @return true If the Access Point was started successfully.
         * @return false If the AP failed to start.
         */
        bool ACturnOn(String ssid, String psw);

        /**
         * @brief Connects the ESP32 to an external WiFi network (Station mode).
         * 
         * This method trims whitespace from inputs and attempts to connect.
         * It features a 20-second timeout mechanism.
         * 
         * @param ssid The SSID of the target network.
         * @param psw The password of the target network.
         * @return true If the connection was established (WL_CONNECTED).
         * @return false If the connection failed or timed out.
         */
        bool WifiConnect(String ssid, String psw);
};

#endif 