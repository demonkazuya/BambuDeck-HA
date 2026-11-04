#ifndef _BAMBUDECK_CONNECTION
#define _BAMBUDECK_CONNECTION

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <Update.h>
#include "mbedtls/base64.h"
#include "config.h"

static WebServer bambudeck_ota_server(80);
static bool bambudeck_ota_started = false;

static const char BAMBUDECK_OTA_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>BambuDeck-HA OTA Update</title>
<style>
  body { background:#141414; color:#eee; font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,sans-serif; margin:0; padding:16px; }
  .card { max-width:420px; margin:0 auto; background:#242424; padding:20px; border-radius:12px; box-shadow:0 4px 16px rgba(0,0,0,0.6); text-align:center; }
  h2 { color:#00ae42; margin:0 0 8px 0; }
  .sub { font-size:13px; color:#aaa; margin-bottom:18px; }
  input[type=file] { width:100%; box-sizing:border-box; padding:12px; border-radius:6px; border:1px dashed #555; background:#111; color:#fff; margin-bottom:16px; }
  button { width:100%; padding:12px; background:#00ae42; color:#fff; border:none; border-radius:6px; font-size:16px; font-weight:bold; cursor:pointer; }
  button:hover { background:#008f36; }
  .hint { font-size:12px; color:#888; margin-top:12px; }
</style>
</head>
<body>
<div class="card">
  <h2>BambuDeck-HA OTA</h2>
  <div class="sub">Upload <b>bambudeck.bin</b> (application binary) to update over WiFi</div>
  <form method="POST" action="/update" enctype="multipart/form-data">
    <input type="file" name="update" accept=".bin" required>
    <button type="submit">Upload &amp; Flash OTA</button>
  </form>
  <div class="hint">Device will automatically reboot when complete.</div>
</div>
</body>
</html>
)rawliteral";

inline void bambudeck_ota_setup()
{
    if (bambudeck_ota_started || WiFi.status() != WL_CONNECTED)
        return;

    ArduinoOTA.setHostname("bambudeck");
    ArduinoOTA.onStart([]() {
        ConsoleInfo.println("[BambuDeck][OTA] ArduinoOTA update starting...");
    });
    ArduinoOTA.onError([](ota_error_t error) {
        ConsoleError.printf("[BambuDeck][OTA] ArduinoOTA Error[%u]\n", error);
    });
    ArduinoOTA.begin();

    bambudeck_ota_server.on("/", HTTP_GET, []() {
        bambudeck_ota_server.send_P(200, "text/html", BAMBUDECK_OTA_HTML);
    });
    bambudeck_ota_server.on("/ota", HTTP_GET, []() {
        bambudeck_ota_server.send_P(200, "text/html", BAMBUDECK_OTA_HTML);
    });
    bambudeck_ota_server.on("/ip", HTTP_GET, []() {
        String json = String("{\"ip\":\"") + WiFi.localIP().toString() + "\",\"printer\":\"" + String(bambuDeckConfig.printerName) + "\"}";
        bambudeck_ota_server.send(200, "application/json", json);
    });
    bambudeck_ota_server.on(
        "/update", HTTP_POST,
        []() {
            bool ok = !Update.hasError();
            bambudeck_ota_server.sendHeader("Connection", "close");
            bambudeck_ota_server.send(ok ? 200 : 500, "text/html",
                                      ok ? "<html><body style='background:#141414;color:#00ae42;font-family:sans-serif;text-align:center;padding-top:40px;'><h2>OTA Update Success!</h2><p style='color:#eee;'>Rebooting BambuDeck...</p></body></html>"
                                         : "<html><body style='background:#141414;color:#ff5555;font-family:sans-serif;text-align:center;padding-top:40px;'><h2>OTA Update Failed</h2></body></html>");
            delay(500);
            if (ok)
                ESP.restart();
        },
        []() {
            HTTPUpload &upload = bambudeck_ota_server.upload();
            if (upload.status == UPLOAD_FILE_START)
            {
                ConsoleInfo.printf("[BambuDeck][OTA] HTTP Upload Start: %s\n", upload.filename.c_str());
                Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH);
            }
            else if (upload.status == UPLOAD_FILE_WRITE)
            {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END)
            {
                if (Update.end(true))
                {
                    ConsoleInfo.printf("[BambuDeck][OTA] HTTP Upload Success: %u bytes\n", upload.totalSize);
                }
                else
                {
                    Update.printError(Serial);
                }
            }
        });

    bambudeck_ota_server.begin();
    bambudeck_ota_started = true;
    ConsoleInfo.printf("[BambuDeck][OTA] Ready at http://%s/ (or ArduinoOTA port 3232)\n", WiFi.localIP().toString().c_str());
}

inline void bambudeck_ota_loop()
{
    if (bambudeck_ota_started && WiFi.status() == WL_CONNECTED)
    {
        ArduinoOTA.handle();
        bambudeck_ota_server.handleClient();
    }
}

static const char BAMBUDECK_PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>BambuDeck-HA Setup</title>
<style>
  body { background:#141414; color:#eee; font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,sans-serif; margin:0; padding:16px; }
  .card { max-width:420px; margin:0 auto; background:#242424; padding:20px; border-radius:12px; box-shadow:0 4px 16px rgba(0,0,0,0.6); }
  h2 { color:#00ae42; margin:0 0 4px 0; text-align:center; }
  .sub { text-align:center; font-size:12px; color:#999; margin-bottom:16px; }
  label { display:block; margin-top:14px; font-size:13px; font-weight:600; color:#ccc; }
  input, select { width:100%; box-sizing:border-box; padding:10px; margin-top:5px; border-radius:6px; border:1px solid #444; background:#111; color:#fff; font-size:14px; }
  .row { display:flex; gap:8px; align-items:center; margin-top:5px; }
  .row select { margin-top:0; flex:1; }
  .btn-scan { width:auto; margin-top:0; padding:10px 14px; background:#383838; color:#eee; border:1px solid #555; border-radius:6px; font-size:13px; cursor:pointer; white-space:nowrap; }
  .btn-scan:hover { background:#484848; }
  button[type=submit] { width:100%; margin-top:22px; padding:12px; background:#00ae42; color:#fff; border:none; border-radius:6px; font-size:16px; font-weight:bold; cursor:pointer; }
  button[type=submit]:hover { background:#008f36; }
  .hint { font-size:11px; color:#888; margin-top:3px; line-height:1.35; }
  #custom_ssid_wrap { display:none; margin-top:8px; }
</style>
</head>
<body>
<div class="card">
  <h2>BambuDeck-HA</h2>
  <div class="sub">Direct Home Assistant Setup (No MQTT Required)</div>
  <form method="POST" action="/save" onsubmit="prepareSubmit()">
    <label>WiFi Network (2.4 GHz)</label>
    <div class="row">
      <select id="ssid_select" onchange="onSsidChange()">
        <option value="">Scanning networks...</option>
      </select>
      <button type="button" class="btn-scan" onclick="scanWifi()">&#x21bb; Scan</button>
    </div>
    <div id="custom_ssid_wrap">
      <input id="custom_ssid" placeholder="Enter WiFi SSID manually">
    </div>
    <input type="hidden" name="wifi_ssid" id="wifi_ssid">

    <label>WiFi Password</label>
    <input name="wifi_password" type="password" placeholder="Your WiFi Password">

    <label>Home Assistant URL</label>
    <input name="ha_url" required placeholder="http://192.168.1.100:8123">
    <div class="hint">Include http:// and port :8123 (e.g. http://192.168.1.50:8123)</div>

    <label>Home Assistant Long-Lived Access Token</label>
    <input name="ha_token" required placeholder="YOUR_LONG_LIVED_ACCESS_TOKEN">
    <div class="hint">Create in Home Assistant &rarr; Profile &rarr; Security &rarr; Long-Lived Access Tokens</div>

    <label>Printer Entity Prefix (Optional)</label>
    <input name="name" placeholder="Auto-detect (leave blank or e.g. p1s)">
    <div class="hint">Leave blank to auto-discover your Bambu printer from Home Assistant</div>

    <button type="submit">Save &amp; Connect</button>
  </form>
</div>
<script>
function onSsidChange() {
  var sel = document.getElementById('ssid_select');
  var wrap = document.getElementById('custom_ssid_wrap');
  if (sel.value === '__custom__') {
    wrap.style.display = 'block';
    document.getElementById('custom_ssid').focus();
  } else {
    wrap.style.display = 'none';
  }
}
function prepareSubmit() {
  var sel = document.getElementById('ssid_select');
  var out = document.getElementById('wifi_ssid');
  if (sel.value === '__custom__' || sel.value === '') {
    out.value = document.getElementById('custom_ssid').value.trim();
  } else {
    out.value = sel.value;
  }
}
function scanWifi() {
  var sel = document.getElementById('ssid_select');
  sel.innerHTML = '<option value="">Scanning nearby WiFi...</option>';
  fetch('/scan').then(function(r){ return r.json(); }).then(function(list){
    sel.innerHTML = '';
    var seen = {};
    for (var i = 0; i < list.length; i++) {
      var net = list[i];
      if (!net.ssid || seen[net.ssid]) continue;
      seen[net.ssid] = true;
      var opt = document.createElement('option');
      opt.value = net.ssid;
      opt.textContent = net.ssid + ' (' + net.rssi + ' dBm)';
      sel.appendChild(opt);
    }
    var customOpt = document.createElement('option');
    customOpt.value = '__custom__';
    customOpt.textContent = 'Other / Enter Manually...';
    sel.appendChild(customOpt);
    onSsidChange();
  }).catch(function(){
    sel.innerHTML = '<option value="__custom__">Enter WiFi Manually...</option>';
    onSsidChange();
  });
}
window.onload = scanWifi;
</script>
</body>
</html>
)rawliteral";

String bambudeck_scan_networks_json()
{
    int n = WiFi.scanNetworks(false, false);
    DynamicJsonDocument doc(2048);
    JsonArray arr = doc.to<JsonArray>();
    for (int i = 0; i < n; i++)
    {
        String ssid = WiFi.SSID(i);
        if (ssid.length() > 0)
        {
            JsonObject obj = arr.createNestedObject();
            obj["ssid"] = ssid;
            obj["rssi"] = WiFi.RSSI(i);
        }
    }
    WiFi.scanDelete();
    String out;
    serializeJson(arr, out);
    return out;
}

void bambudeck_run_setup_portal()
{
    ConsoleInfo.println(F("[BambuDeck][Setup] Starting WiFi Setup Portal: BambuDeck-Setup"));

    lv_label_set_text(introScreenCaption, LV_SYMBOL_WIFI " Scanning WiFi Networks...");
    lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0x00ae42), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_timer_handler();

    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP_STA);
    String cachedScanJson = bambudeck_scan_networks_json();

    WiFi.softAP("BambuDeck-Setup");

    DNSServer dnsServer;
    WebServer server(80);
    dnsServer.start(53, "*", WiFi.softAPIP());

    bool saved = false;

    server.on("/", HTTP_GET, [&server]() {
        server.send_P(200, "text/html", BAMBUDECK_PORTAL_HTML);
    });

    server.on("/scan", HTTP_GET, [&server, &cachedScanJson]() {
        if (cachedScanJson.length() <= 2)
        {
            cachedScanJson = bambudeck_scan_networks_json();
        }
        server.send(200, "application/json", cachedScanJson);
        // Refresh cache on next manual scan click
        cachedScanJson = "";
    });

    server.onNotFound([&server]() {
        server.sendHeader("Location", "http://192.168.4.1/", true);
        server.send(302, "text/plain", "");
    });

    server.on("/save", HTTP_POST, [&server, &saved]() {
        DynamicJsonDocument doc(2048);
        String ssid = server.arg("wifi_ssid");
        ssid.trim();
        String pwd = server.arg("wifi_password");
        String haUrl = server.arg("ha_url");
        haUrl.trim();
        if (haUrl.length() > 0 && !haUrl.startsWith("http://") && !haUrl.startsWith("https://"))
        {
            haUrl = "http://" + haUrl;
        }
        while (haUrl.endsWith("/"))
        {
            haUrl = haUrl.substring(0, haUrl.length() - 1);
        }
        String haToken = server.arg("ha_token");
        haToken.trim();
        String name = server.arg("name");
        name.trim();

        doc["wifi_ssid"] = ssid;
        doc["wifi_password"] = pwd;
        doc["ssid"] = ssid;
        doc["pwd"] = pwd;
        doc["ha_url"] = haUrl;
        doc["ha_token"] = haToken;
        doc["name"] = name;
        doc["serial"] = name.length() > 0 ? name : "BambuPrinter";
        doc["model"] = "Universal";
        doc["chamber_sensor"] = true;

        bambudeck_filesystem_writeJson(bambudeck_paths_config, doc);

        server.send(200, "text/html", "<html><body style='background:#141414;color:#00ae42;font-family:sans-serif;text-align:center;padding-top:40px;'><h2>Configuration Saved!</h2><p style='color:#eee;'>BambuDeck is connecting to your WiFi &amp; Home Assistant...</p></body></html>");
        delay(500);
        saved = true;
    });

    server.begin();

    lv_label_set_text(introScreenCaption, LV_SYMBOL_WIFI " Connect WiFi: BambuDeck-Setup\nOpen http://192.168.4.1");
    lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0x00ae42), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_timer_handler();

    while (!saved)
    {
        dnsServer.processNextRequest();
        server.handleClient();
        lv_timer_handler();
        delay(10);
    }

    server.stop();
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    delay(300);
}

bool bambudeck_wifi_setup()
{
    DynamicJsonDocument wifiConfig = bambudeck_load_config();
    const char *ssid = wifiConfig.containsKey("ssid") ? wifiConfig["ssid"].as<const char *>() : (wifiConfig.containsKey("wifi_ssid") ? wifiConfig["wifi_ssid"].as<const char *>() : NULL);
    const char *pwd = wifiConfig.containsKey("pwd") ? wifiConfig["pwd"].as<const char *>() : (wifiConfig.containsKey("wifi_password") ? wifiConfig["wifi_password"].as<const char *>() : "");

    // Launch portal if WiFi SSID is missing OR Home Assistant URL/Token is not configured
    bool hasHa = strlen(bambuDeckConfig.haUrl) > 0 && strlen(bambuDeckConfig.haToken) > 0;
    if (wifiConfig.isNull() || ssid == NULL || strlen(ssid) == 0 || !hasHa)
    {
        bambudeck_run_setup_portal();
        wifiConfig = bambudeck_load_config();
        ssid = wifiConfig.containsKey("ssid") ? wifiConfig["ssid"].as<const char *>() : (wifiConfig.containsKey("wifi_ssid") ? wifiConfig["wifi_ssid"].as<const char *>() : NULL);
        pwd = wifiConfig.containsKey("pwd") ? wifiConfig["pwd"].as<const char *>() : (wifiConfig.containsKey("wifi_password") ? wifiConfig["wifi_password"].as<const char *>() : "");
        if (ssid == NULL || strlen(ssid) == 0)
        {
            delay(500);
            return false;
        }
    }

    String ssidB64String = String(ssid);
    String ssidPWDString = String(pwd ? pwd : "");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidB64String.c_str(), ssidPWDString.c_str());
    ConsoleInfo.printf("[BambuDeck][WiFi] Connecting to %s ..\n", ssidB64String.c_str());

    lv_label_set_text_fmt(introScreenCaption, LV_SYMBOL_WIFI " Connecting to %s...", ssidB64String.c_str());
    lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_timer_handler();
    lv_task_handler();

    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        lv_timer_handler();
        lv_task_handler();

        if (millis() - startAttempt > 20000)
        {
            ConsoleError.println(F("[BambuDeck][WiFi] Connection timed out, launching setup portal"));
            lv_label_set_text(introScreenCaption, LV_SYMBOL_WARNING " WiFi Failed - Starting Setup AP");
            lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0xff5555), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_timer_handler();
            delay(1500);
            bambudeck_run_setup_portal();
            return false;
        }
    }

    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    strncpy(bambuDeckConfig.localIP, WiFi.localIP().toString().c_str(), sizeof(bambuDeckConfig.localIP) - 1);

    lv_label_set_text_fmt(introScreenCaption, LV_SYMBOL_WIFI " WiFi Connected (%s)", bambuDeckConfig.localIP);
    lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0x00ae42), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_timer_handler();
    lv_task_handler();
    delay(500);
    ConsoleInfo.print(F("[BambuDeck][WiFi] Connected with IP: "));
    ConsoleInfo.println(WiFi.localIP());

    return true;
}

#endif
