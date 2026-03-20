// WebManager.h implementation
#include "WebManager.h"

#include <LittleFS.h>

WebManager::WebType WebManager::begin() {
    
    // handle some IOS conflict
    server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", "<html><body>Success</body></html>");
    });

    server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(204);
    });

    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(204); 
    });

    //dynamic redirecting
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        String path;
        if (request->client()->localIP() == WiFi.softAPIP()) {
            path = "/esp32_webapp/index.html";
            Serial.println("Web: Accesso da Access Point -> esp32_webapp");
        } else {
            path = "/external_webapp/index.html";
            Serial.println("Web: Accesso da Station -> external_webapp");
        }
        
        if (LittleFS.exists(path)) {
            
            AsyncWebServerResponse *response = request->beginResponse(LittleFS, path, "text/html");
            response->addHeader("Connection", "close");
            request->send(response);
        } else {
            request->send(404, "text/plain", "Errore: File Index non trovato");
        }
    });

    server.onNotFound([this](AsyncWebServerRequest *request){
        String url = request->url();
        
        if (url.endsWith("/")) {
            request->send(404);
            return;
        }

        String fullPath = "";
        String folderAP = "/esp32_webapp";
        String folderSTA = "/external_webapp";

        if (url.startsWith(folderAP) || url.startsWith(folderSTA)) {
            fullPath = url;
        } else {
            String prefix = (request->client()->localIP() == WiFi.softAPIP()) ? folderAP : folderSTA;
            fullPath = prefix + url;
        }

        fullPath.replace("//", "/");

        if (LittleFS.exists(fullPath)) {
            AsyncWebServerResponse *response = request->beginResponse(LittleFS, fullPath, String(), false);
            //Safari handler
            response->addHeader("Connection", "close"); 
            response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
            response->addHeader("Pragma", "no-cache");
            response->addHeader("Expires", "0");
            
            request->send(response);
        } else {
            Serial.printf("Web: 404 Not Found -> %s\n", fullPath.c_str());
            request->send(404);
        }
    });
    
    //Websocket configuration
    ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        
        if (type == WS_EVT_CONNECT) {
            Serial.printf("WS: Client #%u connesso da %s\n", client->id(), client->remoteIP().toString().c_str());
            client->text("STATUS:online");
        } 
        
        else if (type == WS_EVT_DISCONNECT) {
            Serial.printf("WS: Client #%u disconnesso\n", client->id());
        } 
        
        else if (type == WS_EVT_DATA) {
            AwsFrameInfo *info = (AwsFrameInfo*)arg;
            if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                data[len] = 0; 
                this->getData(arg, data, len);
            }
        }
        
        else if (type == WS_EVT_PONG) {
            Serial.printf("WS: Client #%u PONG\n", client->id());
        }
    });

    server.addHandler(&ws);
    server.begin();
    
    Serial.println("Web Server Online");
    return SERVER_ONLINE;
}

WebManager::WebType WebManager::updateColor(RGB rgb) {
    JsonDocument doc;
    doc["r"] = rgb.r;
    doc["g"] = rgb.g;
    doc["b"] = rgb.b;
    doc["name"] = (rgb.color_name != "") ? rgb.color_name : "Unknown";

    String msg;
    serializeJson(doc, msg); 
    
    msg = "COLOR_JSON:" + msg;

    Serial.println("Sending to WS: " + msg);
    
    ws.textAll(msg);
    
    
    return COLOR_UPDATED;
}

void WebManager::getData(void *arg, uint8_t *data, size_t len) {
    JsonDocument doc;

    // Deserializzation with error check
    DeserializationError error = deserializeJson(doc, data, len);
    if (error) {
        Serial.print("JSON Deserialization failed: ");
        Serial.println(error.f_str());
        return;
    }

    const char* type = doc["type"]; 

    if (type && strcmp(type, "SET_WIFI") == 0) {
        String ssid = doc["ssid"] | "";
        String psw = doc["psw"] | "";

        if (ssid != "") {
            LittleFSHandler fs;
            JsonDocument json;
            json["ssid_wifi"] =ssid.c_str();
            json["psw_wifi"] = psw.c_str();
            fs.saveFS_json(json, "/wifi.json");
            
            new_credentials = true;

            ws.textAll("STATUS:connected");
            
            Serial.println("Credenziali salvate e conferma inviata al client.");
        } else {
            ws.textAll("STATUS:error_empty_ssid");
        }
    } 
    else {
        Serial.println("Tipo di pacchetto sconosciuto");
    }
}


void WebManager::cleanup() {
    ws.cleanupClients();
}