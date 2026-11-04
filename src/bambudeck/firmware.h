#ifndef _BAMBUDECK_FIRMWARE
#define _BAMBUDECK_FIRMWARE

#include <Arduino.h>
#include <Update.h>
#include <SD.h>
#include <stdio.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

struct SemVer
{
    int major;
    int minor;
    int patch;
};

inline bool parseSemVer(const char *version, SemVer &semver)
{
    int parsed = sscanf(version, "%d.%d.%d", &semver.major, &semver.minor, &semver.patch);
    return parsed == 3;
}

inline int compareSemVer(const SemVer &v1, const SemVer &v2)
{
    if (v1.major != v2.major)
    {
        return v1.major - v2.major;
    }
    if (v1.minor != v2.minor)
    {
        return v1.minor - v2.minor;
    }
    if (v1.patch != v2.patch)
    {
        return v1.patch - v2.patch;
    }
    return 0;
}

inline bool isSemVerGreaterThan(const SemVer &v1, const SemVer &v2)
{
    return compareSemVer(v1, v2) > 0;
}

#define BAMBUDECK_FIRMWARE_DOWNLOAD_RETRIES 5
static byte bambudeck_firmware_updateDownloadRetries = 0;

inline bool bambudeck_firmware_semverNeedsUpdate(const char *compare)
{
    SemVer version1, version2;

    if (parseSemVer(BAMBUDECK_FIRMWARE_VERSION, version1) && parseSemVer(compare, version2))
    {
        if (isSemVerGreaterThan(version2, version1))
        {
            return true;
        }
        return false;
    }

    return false;
}

static lv_obj_t *bambudeck_ota_modal = NULL;
static lv_obj_t *bambudeck_ota_bar = NULL;
static lv_obj_t *bambudeck_ota_title = NULL;
static lv_obj_t *bambudeck_ota_percent = NULL;
static int bambudeck_ota_last_reported_pct = -1;

