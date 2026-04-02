#ifndef WEBMANAGER_H
#define WEBMANAGER_H

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "Types.h"
#include "WifiHandler.h"

#pragma once

/**
 * @class WebManager
 * @brief Handles HTTP serving and real-time WebSocket communication.
 * 
 * This class manages an asynchronous web server that dynamically serves different 
 * web applications based on the connection type (Access Point vs. Station).
 * It also handles WebSocket events for real-time data exchange (e.g., RGB updates, WiFi setup).
 */
class WebManager {
private:
    AsyncWebServer server;      ///< Asynchronous HTTP server instance on port 80.
    AsyncWebSocket ws;          ///< WebSocket instance mapped to "/ws".
    String ac_webapp_path;      ///< Path to the webapp served when in AP mode.
    String wifi_webapp_path;    ///< Path to the webapp served when in STA mode.

public:
    /**
     * @brief Flag indicating if new WiFi credentials have been received via WebSocket.
     */
    bool new_credentials = false;

    /**
     * @brief Constructor for WebManager.
     * Initializes the server on port 80 and the WebSocket endpoint at "/ws".
     */
    WebManager() : server(80), ws("/ws") {
        ac_webapp_path = "/esp32_webapp";
        wifi_webapp_path = "/external_webapp";
    }

    /**
     * @enum WebType
     * @brief Return status codes for WebManager operations.
     */
    typedef enum {
        SERVER_ONLINE,      ///< Server and WebSocket started successfully.
        FAILED_TO_CONNECT,  ///< Reserved for potential connection errors.
        COLOR_UPDATED       ///< RGB data was successfully broadcasted to clients.
    } WebType;

    /**
     * @brief Configures and starts the Web Server and WebSocket handler.
     * 
     * Key functionalities:
     * - Fixes captive portal detection for iOS (hotspot-detect.html).
     * - Implements dynamic routing: serves index.html from `/esp32_webapp` if the user 
     *   is on the AP, or from `/external_webapp` if on the local network.
     * - Sets up the global `onNotFound` handler to serve static assets from the 
     *   correct folder based on the client's IP.
     * - Configures WebSocket events (Connect, Disconnect, Data, Pong).
     * 
     * @return SERVER_ONLINE upon successful initialization.
     */
    WebType begin();
    
    /**
     * @brief Broadcasts RGB color data to all connected WebSocket clients.
     * 
     * Serializes an RGB object into a JSON string prefixed with "COLOR_JSON:".
     * 
     * @param rgb The RGB structure containing r, g, b values and an optional color name.
     * @return COLOR_UPDATED after the message is sent.
     */
    WebType updateColor(RGB rgb);

    /**
     * @brief Internal handler for processing incoming WebSocket data.
     * 
     * Specifically handles:
     * - "SET_WIFI" packets: extracts SSID/Password and saves them to "/wifi.json".
     * - Sets the `new_credentials` flag to true upon successful save.
     * 
     * @param arg Pointer to frame info.
     * @param data The raw data buffer received.
     * @param len The length of the data buffer.
     */
    void getData(void *arg, uint8_t *data, size_t len);

    /**
     * @brief Performs periodic WebSocket cleanup.
     * 
     * Should be called in the main loop to clear stale client connections 
     * and manage memory efficiency.
     */
    void cleanup();
};

#endif 