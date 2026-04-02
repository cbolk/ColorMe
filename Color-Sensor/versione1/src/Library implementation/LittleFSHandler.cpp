//implementation of LittleFSHandler.h

#include "LittleFSHandler.h"

#include <LittleFS.h>



void LittleFSHandler :: begin(){
    if(!LittleFS.begin(true)){
     Serial.println("Ancora errore critico su LittleFS!");
    } else {
        Serial.println("LittleFS montato correttamente!");
    } 
    LittleFSHandler :: printFS("/", 2,0);
}

LittleFSHandler :: fs_status LittleFSHandler::printFS(const char * dirname, uint8_t levels, int indent) {
    File root = LittleFS.open(dirname);
    if (!root || !root.isDirectory()) {
        return FS_NOT_FOUND;
    }

    File file = root.openNextFile();
    while (file) {
        // Crea lo spazio per l'indentazione
        String space = "";
        for (int i = 0; i < indent; i++) space += "    ";

        if (file.isDirectory()) {
            // Se è una cartella, aggiungi la freccia e lo slash
            Serial.printf("%s-> %s/\n", space.c_str(), file.name());
            
            // Se non abbiamo raggiunto il limite di livelli, entra nella cartella
            if (levels > 0) {
                // Costruisce il percorso completo per la ricorsione
                String path = String(dirname);
                if (!path.endsWith("/")) path += "/";
                path += file.name();
                
                printFS(path.c_str(), levels - 1, indent + 1);
            }
        } else {
            // Se è un file, stampa solo il nome (indentato)
            Serial.printf("%s   %s (%u bytes)\n", space.c_str(), file.name(), file.size());
        }
        file = root.openNextFile();
    }

    return OPERATION_DONE;
}

LittleFSHandler::fs_status LittleFSHandler::saveFS_json(JsonDocument& nuoviDati, const char* path) {
    Serial.println("--- JSON Update (Object Input) ---");
    JsonDocument docEsistente;

    // 1. LEGGI il file attuale (se esiste)
    if (LittleFS.exists(path)) {
        File readFile = LittleFS.open(path, "r");
        if (readFile) {
            DeserializationError error = deserializeJson(docEsistente, readFile);
            readFile.close();
            if (error) Serial.println("File esistente corrotto, creo nuovo documento.");
        }
    }

    // 2. UNISCI (Merge) i dati
    // Questo ciclo scorre le chiavi del JSON in input e le sovrascrive nel documento principale
    JsonObject objNuovi = nuoviDati.as<JsonObject>();
    for (JsonPair p : objNuovi) {
        docEsistente[p.key()] = p.value();
    }

    // 3. SCRIVI il risultato finale
    File writeFile = LittleFS.open(path, "w");
    if (!writeFile) return FAILED_WRITING;

    if (serializeJson(docEsistente, writeFile) == 0) {
        writeFile.close();
        return FAILED_WRITING;
    }

    writeFile.close();
    Serial.println("--- JSON Merge & Save Completato ---");
    return OPERATION_DONE;
}

bool LittleFSHandler :: loadFS_json(std :: vector<String>& output, const std :: vector<String>& data, const char* path){

    if (!LittleFS.exists(path)) { // check if the path is right
        Serial.println("File path not found!");
        return {}; 
    }

    File file = LittleFS.open(path, "r");
    if (!file) { //check if the file exist and is ok
        Serial.println("Cannot read file");
        return {};
    }
    //JSON Decode
    {
        JsonDocument json;
        output.reserve(data.size());
        if(deserializeJson(json, file))     return false; // check if the json is ok
        file.close(); 
        for(const String& d : data)     output.push_back(json[d].as<String>()); //collecting all the output data
        for (const String& d:output) Serial.println(d);
        Serial.println("data loaded succesfully!");
    }
    
    return true;

}

LittleFSHandler :: fs_status LittleFSHandler :: new_file(JsonDocument& new_file_data, const char* path){

    File new_file = LittleFS.open(path,"w");
    serializeJson(new_file_data,new_file);
    new_file.close();
    return OPERATION_DONE;
}