#ifndef _BAMBUDECK_JSONCONFIG
#define _BAMBUDECK_JSONCONFIG

#include <ArduinoJson.h>
#include <Preferences.h>
#include "filesystem.h"
#include "paths.h"
#include "types.h"
#include "debug.h"

/*
BambuDeck-HA Configuration Schema (/bambudeck.json and NVS namespace "bambudeck"):
{
  "wifi_ssid": "Your-WiFi-SSID",
  "wifi_password": "Your-WiFi-Password",
  "ha_url": "http://192.168.1.100:8123",
  "ha_token": "YOUR_LONG_LIVED_ACCESS_TOKEN",
  "name": "p1s",
  "model": "Universal",
  "chamber_sensor": true
}
*/

inline void bambudeck_nvs_save_config()
{
    Preferences prefs;
    if (!prefs.begin("bambudeck", false))
    {
        ConsoleError.println(F("[BambuDeck][NVS] Failed to open Preferences for writing"));
        return;
    }
    if (strlen(bambuDeckConfig.wifiSsid) > 0)
        prefs.putString("ssid", bambuDeckConfig.wifiSsid);
    if (strlen(bambuDeckConfig.wifiPassword) > 0)
        prefs.putString("pwd", bambuDeckConfig.wifiPassword);
    if (strlen(bambuDeckConfig.haUrl) > 0)
        prefs.putString("ha_url", bambuDeckConfig.haUrl);
    if (strlen(bambuDeckConfig.haToken) > 0)
        prefs.putString("ha_token", bambuDeckConfig.haToken);
    if (strlen(bambuDeckConfig.printerName) > 0)
        prefs.putString("name", bambuDeckConfig.printerName);
    if (strlen(bambuDeckConfig.printerModel) > 0)
        prefs.putString("model", bambuDeckConfig.printerModel);
    prefs.putBool("chamber", bambuDeckConfig.chamberSensorEnabled);
    prefs.end();
    ConsoleInfo.println(F("[BambuDeck][NVS] Configuration backed up to NVS flash"));
}

inline bool bambudeck_nvs_load_config()
{
    Preferences prefs;
    if (!prefs.begin("bambudeck", true))
    {
        return false;
    }
    String ssid = prefs.getString("ssid", "");
    String pwd = prefs.getString("pwd", "");
    String haUrl = prefs.getString("ha_url", "");
    String haToken = prefs.getString("ha_token", "");
    String name = prefs.getString("name", "");
    String model = prefs.getString("model", "Universal");
    bool chamber = prefs.getBool("chamber", true);
    prefs.end();

    if (ssid.length() == 0 && haUrl.length() == 0)
    {
        return false;
    }

    if (strlen(bambuDeckConfig.wifiSsid) == 0 && ssid.length() > 0)
        strncpy(bambuDeckConfig.wifiSsid, ssid.c_str(), sizeof(bambuDeckConfig.wifiSsid) - 1);
    if (strlen(bambuDeckConfig.wifiPassword) == 0 && pwd.length() > 0)
        strncpy(bambuDeckConfig.wifiPassword, pwd.c_str(), sizeof(bambuDeckConfig.wifiPassword) - 1);
    if (strlen(bambuDeckConfig.haUrl) == 0 && haUrl.length() > 0)
        strncpy(bambuDeckConfig.haUrl, haUrl.c_str(), sizeof(bambuDeckConfig.haUrl) - 1);
    if (strlen(bambuDeckConfig.haToken) == 0 && haToken.length() > 0)
        strncpy(bambuDeckConfig.haToken, haToken.c_str(), sizeof(bambuDeckConfig.haToken) - 1);
    if (strlen(bambuDeckConfig.printerName) == 0 && name.length() > 0)
        strncpy(bambuDeckConfig.printerName, name.c_str(), sizeof(bambuDeckConfig.printerName) - 1);
    if (strlen(bambuDeckConfig.printerModel) == 0 && model.length() > 0)
        strncpy(bambuDeckConfig.printerModel, model.c_str(), sizeof(bambuDeckConfig.printerModel) - 1);
    bambuDeckConfig.chamberSensorEnabled = chamber;

    ConsoleInfo.println(F("[BambuDeck][NVS] Configuration restored from NVS flash"));
    return true;
}

