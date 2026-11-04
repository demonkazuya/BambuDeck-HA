#ifndef _BAMBUDECK_JSONCONFIG
#define _BAMBUDECK_JSONCONFIG

#include <ArduinoJson.h>
#include "filesystem.h"
#include "paths.h"
#include "types.h"
#include "debug.h"

/*
BambuDeck-HA Configuration Schema (/bambudeck.json):
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

inline DynamicJsonDocument bambudeck_load_config()
{
    DynamicJsonDocument config = bambudeck_filesystem_readJson(bambudeck_paths_config, false, 2048);

    bambuDeckConfig.haUrl[0] = '\0';
    bambuDeckConfig.haToken[0] = '\0';
    bambuDeckConfig.printerName[0] = '\0';
    bambuDeckConfig.printerModel[0] = '\0';
    bambuDeckConfig.wifiSsid[0] = '\0';
    bambuDeckConfig.wifiPassword[0] = '\0';

    if (config.isNull() || config.size() == 0)
    {
        ConsoleInfo.println(F("[BambuDeck][Config] No config found, defaults applied"));
        strncpy(bambuDeckConfig.printerModel, "Universal", sizeof(bambuDeckConfig.printerModel));
        return config;
    }

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
    ConsoleInfo.printf("[BambuDeck][Config] Saved selected printer: %s\n", printerPrefix);
}

#endif
