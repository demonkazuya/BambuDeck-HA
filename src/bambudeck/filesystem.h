#ifndef _BAMBUDECK_FILESYSTEM
#define _BAMBUDECK_FILESYSTEM

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "debug.h"
#include "paths.h"

inline bool bambudeck_filesystem_init()
{
    if (!LittleFS.begin(true))
    {
        ConsoleError.println(F("[BambuDeck][FS] LittleFS mount failed"));
        return false;
    }
    if (!LittleFS.exists(bambudeck_paths_root))
    {
        LittleFS.mkdir(bambudeck_paths_root);
    }
    return true;
}

inline bool bambudeck_filesystem_exist(const char *path)
{
    return LittleFS.exists(path);
}

inline bool bambudeck_filesystem_exist(fs::FS &fs, const char *path)
{
    return fs.exists(path);
}

inline File bambudeck_filesystem_open(const char *path, const char *mode = FILE_READ)
{
    return LittleFS.open(path, mode);
}

inline File bambudeck_filesystem_open(fs::FS &fs, const char *path)
{
    return fs.open(path);
}

inline bool bambudeck_filesystem_deleteFile(const char *path)
{
    return LittleFS.remove(path);
}

inline bool bambudeck_filesystem_deleteFile(fs::FS &fs, const char *path)
{
    return fs.remove(path);
}

inline bool bambudeck_filesystem_mkdir(const char *path)
{
    if (!LittleFS.exists(path))
    {
        return LittleFS.mkdir(path);
    }
    return true;
}

inline bool bambudeck_filesystem_mkdir(fs::FS &fs, const char *path)
{
    if (!fs.exists(path))
    {
        return fs.mkdir(path);
    }
    return true;
}

inline fs::FS &bambudeck_get_fs()
{
    return LittleFS;
}

inline void bambudeck_filesystem_writeJson(const char *filename, const JsonDocument &json)
{
    ConsoleDebug.printf("[BambuDeck][FS] Writing JSON: %s\n", filename);
    if (!LittleFS.exists(bambudeck_paths_root))
    {
        LittleFS.mkdir(bambudeck_paths_root);
    }
    File f = LittleFS.open(filename, FILE_WRITE);
    if (!f)
    {
        ConsoleError.printf("[BambuDeck][FS] Failed to open for write: %s\n", filename);
        return;
    }
    serializeJson(json, f);
    f.flush();
    f.close();
}

inline void bambudeck_filesystem_writeJson(fs::FS &fs, const char *filename, const JsonDocument &json, bool defaultsToArray = false, int size = 1024)
{
    (void)fs;
    (void)defaultsToArray;
    (void)size;
    bambudeck_filesystem_writeJson(filename, json);
}

inline DynamicJsonDocument bambudeck_filesystem_readJson(const char *filename, bool defaultsToArray = false, int size = 1024)
{
    DynamicJsonDocument doc(size);
    if (!LittleFS.exists(filename))
    {
        ConsoleDebug.printf("[BambuDeck][FS] JSON file does not exist: %s\n", filename);
        if (defaultsToArray)
            doc.to<JsonArray>();
        else
            doc.to<JsonObject>();
        return doc;
    }

    File f = LittleFS.open(filename, FILE_READ);
    if (!f)
    {
        ConsoleError.printf("[BambuDeck][FS] Failed to open for read: %s\n", filename);
        if (defaultsToArray)
            doc.to<JsonArray>();
        else
            doc.to<JsonObject>();
        return doc;
    }

    DeserializationError err = deserializeJson(doc, f);
    f.close();

    if (err)
    {
        ConsoleError.printf("[BambuDeck][FS] Error parsing JSON %s: %s\n", filename, err.c_str());
        doc.clear();
        if (defaultsToArray)
            doc.to<JsonArray>();
        else
            doc.to<JsonObject>();
    }
    return doc;
}

inline DynamicJsonDocument bambudeck_filesystem_readJson(fs::FS &fs, const char *filename, bool defaultsToArray = false, int size = 1024)
{
    (void)fs;
    return bambudeck_filesystem_readJson(filename, defaultsToArray, size);
}

#endif
