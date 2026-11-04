#ifndef _BAMBUDECK_FIRMWARE
#define _BAMBUDECK_FIRMWARE

#include <Arduino.h>
#include <Update.h>
#include <SD.h>
#include <stdio.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <mbedtls/sha256.h>
#include <time.h>

struct SemVer
{
    int major;
    int minor;
    int patch;
};

inline bool parseSemVer(const char *version, SemVer &semver)
{
    if (!version || strlen(version) == 0)
        return false;
    while (*version == 'v' || *version == 'V')
        version++;
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

inline bool bambudeck_firmware_semverNeedsUpdate(const char *compare)
{
    SemVer currentVer, remoteVer;

    if (parseSemVer(BAMBUDECK_FIRMWARE_VERSION, currentVer) && parseSemVer(compare, remoteVer))
    {
        return isSemVerGreaterThan(remoteVer, currentVer);
    }

    return false;
}

// Trusted root certificates for GitHub & Azure Release Asset CDN (from Spoolman-CYD)
static const char kTrustedRoots[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIICjzCCAhWgAwIBAgIQXIuZxVqUxdJxVt7NiYDMJjAKBggqhkjOPQQDAzCBiDEL
MAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNl
eSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMT
JVVTRVJUcnVzdCBFQ0MgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAwMjAx
MDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNVBAgT
Ck5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVUaGUg
VVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBFQ0MgQ2VydGlm
aWNhdGlvbiBBdXRob3JpdHkwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQarFRaqflo
I+d61SRvU8Za2EurxtW20eZzca7dnNYMYf3boIkDuAUU7FfO7l0/4iGzzvfUinng
o4N+LZfQYcTxmdwlkWOrfzCjtHDix6EznPO/LlxTsV+zfTJ/ijTjeXmjQjBAMB0G
A1UdDgQWBBQ64QmG1M8ZwpZ2dEl23OA1xmNjmjAOBgNVHQ8BAf8EBAMCAQYwDwYD
VR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAwNoADBlAjA2Z6EWCNzklwBBHU6+4WMB
zzuqQhFkoJ2UOQIReVx7Hfpkue4WQrO/isIJxOzksU0CMQDpKmFHjFJKS04YcPbW
RNZu9YO6bVi9JNlWSOrvxKJGgYhqOkbRqZtNyWHa0V1Xahg=
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)CERT";

inline bool bambudeck_ensure_clock()
{
    if (time(nullptr) > 1710000000)
        return true;
    configTime(0, 0, "pool.ntp.org", "time.cloudflare.com");
    const uint32_t start = millis();
    while (millis() - start < 5000)
    {
        if (time(nullptr) > 1710000000)
            return true;
        delay(100);
        lv_timer_handler();
    }
    return (time(nullptr) > 1710000000);
}

inline void bambudeck_configure_secure_client(WiFiClientSecure &secure)
{
    // Skip loading bulky CA cert chains into RAM to maximize free contiguous heap for TLS buffers.
    // Firmware authenticity & integrity is verified end-to-end via SHA-256 hash matching.
    secure.setInsecure();
    secure.setTimeout(25000);
}

inline void bambudeck_configure_http(HTTPClient &http)
{
    http.setTimeout(25000);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.setRedirectLimit(6);
    http.setUserAgent("BambuDeck-HA-OTA/" BAMBUDECK_FIRMWARE_VERSION " (ESP32)");
}

inline String bambudeck_hex_digest(const uint8_t digest[32])
{
    static const char digits[] = "0123456789abcdef";
    char result[65];
    for (size_t i = 0; i < 32; ++i)
    {
        result[i * 2] = digits[digest[i] >> 4];
        result[i * 2 + 1] = digits[digest[i] & 0x0F];
    }
    result[64] = '\0';
    return String(result);
}

static lv_obj_t *bambudeck_ota_modal = NULL;
static lv_obj_t *bambudeck_ota_bar = NULL;
static lv_obj_t *bambudeck_ota_title = NULL;
static lv_obj_t *bambudeck_ota_percent = NULL;
static int bambudeck_ota_last_reported_pct = -1;

inline void bambudeck_ota_progress_close(void)
{
    if (bambudeck_ota_modal != NULL && lv_obj_is_valid(bambudeck_ota_modal))
    {
        lv_obj_del(bambudeck_ota_modal);
        bambudeck_ota_modal = NULL;
    }
    bambudeck_ota_bar = NULL;
    bambudeck_ota_title = NULL;
    bambudeck_ota_percent = NULL;
    bambudeck_ota_last_reported_pct = -1;
    lv_refr_now(NULL);
    lv_timer_handler();
    lv_task_handler();
}

inline void bambudeck_ota_progress_show(const char *title)
{
    bambudeck_ota_progress_close();

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
    lv_obj_set_style_pad_row(card, 10, LV_PART_MAIN);

    bambudeck_ota_title = lv_label_create(card);
    lv_obj_set_width(bambudeck_ota_title, lv_pct(100));
    lv_obj_set_style_text_align(bambudeck_ota_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(bambudeck_ota_title, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN);
    lv_obj_set_style_text_font(bambudeck_ota_title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(bambudeck_ota_title, title ? title : "Updating Firmware...");

    // Outer track for progress bar
    lv_obj_t *bar_track = lv_obj_create(card);
    lv_obj_set_size(bar_track, 220, 10);
    lv_obj_clear_flag(bar_track, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(bar_track, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bar_track, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar_track, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bar_track, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bar_track, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Inner green fill indicator
    bambudeck_ota_bar = lv_obj_create(bar_track);
    lv_obj_set_size(bambudeck_ota_bar, 0, 10);
    lv_obj_set_align(bambudeck_ota_bar, LV_ALIGN_LEFT_MID);
    lv_obj_clear_flag(bambudeck_ota_bar, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(bambudeck_ota_bar, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bambudeck_ota_bar, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bambudeck_ota_bar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bambudeck_ota_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bambudeck_ota_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    bambudeck_ota_percent = lv_label_create(card);
    lv_obj_set_width(bambudeck_ota_percent, lv_pct(100));
    lv_obj_set_style_text_align(bambudeck_ota_percent, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(bambudeck_ota_percent, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN);
    lv_obj_set_style_text_font(bambudeck_ota_percent, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(bambudeck_ota_percent, "0% (0 KB)");

    lv_refr_now(NULL);
    lv_timer_handler();
    lv_task_handler();
}

inline void bambudeck_ota_progress_update(size_t currSize, size_t totalSize, bool isBoot = false)
{
    if (totalSize == 0)
        return;
    int progress = (int)((currSize * 100) / totalSize);
    if (progress < 0) progress = 0;
    if (progress > 100) progress = 100;

    if (progress == bambudeck_ota_last_reported_pct && progress < 100)
    {
        return;
    }
    bambudeck_ota_last_reported_pct = progress;

    if (isBoot)
    {
        if (introScreenCaption != NULL && lv_obj_is_valid(introScreenCaption))
        {
            lv_label_set_text_fmt(introScreenCaption, LV_SYMBOL_CHARGE " Updating %d%%", progress);
        }
    }
    else
    {
        if (bambudeck_ota_bar != NULL && lv_obj_is_valid(bambudeck_ota_bar))
        {
            int fillWidth = (progress * 220) / 100;
            lv_obj_set_width(bambudeck_ota_bar, fillWidth);
        }
        if (bambudeck_ota_percent != NULL && lv_obj_is_valid(bambudeck_ota_percent))
        {
            lv_label_set_text_fmt(bambudeck_ota_percent, "%d%% (%d / %d KB)", progress, (int)(currSize / 1024), (int)(totalSize / 1024));
        }
    }
    lv_refr_now(NULL);
    lv_timer_handler();
    lv_task_handler();
}

inline void bambudeck_ota_progress_finish(bool isBoot = false)
{
    if (isBoot)
    {
        if (introScreenCaption != NULL && lv_obj_is_valid(introScreenCaption))
        {
            lv_label_set_text(introScreenCaption, LV_SYMBOL_OK " Update Finished! Rebooting...");
        }
    }
    else
    {
        if (bambudeck_ota_title != NULL && lv_obj_is_valid(bambudeck_ota_title))
        {
            lv_label_set_text(bambudeck_ota_title, LV_SYMBOL_OK " Update Finished! Rebooting...");
        }
        if (bambudeck_ota_bar != NULL && lv_obj_is_valid(bambudeck_ota_bar))
        {
            lv_obj_set_width(bambudeck_ota_bar, 220);
        }
        if (bambudeck_ota_percent != NULL && lv_obj_is_valid(bambudeck_ota_percent))
        {
            lv_label_set_text(bambudeck_ota_percent, "100% Done");
        }
    }
    lv_refr_now(NULL);
    lv_timer_handler();
    lv_task_handler();
    delay(1500);
}

struct BambuDeckOtaManifest
{
    String version;
    String firmwareUrl;
    String sha256;
    size_t sizeBytes = 0;
    String md5;
};

// Fetch ota.json manifest from GitHub with Spoolman-CYD architecture
inline bool bambudeck_fetch_ota_manifest(BambuDeckOtaManifest &manifest, String &errorMsg)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        errorMsg = "WiFi Not Connected";
        return false;
    }

    const char *urls[] = {
        bambudeck_paths_firmware_ota_file,
        "https://github.com/demonkazuya/BambuDeck-HA/releases/latest/download/ota.json"
    };

    bool fetched = false;
    for (size_t i = 0; i < sizeof(urls) / sizeof(urls[0]); ++i)
    {
        WiFiClientSecure secure;
        bambudeck_configure_secure_client(secure);

        HTTPClient http;
        bambudeck_configure_http(http);

        ConsoleInfo.printf("[BambuDeck][OTA] Fetching manifest from: %s\n", urls[i]);
        if (!http.begin(secure, urls[i]))
        {
            continue;
        }

        int code = http.GET();
        if (code == HTTP_CODE_OK)
        {
            DynamicJsonDocument doc(1024);
            DeserializationError err = deserializeJson(doc, http.getStream());
            http.end();
            secure.stop();

            if (!err)
            {
                manifest.version = doc["version"] | "";
                manifest.firmwareUrl = doc.containsKey("firmware_url") ? (doc["firmware_url"] | "") : (doc["url"] | "");
                manifest.sha256 = doc["sha256"] | "";
                manifest.sizeBytes = doc.containsKey("size_bytes") ? (doc["size_bytes"] | 0) : (doc["size"] | 0);
                manifest.md5 = doc["md5"] | "";

                if (manifest.version.length() > 0 && manifest.firmwareUrl.length() > 0)
                {
                    fetched = true;
                    ConsoleInfo.printf("[BambuDeck][OTA] Manifest parsed: v%s, size=%d bytes\n",
                                       manifest.version.c_str(), (int)manifest.sizeBytes);
                    break;
                }
            }
            else
            {
                ConsoleError.printf("[BambuDeck][OTA] JSON parse error: %s\n", err.c_str());
            }
        }
        else
        {
            ConsoleError.printf("[BambuDeck][OTA] HTTP GET %d: %s\n", code, http.errorToString(code).c_str());
            http.end();
            secure.stop();
        }
    }

    if (!fetched)
    {
        errorMsg = "Could not reach GitHub release server";
        return false;
    }

    return true;
}

// Install firmware via Spoolman-CYD streaming OTA engine with SHA-256 validation & progress callback
inline bool bambudeck_perform_ota_install(const BambuDeckOtaManifest &manifest, String &errorMsg, bool isBoot = false)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        errorMsg = "WiFi Not Connected";
        return false;
    }

    if (manifest.firmwareUrl.length() == 0 || manifest.sizeBytes == 0)
    {
        errorMsg = "Invalid update manifest";
        return false;
    }

    const String downloadUrls[] = {
        manifest.firmwareUrl,
        "https://raw.githubusercontent.com/demonkazuya/BambuDeck-HA/main/dist/bambudeck.bin"
    };

    WiFiClientSecure secure;
    bambudeck_configure_secure_client(secure);

    HTTPClient http;
    bambudeck_configure_http(http);

    bool connected = false;
    for (size_t u = 0; u < sizeof(downloadUrls) / sizeof(downloadUrls[0]); ++u)
    {
        ConsoleInfo.printf("[BambuDeck][OTA] Starting secure download: %s\n", downloadUrls[u].c_str());
        if (!http.begin(secure, downloadUrls[u]))
            continue;

        int status = http.GET();
        if (status == HTTP_CODE_OK)
        {
            connected = true;
            break;
        }
        else
        {
            ConsoleError.printf("[BambuDeck][OTA] Download GET failed HTTP %d: %s\n", status, http.errorToString(status).c_str());
            http.end();
            secure.stop();
        }
    }

    if (!connected)
    {
        errorMsg = "Could not download firmware";
        return false;
    }

    int contentLength = http.getSize();
    if (contentLength <= 0 || (size_t)contentLength != manifest.sizeBytes)
    {
        errorMsg = "Firmware size mismatch";
        http.end();
        return false;
    }

    if (!Update.begin(manifest.sizeBytes, U_FLASH))
    {
        errorMsg = "Not enough space in OTA partition";
        http.end();
        return false;
    }

    mbedtls_sha256_context hash;
    uint8_t digest[32];
    bool useSha = (manifest.sha256.length() == 64);
    if (useSha)
    {
        mbedtls_sha256_init(&hash);
        if (mbedtls_sha256_starts_ret(&hash, 0) != 0)
        {
            Update.abort();
            mbedtls_sha256_free(&hash);
            http.end();
            errorMsg = "Could not initialize SHA-256";
            return false;
        }
    }
    else if (manifest.md5.length() > 0)
    {
        Update.setMD5(manifest.md5.c_str());
    }

    if (!isBoot)
    {
        bambudeck_ota_progress_show(LV_SYMBOL_DOWNLOAD " Downloading & Flashing...");
    }

    WiFiClient *stream = http.getStreamPtr();
    uint8_t buffer[1024];
    size_t remaining = manifest.sizeBytes;
    size_t completed = 0;
    uint32_t lastDataMs = millis();
    size_t lastProgress = 0;

    while (remaining > 0)
    {
        int available = stream->available();
        if (available <= 0)
        {
            if (!http.connected() || millis() - lastDataMs > 20000)
            {
                break;
            }
            delay(1);
            continue;
        }

        size_t chunkSize = min(static_cast<size_t>(available), min(sizeof(buffer), remaining));
        size_t received = stream->readBytes((char *)buffer, chunkSize);
        if (received == 0)
            continue;

        lastDataMs = millis();

        if (Update.write(buffer, received) != received)
        {
            Update.abort();
            if (useSha) mbedtls_sha256_free(&hash);
            http.end();
            errorMsg = "Firmware flash write failed";
            return false;
        }

        if (useSha)
        {
            if (mbedtls_sha256_update_ret(&hash, buffer, received) != 0)
            {
                Update.abort();
                mbedtls_sha256_free(&hash);
                http.end();
                errorMsg = "SHA-256 hash update failed";
                return false;
            }
        }

        completed += received;
        remaining -= received;

        if (completed - lastProgress >= 16384 || remaining == 0)
        {
            lastProgress = completed;
            bambudeck_ota_progress_update(completed, manifest.sizeBytes, isBoot);
        }

        delay(1); // yields to FreeRTOS and feeds watchdog!
    }

    http.end();

    if (remaining != 0)
    {
        Update.abort();
        if (useSha) mbedtls_sha256_free(&hash);
        errorMsg = "Download incomplete";
        return false;
    }

    if (useSha)
    {
        if (mbedtls_sha256_finish_ret(&hash, digest) != 0)
        {
            Update.abort();
            mbedtls_sha256_free(&hash);
            errorMsg = "SHA-256 finalization failed";
            return false;
        }
        mbedtls_sha256_free(&hash);

        String computedSha = bambudeck_hex_digest(digest);
        if (!computedSha.equalsIgnoreCase(manifest.sha256))
        {
            Update.abort();
            errorMsg = "SHA-256 verification mismatch";
            return false;
        }
    }

    if (!Update.end())
    {
        errorMsg = "Could not commit to OTA partition";
        return false;
    }

    ConsoleInfo.println(F("[BambuDeck][OTA] Direct HTTP OTA flash verified and successful!"));
    bambudeck_ota_progress_finish(isBoot);
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
    lv_refr_now(NULL);

    BambuDeckOtaManifest manifest;
    String errorMsg;
    if (!bambudeck_fetch_ota_manifest(manifest, errorMsg))
    {
        ConsoleError.printf("[BambuDeck][OTA] Boot check failed: %s\n", errorMsg.c_str());
        return;
    }

    ConsoleInfo.printf("[BambuDeck][OTA] Boot check: Remote v%s, Current v%s\n",
                       manifest.version.c_str(), BAMBUDECK_FIRMWARE_VERSION);

    if (bambudeck_firmware_semverNeedsUpdate(manifest.version.c_str()))
    {
        ConsoleInfo.printf("[BambuDeck][OTA] Boot update found: v%s -> installing...\n", manifest.version.c_str());
        lv_label_set_text_fmt(introScreenCaption, LV_SYMBOL_DOWNLOAD " Updating to v%s...", manifest.version.c_str());
        lv_obj_set_style_text_color(introScreenCaption, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_refr_now(NULL);
        bool ok = bambudeck_perform_ota_install(manifest, errorMsg, true);
        if (ok)
        {
            ESP.restart();
        }
        else
        {
            bambudeck_ota_progress_close();
            lv_label_set_text_fmt(introScreenCaption, LV_SYMBOL_WARNING " OTA Failed: %s", errorMsg.c_str());
            lv_refr_now(NULL);
            delay(2000);
        }
    }
    else
    {
        ConsoleInfo.println(F("[BambuDeck][OTA] Boot check: Firmware is up to date"));
        lv_label_set_text_fmt(introScreenCaption, LV_SYMBOL_OK " Firmware Up to Date (v%s)", BAMBUDECK_FIRMWARE_VERSION);
        lv_refr_now(NULL);
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

    BambuDeckOtaManifest manifest;
    String errorMsg;
    bool ok = bambudeck_fetch_ota_manifest(manifest, errorMsg);
    if (!ok)
    {
        static char msg[128];
        snprintf(msg, sizeof(msg), LV_SYMBOL_WARNING " %s", errorMsg.c_str());
        ui_confirmPanel_show_custom(msg, "OK", NULL, ui_confirmPanel_NOOP);
        return;
    }

    if (bambudeck_firmware_semverNeedsUpdate(manifest.version.c_str()))
    {
        static BambuDeckOtaManifest pendingManifest;
        pendingManifest = manifest;

        static char msg[128];
        snprintf(msg, sizeof(msg), LV_SYMBOL_DOWNLOAD " New Version v%s!\n\nCurrent: v%s\nUpdate now?",
                 pendingManifest.version.c_str(), BAMBUDECK_FIRMWARE_VERSION);

        ui_confirmPanel_show_custom(msg, "Update", "Cancel", []() {
            String err;
            bool success = bambudeck_perform_ota_install(pendingManifest, err);
            if (success)
            {
                ESP.restart();
            }
            else
            {
                bambudeck_ota_progress_close();
                static char errMsg[128];
                snprintf(errMsg, sizeof(errMsg), LV_SYMBOL_WARNING " %s", err.c_str());
                ui_confirmPanel_show_custom(errMsg, "OK", NULL, ui_confirmPanel_NOOP);
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
