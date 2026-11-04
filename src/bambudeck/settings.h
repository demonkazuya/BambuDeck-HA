#ifndef _BAMBUDECK_SETTINGS
#define _BAMBUDECK_SETTINGS

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "filesystem.h"
#include "paths.h"
#include "types.h"

inline void bambudeck_settings_save(bool onlyRoot = false)
{
    (void)onlyRoot;
    DynamicJsonDocument doc(256);
    doc["backlight"] = bambuDeckConfig.backlightLevel;
    doc["tftOff"] = bambuDeckConfig.tftOffValue;
    doc["tftInvert"] = bambuDeckConfig.tftInvert;
    doc["ota"] = bambuDeckConfig.otaEnabled;
    doc["wop"] = bambuDeckConfig.wakeOnPrint;
    doc["lightMode"] = bambuDeckConfig.lightMode;
    doc["chamberTempDiff"] = bambuDeckConfig.chamberSensorReadingDiff;
    doc["chamberTemp"] = bambuDeckConfig.chamberSensorEnabled;
    doc["auxFan"] = bambuDeckConfig.auxFanEnabled;
    doc["chamberFan"] = bambuDeckConfig.chamberFanEnabled;

    bambudeck_filesystem_writeJson(bambudeck_paths_settings, doc);

    Preferences prefs;
    if (prefs.begin("bambudeck_set", false))
    {
        prefs.putInt("backlight", bambuDeckConfig.backlightLevel);
        prefs.putInt("tftOff", bambuDeckConfig.tftOffValue);
        prefs.putBool("tftInvert", bambuDeckConfig.tftInvert);
        prefs.putBool("ota", bambuDeckConfig.otaEnabled);
        prefs.putBool("wop", bambuDeckConfig.wakeOnPrint);
        prefs.putBool("lightMode", bambuDeckConfig.lightMode);
        prefs.putBool("chamberTemp", bambuDeckConfig.chamberSensorEnabled);
        prefs.end();
    }
}

inline void bambudeck_settings_loadSettings()
{
    if (!bambudeck_filesystem_exist(bambudeck_paths_settings))
    {
        bambuDeckConfig.backlightLevel = 128;
        bambuDeckConfig.tftOffValue = 15;
        bambuDeckConfig.tftInvert = false;
        bambuDeckConfig.otaEnabled = true;
        bambuDeckConfig.wakeOnPrint = true;
        bambuDeckConfig.lightMode = false;
        bambuDeckConfig.chamberSensorReadingDiff = 0;
        bambuDeckConfig.chamberSensorEnabled = true;
        bambuDeckConfig.auxFanEnabled = false;
        bambuDeckConfig.chamberFanEnabled = false;

        Preferences prefs;
        if (prefs.begin("bambudeck_set", true))
        {
            bambuDeckConfig.backlightLevel = prefs.getInt("backlight", 128);
            bambuDeckConfig.tftOffValue = prefs.getInt("tftOff", 15);
            bambuDeckConfig.tftInvert = prefs.getBool("tftInvert", false);
            bambuDeckConfig.otaEnabled = prefs.getBool("ota", true);
            bambuDeckConfig.wakeOnPrint = prefs.getBool("wop", true);
            bambuDeckConfig.lightMode = prefs.getBool("lightMode", false);
            bambuDeckConfig.chamberSensorEnabled = prefs.getBool("chamberTemp", true);
            prefs.end();
        }

        bambudeck_settings_save(true);
    }

    DynamicJsonDocument settings = bambudeck_filesystem_readJson(bambudeck_paths_settings);

    bambuDeckConfig.backlightLevel = settings.containsKey("backlight") ? settings["backlight"].as<int>() : 128;
    bambuDeckConfig.tftOffValue = settings.containsKey("tftOff") ? settings["tftOff"].as<int>() : 15;
    bambuDeckConfig.tftInvert = settings.containsKey("tftInvert") ? settings["tftInvert"].as<bool>() : false;
    bambuDeckConfig.otaEnabled = settings.containsKey("ota") ? settings["ota"].as<bool>() : false;
    bambuDeckConfig.wakeOnPrint = settings.containsKey("wop") ? settings["wop"].as<bool>() : true;
    bambuDeckConfig.lightMode = settings.containsKey("lightMode") ? settings["lightMode"].as<bool>() : false;
    bambuDeckConfig.chamberSensorReadingDiff = settings.containsKey("chamberTempDiff") ? settings["chamberTempDiff"].as<int8_t>() : 0;
    bambuDeckConfig.chamberSensorEnabled = settings.containsKey("chamberTemp") ? settings["chamberTemp"].as<bool>() : false;
    bambuDeckConfig.auxFanEnabled = settings.containsKey("auxFan") ? settings["auxFan"].as<bool>() : false;
    bambuDeckConfig.chamberFanEnabled = settings.containsKey("chamberFan") ? settings["chamberFan"].as<bool>() : false;

    // Sync to NVS
    Preferences prefs;
    if (prefs.begin("bambudeck_set", false))
    {
        prefs.putInt("backlight", bambuDeckConfig.backlightLevel);
        prefs.putInt("tftOff", bambuDeckConfig.tftOffValue);
        prefs.putBool("tftInvert", bambuDeckConfig.tftInvert);
        prefs.putBool("ota", bambuDeckConfig.otaEnabled);
        prefs.putBool("wop", bambuDeckConfig.wakeOnPrint);
        prefs.putBool("lightMode", bambuDeckConfig.lightMode);
        prefs.putBool("chamberTemp", bambuDeckConfig.chamberSensorEnabled);
        prefs.end();
    }

    bool isTFTFlipped = bambudeck_screen_getTFTFlip();
    tft.setRotation(isTFTFlipped ? 3 : 1);
    bambudeck_touchScreen.setRotation(isTFTFlipped ? 3 : 1);
    bambudeck_screen_setBrightness(bambuDeckConfig.backlightLevel);

    bambudeck_screen_invertColors();
}

#endif
