#ifndef LITTLEFSHANDLER_H
#define LITTLEFSHANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

#pragma once

/**
 * @class LittleFSHandler
 * @brief Utility class for managing the LittleFS filesystem and JSON serialization.
 * 
 * Provides high-level methods to mount the FS, visualize the file structure, 
 * and perform advanced JSON operations like merging new data into existing files.
 */
class LittleFSHandler {
public:
    
    /**
     * @enum fs_status
     * @brief Result codes for filesystem operations.
     */
    typedef enum {
        OPERATION_DONE,         ///< Operation completed successfully.
        FS_NOT_FOUND,           ///< LittleFS is not mounted or path does not exist.
        CANNOT_OPEN_DIRECTORY,  ///< Failed to open the requested directory.
        WRONG_INPUT_FORMAT,     ///< Data provided does not match expected JSON format.
        FAILED_WRITING,         ///< Critical error during file write.
        FAILED_READING          ///< Critical error during file read or file corrupted.
    } fs_status;

    /**
     * @brief Mounts the LittleFS filesystem.
     * 
     * If mounting fails, it attempts to format the partition (if `true` is passed to begin).
     * Automatically calls `printFS` to show the root directory content on startup.
     */
    static void begin(); 

    /**
     * @brief Recursively prints the directory structure to the Serial monitor.
     * 
     * Useful for debugging. Displays file names, sizes, and directory hierarchy.
     * 
     * @param dirname Starting path (e.g., "/").
     * @param levels Depth of recursion allowed.
     * @param indent Internal use for visual formatting (leave at 0).
     * @return OPERATION_DONE on success, FS_NOT_FOUND if path is invalid.
     */
    static fs_status printFS(const char * dirname, uint8_t levels, int indent);

    /**
     * @brief Updates an existing JSON file by merging new data.
     * 
     * This method follows a Read-Modify-Write pattern:
     * 1. Opens the file at `path` and reads existing JSON.
     * 2. Iterates through `nuoviDati` and updates/adds keys to the existing document.
     * 3. Saves the merged result back to the same path.
     * 
     * @param nuoviDati JsonDocument containing the key-value pairs to update.
     * @param path The filesystem path to the .json file.
     * @return OPERATION_DONE or FAILED_WRITING.
     */
    fs_status saveFS_json(JsonDocument& nuoviDati, const char* path);

    /**
     * @brief Loads specific values from a JSON file into a vector of strings.
     * 
     * Maps a list of requested keys to their values stored in the file.
     * 
     * @param output Vector where the retrieved values will be stored.
     * @param data Vector containing the names of the keys to search for.
     * @param path Path to the .json file.
     * @return true if all data was loaded successfully, false if file is missing or corrupted.
     */
    bool loadFS_json(std :: vector<String>& output, const std :: vector<String>& data, const char* path);

    /**
     * @brief Creates a new file or overwrites an existing one with JSON data.
     * 
     * Unlike `saveFS_json`, this method does not perform a merge; it replaces 
     * the file content entirely.
     * 
     * @param new_file_data The JsonDocument to serialize.
     * @param path The filesystem path to create.
     * @return OPERATION_DONE.
     */
    static fs_status new_file(JsonDocument& new_file_data, const char* path);

};

#endif 