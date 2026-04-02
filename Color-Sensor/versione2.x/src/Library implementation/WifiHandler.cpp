//WifiHandler.h implementation
#include "WifiHandler.h"
#define NETWORK_JSON_PATH "/wifi.json"
const std::vector<String> KEY_NAME_NETWORK_JSON = {"ssid_ac","psw_ac","ssid_wifi","psw_wifi"};


void WifiHandler :: begin(){
    {
            display.begin();

            LittleFSHandler fs;
            Serial.println("loading data");
            std::vector<String> data;
            data.reserve(KEY_NAME_NETWORK_JSON.size());
            fs.loadFS_json(data,KEY_NAME_NETWORK_JSON,NETWORK_JSON_PATH);
            while(data.size()<KEY_NAME_NETWORK_JSON.size()){
                JsonDocument json;
                json[KEY_NAME_NETWORK_JSON[0]] = "ESP32_ACCES";
                json[KEY_NAME_NETWORK_JSON[1]] = "ESP32_PSW";
                json[KEY_NAME_NETWORK_JSON[2]] = "";
                json[KEY_NAME_NETWORK_JSON[3]] = "";
                fs.new_file(json,NETWORK_JSON_PATH);
                if(!fs.loadFS_json(data,KEY_NAME_NETWORK_JSON,NETWORK_JSON_PATH))
                   display.print("Error while loading"); 
            }

            this->ssid = data[0];
            this->psw = data[1];
            Serial.print("ssid_ac   ");Serial.println(ssid);
            Serial.print("psw_ac   ");Serial.println(psw);
            Serial.print("ssid_wifi   ");Serial.println(data[2]);
            Serial.print("psw_wifi   ");Serial.println(data[3]);
            WiFi.disconnect(true);
            delay(500);
            WiFi.mode(WIFI_AP_STA); //act as an AccesPoint as a Client
            delay(100);
            if(!ACturnOn(ssid,psw))
                display.print("Something went wrong, please restart","");
            delay(1000);
            if(data[2] != "")
                WifiConnect(data[2], data[3]);
            
        }
}

bool WifiHandler :: ACturnOn(String ssid, String psw){
    
    display.print("Starting AC ");
    delay(500);

    int start=millis();
    
    if(WiFi.softAP(ssid.c_str(),psw.c_str())){
        display.print("AC online!","IP : "+WiFi.softAPIP().toString() );  
        Serial.print("AC online! ");Serial.print("IP : "); Serial.println(WiFi.softAPIP().toString() );

        return true;
    }
    
    return false;
}   



bool WifiHandler :: WifiConnect(String ssid, String psw){
    WiFi.disconnect();
    Serial.println("WIFI CONNECTION");
    display.print("WIFI CONNECTING");
    delay(500);
    if (ssid.length() == 0) {
        display.print("WiFi Error:", "SSID is empty");
        delay(500);
        return false;
    }

    display.print("Connecting to ",ssid);
    Serial.print("Connecting to");Serial.print(ssid);Serial.print(psw);Serial.println("##");
    ssid.trim();
    psw.trim();
    delay(1000);

    

    WiFi.begin(ssid.c_str(), psw.c_str());
    


    int start=millis();
    while(WiFi.status() != WL_CONNECTED){
        //Serial.print(WiFi.status());
        display.print("connecting");
        delay(500);
        if(millis()-start > 20000){
                display.print("Cannot connceted to WIFI!", "IP: " + WiFi.localIP().toString());
                Serial.print("Cannot connceted to WIFI! Status: ");Serial.print(WiFi.status());
                return false;
            }
    }

    display.print("Succesfully connceted to WIFI!", "IP: " + WiFi.localIP().toString());
    Serial.println("Succesfully connceted to WIFI! IP: ");Serial.print(WiFi.localIP().toString());

    return true;
}
