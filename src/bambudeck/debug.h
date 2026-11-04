#ifndef _BAMBUDECK_DEBUG
#define _BAMBUDECK_DEBUG

#include <Arduino.h>
#include <ArduinoJson.h>

#define BAMBUDECK_DEBUG_LEVEL_LOG   1
#define BAMBUDECK_DEBUG_LEVEL_INFO  2
#define BAMBUDECK_DEBUG_LEVEL_DEBUG 3
#define BAMBUDECK_DEBUG_LEVEL_ERROR 4

#ifndef BAMBUDECK_LOG_LEVEL
#define BAMBUDECK_LOG_LEVEL BAMBUDECK_DEBUG_LEVEL_DEBUG
#endif

#define ConsoleLog   if (BAMBUDECK_LOG_LEVEL >= BAMBUDECK_DEBUG_LEVEL_LOG)   Serial
#define ConsoleInfo  if (BAMBUDECK_LOG_LEVEL >= BAMBUDECK_DEBUG_LEVEL_INFO)  Serial
#define ConsoleDebug if (BAMBUDECK_LOG_LEVEL >= BAMBUDECK_DEBUG_LEVEL_DEBUG) Serial
#define ConsoleError if (BAMBUDECK_LOG_LEVEL >= BAMBUDECK_DEBUG_LEVEL_ERROR) Serial

inline void bambudeck_debug_json(const JsonDocument &doc)
{
#if BAMBUDECK_LOG_LEVEL >= BAMBUDECK_DEBUG_LEVEL_DEBUG
    String output;
    serializeJsonPretty(doc, output);
    Serial.println(output);
#endif
}

#endif