inline void bambudeck_ota_progress_show(const char *title)
{
    if (bambudeck_ota_modal != NULL)
    {
        lv_obj_del(bambudeck_ota_modal);
        bambudeck_ota_modal = NULL;
    }

    bambudeck_ota_last_reported_pct = -1;

    bambudeck_ota_modal = lv_obj_create(lv_layer_top());
    lv_obj_set_size(bambudeck_ota_modal, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(bambudeck_ota_modal, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bambudeck_ota_modal, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(bambudeck_ota_modal, LV_OBJ_FLAG_FLOATING);
    lv_obj_clear_flag(bambudeck_ota_modal, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(bambudeck_ota_modal, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bambudeck_ota_modal, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bambudeck_ota_modal, 190, LV_PART_MAIN);
    lv_obj_set_style_border_width(bambudeck_ota_modal, 0, LV_PART_MAIN);

    lv_obj_t *card = lv_obj_create(bambudeck_ota_modal);
    lv_obj_set_width(card, 260);
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(card, 10, LV_PART_MAIN);
    lv_obj_set_style_bg_color(card, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(card, 255, LV_PART_MAIN);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(card, BAMBU_COLOR_BORDER(), LV_PART_MAIN);
    lv_obj_set_style_pad_all(card, 16, LV_PART_MAIN);

    bambudeck_ota_title = lv_label_create(card);
    lv_obj_set_width(bambudeck_ota_title, lv_pct(100));
    lv_obj_set_style_text_align(bambudeck_ota_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(bambudeck_ota_title, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN);
    lv_obj_set_style_text_font(bambudeck_ota_title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(bambudeck_ota_title, title ? title : "Updating Firmware...");

    bambudeck_ota_bar = lv_bar_create(card);
    lv_obj_set_size(bambudeck_ota_bar, 220, 14);
    lv_bar_set_range(bambudeck_ota_bar, 0, 100);
    lv_bar_set_value(bambudeck_ota_bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_radius(bambudeck_ota_bar, 6, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bambudeck_ota_bar, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN);
    lv_obj_set_style_radius(bambudeck_ota_bar, 6, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(bambudeck_ota_bar, BAMBU_COLOR_GREEN(), LV_PART_INDICATOR);
    lv_obj_set_style_pad_top(bambudeck_ota_bar, 12, LV_PART_MAIN);

    bambudeck_ota_percent = lv_label_create(card);
    lv_obj_set_width(bambudeck_ota_percent, lv_pct(100));
    lv_obj_set_style_text_align(bambudeck_ota_percent, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(bambudeck_ota_percent, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN);
    lv_obj_set_style_text_font(bambudeck_ota_percent, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_pad_top(bambudeck_ota_percent, 8, LV_PART_MAIN);
    lv_label_set_text(bambudeck_ota_percent, "0% (0 KB)");

    lv_timer_handler();
    lv_task_handler();
}

inline void bambudeck_ota_progress_update(size_t currSize, size_t totalSize)
{
    if (totalSize == 0) return;
    int progress = (int)((currSize * 100) / totalSize);
    if (progress < 0) progress = 0;
    if (progress > 100) progress = 100;

    if (progress == bambudeck_ota_last_reported_pct && progress < 100)
    {
        return;
    }
    bambudeck_ota_last_reported_pct = progress;

    if (bambudeck_ota_bar != NULL)
    {
        lv_bar_set_value(bambudeck_ota_bar, progress, LV_ANIM_OFF);
    }
    if (bambudeck_ota_percent != NULL)
    {
        lv_label_set_text_fmt(bambudeck_ota_percent, "%d%% (%d / %d KB)", progress, (int)(currSize / 1024), (int)(totalSize / 1024));
    }
    if (introScreenCaption != NULL && lv_obj_is_valid(introScreenCaption))
    {
        lv_label_set_text_fmt(introScreenCaption, LV_SYMBOL_CHARGE " Updating %d%%", progress);
    }
    lv_timer_handler();
    lv_task_handler();
}

inline void bambudeck_ota_progress_finish(void)
{
    if (bambudeck_ota_title != NULL)
    {
        lv_label_set_text(bambudeck_ota_title, LV_SYMBOL_OK " Update Finished! Rebooting...");
    }
    if (bambudeck_ota_bar != NULL)
    {
        lv_bar_set_value(bambudeck_ota_bar, 100, LV_ANIM_OFF);
    }
    if (bambudeck_ota_percent != NULL)
    {
        lv_label_set_text(bambudeck_ota_percent, "100% Done");
    }
    if (introScreenCaption != NULL && lv_obj_is_valid(introScreenCaption))
    {
        lv_label_set_text(introScreenCaption, LV_SYMBOL_OK " Update Finished! Rebooting...");
    }
    lv_timer_handler();
    lv_task_handler();
    delay(1500);
}

inline void bambudeck_ota_progress_close(void)
{
    if (bambudeck_ota_modal != NULL)
    {
        lv_obj_del(bambudeck_ota_modal);
        bambudeck_ota_modal = NULL;
        bambudeck_ota_bar = NULL;
        bambudeck_ota_title = NULL;
        bambudeck_ota_percent = NULL;
        lv_timer_handler();
        lv_task_handler();
    }
}

inline void bambudeck_firmware_onProgress(size_t currSize, size_t totalSize)
{
    bambudeck_ota_progress_update(currSize, totalSize);
}

// Fetch ota.json string from GitHub Releases via HTTPClient
inline bool bambudeck_fetch_ota_manifest(String &outJson)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        ConsoleError.println(F("[BambuDeck][OTA] WiFi not connected"));
        return false;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(15000); // 15 seconds

    HTTPClient http;
    http.setUserAgent("Mozilla/5.0 (ESP32; BambuDeck-OTA)");
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.setRedirectLimit(5);
    http.setTimeout(15000);

    if (!http.begin(client, bambudeck_paths_firmware_ota_file))
    {
        ConsoleError.println(F("[BambuDeck][OTA] http.begin failed"));
        return false;
    }

    int code = http.GET();
    if (code == HTTP_CODE_OK)
    {
        outJson = http.getString();
        http.end();
        ConsoleInfo.printf("[BambuDeck][OTA] Manifest received (%d bytes)\n", outJson.length());
        return true;
    }

    ConsoleError.printf("[BambuDeck][OTA] Fetch manifest failed HTTP %d: %s\n", code, http.errorToString(code).c_str());
    http.end();
    return false;
}

// Direct stream HTTP OTA flash into flash partition (No SD card required!)
inline bool bambudeck_perform_direct_http_ota(const char *url, const char *expectedMd5 = NULL)
{
    if (WiFi.status() != WL_CONNECTED || !url || strlen(url) == 0)
        return false;

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(30000); // 30 seconds

    HTTPClient http;
    http.setUserAgent("Mozilla/5.0 (ESP32; BambuDeck-OTA)");
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.setRedirectLimit(5);
    http.setTimeout(30000);

    if (!http.begin(client, url))
        return false;

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK)
    {
        ConsoleError.printf("[BambuDeck][OTA] Stream firmware failed HTTP %d: %s\n", httpCode, http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }

    int contentLength = http.getSize();
    if (contentLength <= 0)
    {
        ConsoleError.println(F("[BambuDeck][OTA] Invalid content length for firmware"));
        http.end();
        return false;
    }

    if (!bambudeck_ota_modal)
    {
        bambudeck_ota_progress_show(LV_SYMBOL_DOWNLOAD " Installing Firmware...");
    }

    bambudeck_ota_last_reported_pct = -1;
    Update.onProgress(bambudeck_firmware_onProgress);

    if (!Update.begin(contentLength, U_FLASH))
    {
        ConsoleError.printf("[BambuDeck][OTA] Update.begin failed: %s\n", Update.errorString());
        http.end();
        return false;
    }

    if (expectedMd5 && strlen(expectedMd5) > 0)
    {
        Update.setMD5(expectedMd5);
    }

    WiFiClient *stream = http.getStreamPtr();
    size_t written = Update.writeStream(*stream);
    http.end();

    if (written != (size_t)contentLength)
    {
        ConsoleError.printf("[BambuDeck][OTA] Written only %d/%d bytes\n", (int)written, contentLength);
        return false;
    }

    if (!Update.end(true))
    {
        ConsoleError.printf("[BambuDeck][OTA] Update.end failed: %s\n", Update.errorString());
        return false;
    }

    ConsoleInfo.println(F("[BambuDeck][OTA] Direct HTTP OTA flash successful!"));
    bambudeck_ota_progress_finish();
    return true;
}

inline void bambudeck_firmware_checkOnlineFirmwareUpdate(void)
{
    if (!bambuDeckConfig.otaEnabled || WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    ConsoleInfo.println(F("[BambuDeck][OTA] Checking for firmware updates on boot..."));
    lv_label_set_text(introScreenCaption, LV_SYMBOL_CHARGE " Checking for Updates...");
    lv_timer_handler();
    lv_task_handler();

    String jsonStr;
    if (!bambudeck_fetch_ota_manifest(jsonStr))
    {
        ConsoleError.println(F("[BambuDeck][OTA] Boot check failed: could not reach manifest"));
        return;
    }

    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, jsonStr);
    if (err)
    {
        ConsoleError.printf("[BambuDeck][OTA] Boot check JSON parse error: %s\n", err.c_str());
        return;
    }

    const char *remoteVer = doc["version"] | "";
    const char *downloadUrl = doc["url"] | "";
    const char *remoteMd5 = doc["md5"] | "";

    ConsoleInfo.printf("[BambuDeck][OTA] Boot check: Remote v%s, Current v%s\n", remoteVer, BAMBUDECK_FIRMWARE_VERSION);

    if (bambudeck_firmware_semverNeedsUpdate(remoteVer))
    {
        ConsoleInfo.printf("[BambuDeck][OTA] Boot update found: v%s -> installing...\n", remoteVer);
        bambudeck_ota_progress_show(LV_SYMBOL_DOWNLOAD " Installing Update...");
        bool ok = bambudeck_perform_direct_http_ota(downloadUrl, remoteMd5);
        if (ok)
        {
            ESP.restart();
        }
        else
        {
            bambudeck_ota_progress_close();
            lv_label_set_text(introScreenCaption, LV_SYMBOL_WARNING " OTA Install Failed");
            lv_timer_handler();
            lv_task_handler();
            delay(2000);
        }
    }
    else
    {
        ConsoleInfo.println(F("[BambuDeck][OTA] Boot check: Firmware is up to date"));
        lv_label_set_text_fmt(introScreenCaption, LV_SYMBOL_OK " Firmware Up to Date (v%s)", BAMBUDECK_FIRMWARE_VERSION);
        lv_timer_handler();
        lv_task_handler();
        delay(600);
    }
}

// Boot firmware check hook
inline void bambudeck_firmware_checkFirmwareUpdate(void)
{
}

// Interactive check from Settings Menu
inline void bambudeck_firmware_interactive_check(void)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        ui_confirmPanel_show_custom(LV_SYMBOL_WARNING " WiFi Not Connected", "OK", NULL, ui_confirmPanel_NOOP);
        return;
    }

    String jsonStr;
    bool ok = bambudeck_fetch_ota_manifest(jsonStr);
    if (!ok)
    {
        ui_confirmPanel_show_custom(LV_SYMBOL_WARNING " Could not reach\nGitHub Release server", "OK", NULL, ui_confirmPanel_NOOP);
        return;
    }

    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, jsonStr);
    if (err)
    {
        ui_confirmPanel_show_custom(LV_SYMBOL_WARNING " Invalid release data", "OK", NULL, ui_confirmPanel_NOOP);
        return;
    }

    const char *remoteVer = doc["version"] | "";
    if (bambudeck_firmware_semverNeedsUpdate(remoteVer))
    {
        static char msg[128];
        snprintf(msg, sizeof(msg), LV_SYMBOL_DOWNLOAD " New Version v%s!\n\nCurrent: v%s\nUpdate now?", remoteVer, BAMBUDECK_FIRMWARE_VERSION);
        ui_confirmPanel_show_custom(msg, "Update", "Cancel", []() {
            // User confirmed update
            bambudeck_ota_progress_show(LV_SYMBOL_DOWNLOAD " Downloading & Flashing...");
            String jStr;
            if (bambudeck_fetch_ota_manifest(jStr))
            {
                DynamicJsonDocument d(512);
                deserializeJson(d, jStr);
                const char *url = d["url"] | "";
                const char *md5 = d["md5"] | "";
                bool success = bambudeck_perform_direct_http_ota(url, md5);
                if (success)
                {
                    ESP.restart();
                }
                else
                {
                    bambudeck_ota_progress_close();
                    ui_confirmPanel_show_custom(LV_SYMBOL_WARNING " OTA Update Failed", "OK", NULL, ui_confirmPanel_NOOP);
                }
            }
            else
            {
                bambudeck_ota_progress_close();
                ui_confirmPanel_show_custom(LV_SYMBOL_WARNING " Download Failed", "OK", NULL, ui_confirmPanel_NOOP);
            }
        });
    }
    else
    {
        static char msg[96];
        snprintf(msg, sizeof(msg), LV_SYMBOL_OK " You are up to date!\n\nv%s (Latest)", BAMBUDECK_FIRMWARE_VERSION);
        ui_confirmPanel_show_custom(msg, "OK", NULL, ui_confirmPanel_NOOP);
    }
}

#endif