inline DynamicJsonDocument bambudeck_load_config()
{
    DynamicJsonDocument config = bambudeck_filesystem_readJson(bambudeck_paths_config, false, 2048);

    bambuDeckConfig.haUrl[0] = '\0';
    bambuDeckConfig.haToken[0] = '\0';
    bambuDeckConfig.printerName[0] = '\0';
    bambuDeckConfig.printerModel[0] = '\0';
    bambuDeckConfig.wifiSsid[0] = '\0';
    bambuDeckConfig.wifiPassword[0] = '\0';

    bool haveFsConfig = (!config.isNull() && config.size() > 0);

    if (haveFsConfig)
    {
        // WiFi SSID
        if (config.containsKey("wifi_ssid"))
            strncpy(bambuDeckConfig.wifiSsid, config["wifi_ssid"] | "", sizeof(bambuDeckConfig.wifiSsid) - 1);
        else if (config.containsKey("ssid"))
            strncpy(bambuDeckConfig.wifiSsid, config["ssid"] | "", sizeof(bambuDeckConfig.wifiSsid) - 1);

        // WiFi Password
        if (config.containsKey("wifi_password"))
            strncpy(bambuDeckConfig.wifiPassword, config["wifi_password"] | "", sizeof(bambuDeckConfig.wifiPassword) - 1);
        else if (config.containsKey("pwd"))
            strncpy(bambuDeckConfig.wifiPassword, config["pwd"] | "", sizeof(bambuDeckConfig.wifiPassword) - 1);

        // Home Assistant URL
        if (config.containsKey("ha_url"))
            strncpy(bambuDeckConfig.haUrl, config["ha_url"] | "", sizeof(bambuDeckConfig.haUrl) - 1);
        else if (config.containsKey("ha") && config["ha"].containsKey("url"))
            strncpy(bambuDeckConfig.haUrl, config["ha"]["url"] | "", sizeof(bambuDeckConfig.haUrl) - 1);

        // Home Assistant Token
        if (config.containsKey("ha_token"))
            strncpy(bambuDeckConfig.haToken, config["ha_token"] | "", sizeof(bambuDeckConfig.haToken) - 1);
        else if (config.containsKey("ha") && config["ha"].containsKey("token"))
            strncpy(bambuDeckConfig.haToken, config["ha"]["token"] | "", sizeof(bambuDeckConfig.haToken) - 1);

        // Printer Name / Prefix
        if (config.containsKey("name"))
            strncpy(bambuDeckConfig.printerName, config["name"] | "", sizeof(bambuDeckConfig.printerName) - 1);
        else if (config.containsKey("printerName"))
            strncpy(bambuDeckConfig.printerName, config["printerName"] | "", sizeof(bambuDeckConfig.printerName) - 1);

        // Printer Model
        if (config.containsKey("model"))
            strncpy(bambuDeckConfig.printerModel, config["model"] | "Universal", sizeof(bambuDeckConfig.printerModel) - 1);
        else
            strncpy(bambuDeckConfig.printerModel, "Universal", sizeof(bambuDeckConfig.printerModel) - 1);

        // Chamber sensor toggle
        if (config.containsKey("chamber_sensor"))
            bambuDeckConfig.chamberSensorEnabled = config["chamber_sensor"].as<bool>();
    }

    // Bidirectional redundancy check:
    // If LittleFS config was missing/corrupted or had no WiFi SSID, attempt restore from NVS
    if (strlen(bambuDeckConfig.wifiSsid) == 0 || strlen(bambuDeckConfig.haUrl) == 0)
    {
        if (bambudeck_nvs_load_config())
        {
            ConsoleInfo.println(F("[BambuDeck][Config] Restoring LittleFS config.json from NVS backup"));
            config["wifi_ssid"] = bambuDeckConfig.wifiSsid;
            config["wifi_password"] = bambuDeckConfig.wifiPassword;
            config["ssid"] = bambuDeckConfig.wifiSsid;
            config["pwd"] = bambuDeckConfig.wifiPassword;
            config["ha_url"] = bambuDeckConfig.haUrl;
            config["ha_token"] = bambuDeckConfig.haToken;
            config["name"] = bambuDeckConfig.printerName;
            config["printerName"] = bambuDeckConfig.printerName;
            config["model"] = bambuDeckConfig.printerModel;
            config["chamber_sensor"] = bambuDeckConfig.chamberSensorEnabled;
            bambudeck_filesystem_writeJson(bambudeck_paths_config, config);
        }
    }
    else
    {
        // LittleFS had valid config: sync to NVS to ensure survival across cold boot & brownout
        bambudeck_nvs_save_config();
    }

    if (strlen(bambuDeckConfig.printerModel) == 0)
    {
        strncpy(bambuDeckConfig.printerModel, "Universal", sizeof(bambuDeckConfig.printerModel) - 1);
    }

    return config;
}

inline void bambudeck_save_selected_printer(const char *printerPrefix)
{
    if (printerPrefix == NULL)
        return;

    strncpy(bambuDeckConfig.printerName, printerPrefix, sizeof(bambuDeckConfig.printerName) - 1);
    DynamicJsonDocument config = bambudeck_load_config();
    config["name"] = printerPrefix;
    config["printerName"] = printerPrefix;
    bambudeck_filesystem_writeJson(bambudeck_paths_config, config);
    bambudeck_nvs_save_config();
    ConsoleInfo.printf("[BambuDeck][Config] Saved selected printer: %s\n", printerPrefix);
}

#endif
