#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "types.h"
#include "debug.h"
#include "../ui/ui.h"
#include "../ui/ui_msgs.h"

extern void bambudeck_send_msg(BAMBUDECK_MESSAGE message, unsigned long long data);
extern void bambudeck_device_set_print_state(String state);
extern void bambudeck_screen_startScreenTimer();
extern void bambudeck_save_selected_printer(const char *prefix);
extern void bambudeck_run_setup_portal();
extern bool bambudeck_has_ds18b20;
extern "C" void bambudeck_thumbnail_update_ui(void);
extern void bambudeck_thumbnail_check_download(void);
extern void bambudeck_thumbnail_clear(void);

struct BambuDiscoveredPrinter
{
    char prefix[32];
    char name[48];
    char state[16];
    char device_id[48];
};

static BambuDiscoveredPrinter bambudeck_printers[8];
static int bambudeck_printer_count = 0;
static int bambudeck_last_http_code = 0;
static char bambudeck_selected_device_id[48] = "";

static volatile bool bambudeck_ha_poll_now = true;
static volatile bool bambudeck_ha_ui_dirty = false;
static volatile bool bambudeck_has_real_subtask = false;
static volatile bool bambudeck_open_selector_req = false;
static TaskHandle_t bambudeck_ha_task_handle = NULL;
static SemaphoreHandle_t bambudeck_ha_mutex = NULL;

static char bambudeck_ha_cover_entity[64] = "";
static char bambudeck_ha_cover_url[192] = "";
static char bambudeck_ha_cover_state[48] = "";
static char bambudeck_ha_pending_state[16] = "IDLE";
static uint32_t bambudeck_ams_colors[4] = {0xFFFFFF, 0xFF0000, 0x00FF00, 0xFFAA00};
static int bambudeck_ams_tray_count = 0;

static char bambudeck_ha_pending_error_msg[256] = "";
static enum BambuDeckDialogButtonStyle bambudeck_ha_pending_error_style = ONLY_CONFIRM;
static volatile bool bambudeck_ha_pending_error_show = false;
static volatile bool bambudeck_ha_pending_error_hide = false;
static char bambudeck_ha_active_error[256] = "";
static unsigned long bambudeck_ha_error_dismissed_time = 0;

extern "C" void bambudeck_ha_on_error_dismissed(void)
{
    bambudeck_ha_error_dismissed_time = millis();
}

// Non-blocking command queue from UI -> HTTP worker
static volatile bool bambudeck_ha_cmd_pending = false;
static char bambudeck_ha_cmd_path[96] = "";
static char bambudeck_ha_cmd_body[768] = "";
static volatile int bambudeck_ha_pending_chamber_temp = -999;

inline void bambudeck_ha_init_mutex()
{
    if (bambudeck_ha_mutex == NULL)
    {
        bambudeck_ha_mutex = xSemaphoreCreateMutex();
    }
}

inline bool bambudeck_use_ha_direct()
{
    return strlen(bambuDeckConfig.haUrl) > 0 && strlen(bambuDeckConfig.haToken) > 0;
}

inline String bambudeck_ha_base_url()
{
    String url = String(bambuDeckConfig.haUrl);
    url.trim();
    if (url.length() > 0 && !url.startsWith("http://") && !url.startsWith("https://"))
    {
        url = "http://" + url;
    }
    while (url.endsWith("/"))
    {
        url = url.substring(0, url.length() - 1);
    }
    if (url.startsWith("http://"))
    {
        String hostPart = url.substring(7);
        int slashIdx = hostPart.indexOf('/');
        if (slashIdx >= 0)
            hostPart = hostPart.substring(0, slashIdx);
        if (hostPart.indexOf(':') < 0)
        {
            url = "http://" + hostPart + ":8123";
        }
    }
    return url;
}

inline uint32_t bambudeck_parse_hex_color(const char *hexStr)
{
    if (!hexStr || strlen(hexStr) == 0)
        return 0x555555;
    const char *p = hexStr;
    if (*p == '#')
        p++;
    char rgb[7] = {0};
    strncpy(rgb, p, 6);
    return (uint32_t)strtoul(rgb, NULL, 16);
}

inline void bambudeck_update_ams_ui_colors()
{
    if (bambuDeckConfig.currentScreenIndex != 0 || ui_homeComponent == NULL)
        return;

    const int colorIndices[4] = {
        UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENAMS_MAINSCREENAMSCOLOR0,
        UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENAMS_MAINSCREENAMSCOLOR1,
        UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENAMS_MAINSCREENAMSCOLOR2,
        UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENAMS_MAINSCREENAMSCOLOR3};

    for (int i = 0; i < 4; i++)
    {
        lv_obj_t *slot = ui_comp_get_child(ui_homeComponent, colorIndices[i]);
        if (slot)
        {
            uint32_t col = (i < bambudeck_ams_tray_count) ? bambudeck_ams_colors[i] : 0x333333;
            lv_obj_set_style_bg_color(slot, lv_color_hex(col), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

inline int bambudeck_ha_post(const String &path, const String &jsonPayload, String *responseOut = NULL, int timeoutMs = 3000)
{
    if (WiFi.status() != WL_CONNECTED || strlen(bambuDeckConfig.haUrl) == 0 || strlen(bambuDeckConfig.haToken) == 0)
        return -1;

    bambudeck_ha_init_mutex();
    if (xSemaphoreTake(bambudeck_ha_mutex, pdMS_TO_TICKS(5000)) != pdTRUE)
        return -2;

    String fullUrl = bambudeck_ha_base_url() + path;
    HTTPClient http;
    WiFiClient client;
    http.setTimeout(timeoutMs);

    if (!http.begin(client, fullUrl))
    {
        xSemaphoreGive(bambudeck_ha_mutex);
        return -1;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + String(bambuDeckConfig.haToken));

    int code = http.POST(jsonPayload);
    bambudeck_last_http_code = code;
    if (code >= 200 && code < 300 && responseOut != NULL)
    {
        *responseOut = http.getString();
    }
    else if (code < 200 || code >= 300)
    {
        ConsoleError.printf("[BambuDeck][HA] POST %s failed with HTTP %d\n", path.c_str(), code);
    }
    http.end();
    xSemaphoreGive(bambudeck_ha_mutex);
    return code;
}

inline void bambudeck_ha_call_service(const char *domain, const char *service, JsonDocument &serviceData)
{
    String body;
    serializeJson(serviceData, body);
    String path = String("/api/services/") + domain + "/" + service;
    strncpy(bambudeck_ha_cmd_path, path.c_str(), sizeof(bambudeck_ha_cmd_path) - 1);
    strncpy(bambudeck_ha_cmd_body, body.c_str(), sizeof(bambudeck_ha_cmd_body) - 1);
    bambudeck_ha_cmd_pending = true;
    bambudeck_ha_poll_now = true;
}

inline void bambudeck_ha_press_button(const char *suffix)
{
    if (strlen(bambuDeckConfig.printerName) == 0)
        return;
    DynamicJsonDocument doc(256);
    doc["entity_id"] = String("button.") + String(bambuDeckConfig.printerName) + "_" + suffix;
    bambudeck_ha_call_service("button", "press", doc);
}

inline void bambudeck_ha_send_gcode(const String &gcode)
{
    DynamicJsonDocument doc(512);
    if (strlen(bambudeck_selected_device_id) > 0)
    {
        doc["device_id"] = (const char *)bambudeck_selected_device_id;
    }
    else if (strlen(bambuDeckConfig.printerName) > 0)
    {
        doc["entity_id"] = String("sensor.") + String(bambuDeckConfig.printerName) + "_print_status";
    }
    doc["command"] = gcode;
    bambudeck_ha_call_service("bambu_lab", "send_command", doc);
}

inline void bambudeck_ha_move_axis(const String &axis, int distance)
{
    if (strlen(bambudeck_selected_device_id) > 0)
    {
        DynamicJsonDocument doc(256);
        doc["device_id"] = (const char *)bambudeck_selected_device_id;
        doc["axis"] = axis;
        doc["distance"] = distance;
        bambudeck_ha_call_service("bambu_lab", "move_axis", doc);
    }
    else
    {
        char cmd[256];
        int speed = (axis == "Z") ? 1500 : 3000;
        sprintf(cmd, "M211 S \nM211 X1 Y1 Z1\nM1002 push_ref_mode\nG91 \nG1 %s%d.0 F%d\nM1002 pop_ref_mode\nM211 R\n", axis.c_str(), distance, speed);
        bambudeck_ha_send_gcode(String(cmd));
    }
}

inline void bambudeck_ha_publish_chamber_temp(int temp)
{
    if (temp > -50 && temp < 125)
    {
        bambudeck_ha_pending_chamber_temp = temp;
    }
}

// Discovers all Bambu Lab printers registered in Home Assistant
inline int bambudeck_ha_discover_printers()
{
    if (WiFi.status() != WL_CONNECTED || strlen(bambuDeckConfig.haUrl) == 0 || strlen(bambuDeckConfig.haToken) == 0)
        return 0;

    String tpl = "{% set ns = namespace(res=[], seen=[]) %}"
                 "{% for s in states.sensor if s.entity_id.endswith('_nozzle_temperature') and not s.entity_id.endswith('_target_nozzle_temperature') %}"
                 "{% set p = s.entity_id[7:-19] %}"
                 "{% if p not in ns.seen %}"
                 "{% set ns.seen = ns.seen + [p] %}"
                 "{% set dn = device_attr(s.entity_id, 'name_by_user') or device_attr(s.entity_id, 'name') or p %}"
                 "{% set dv = device_id(s.entity_id) or '' %}"
                 "{% set st = states('sensor.' ~ p ~ '_print_status') %}"
                 "{% if st in ['unknown','unavailable',''] %}{% set st = states('sensor.' ~ p ~ '_gcode_state') %}{% endif %}"
                 "{% set ns.res = ns.res + [{'p': p, 'n': dn, 's': st, 'd': dv}] %}"
                 "{% endif %}"
                 "{% endfor %}"
                 "{% for s in states.sensor if s.entity_id.endswith('_print_status') %}"
                 "{% set p = s.entity_id[7:-13] %}"
                 "{% if p not in ns.seen %}"
                 "{% set ns.seen = ns.seen + [p] %}"
                 "{% set dn = device_attr(s.entity_id, 'name_by_user') or device_attr(s.entity_id, 'name') or p %}"
                 "{% set dv = device_id(s.entity_id) or '' %}"
                 "{% set ns.res = ns.res + [{'p': p, 'n': dn, 's': s.state, 'd': dv}] %}"
                 "{% endif %}"
                 "{% endfor %}"
                 "{{ ns.res | tojson }}";

    StaticJsonDocument<128> reqDoc;
    reqDoc["template"] = tpl.c_str();
    String reqBody;
    serializeJson(reqDoc, reqBody);

    String respBody;
    int code = bambudeck_ha_post("/api/template", reqBody, &respBody, 3500);
    if (code < 200 || code >= 300)
    {
        return 0;
    }

    DynamicJsonDocument doc(2048);
    if (deserializeJson(doc, respBody) || !doc.is<JsonArray>())
    {
        return 0;
    }

    JsonArray arr = doc.as<JsonArray>();
    bambudeck_printer_count = 0;
    for (JsonObject obj : arr)
    {
        if (bambudeck_printer_count >= 8)
            break;
        const char *p = obj["p"] | "";
        const char *n = obj["n"] | p;
        const char *s = obj["s"] | "idle";
        const char *d = obj["d"] | "";
        if (strlen(p) > 0)
        {
            memset(&bambudeck_printers[bambudeck_printer_count], 0, sizeof(BambuDiscoveredPrinter));
            strncpy(bambudeck_printers[bambudeck_printer_count].prefix, p, sizeof(bambudeck_printers[0].prefix) - 1);
            strncpy(bambudeck_printers[bambudeck_printer_count].name, n, sizeof(bambudeck_printers[0].name) - 1);
            strncpy(bambudeck_printers[bambudeck_printer_count].state, s, sizeof(bambudeck_printers[0].state) - 1);
            strncpy(bambudeck_printers[bambudeck_printer_count].device_id, d, sizeof(bambudeck_printers[0].device_id) - 1);
            if (strcasecmp(bambuDeckConfig.printerName, p) == 0 && strlen(d) > 0)
            {
                strncpy(bambudeck_selected_device_id, d, sizeof(bambudeck_selected_device_id) - 1);
            }
            bambudeck_printer_count++;
        }
    }

    ConsoleInfo.printf("[BambuDeck][HA] Discovered %d Bambu printer(s) in Home Assistant\n", bambudeck_printer_count);
    return bambudeck_printer_count;
}

// Polls ONLY the currently selected printer (bambuDeckConfig.printerName)
inline bool bambudeck_ha_poll_status_worker()
{
    if (WiFi.status() != WL_CONNECTED || strlen(bambuDeckConfig.haUrl) == 0 || strlen(bambuDeckConfig.haToken) == 0)
        return false;

    String p = String(bambuDeckConfig.printerName);
    p.trim();
    p.toLowerCase();
    p.replace(" ", "_");
    p.replace("-", "_");

    if (p.length() == 0)
    {
        if (bambudeck_printer_count == 0)
        {
            bambudeck_ha_discover_printers();
        }
        if (bambudeck_printer_count > 0)
        {
            p = String(bambudeck_printers[0].prefix);
            strncpy(bambuDeckConfig.printerName, p.c_str(), sizeof(bambuDeckConfig.printerName) - 1);
            strncpy(bambudeck_selected_device_id, bambudeck_printers[0].device_id, sizeof(bambudeck_selected_device_id) - 1);
        }
        else
        {
            bambudeck_has_real_subtask = false;
            bambuStatus.subtask_name[0] = '\0';
            bambudeck_ha_ui_dirty = true;
            return false;
        }
    }

    // Compact, fast O(1) Jinja2 template for the selected printer prefix `p`
    String tpl = "{% set p = '" + p + "' %}"
                 "{% set d = device_id('sensor.'~p~'_nozzle_temperature') or device_id('sensor.'~p~'_print_status') or device_id('light.'~p~'_chamber_light') or '' %}"
                 "{% set gc = states('sensor.'~p~'_gcode_state') %}"
                 "{% set st = states('sensor.'~p~'_print_status') %}"
                 "{% if st in ['unknown','unavailable','None',''] %}{% set st = gc %}{% endif %}"
                 "{% set sub = states('sensor.'~p~'_subtask_name') %}"
                 "{% if sub in ['unknown','unavailable','None',''] %}{% set sub = states('sensor.'~p~'_task_name') %}{% endif %}"
                 "{% set spd = states('select.'~p~'_printing_speed') %}"
                 "{% if spd in ['unknown','unavailable','None',''] %}{% set spd = states('sensor.'~p~'_speed_profile') %}{% endif %}"
                 "{% set pct = states('sensor.'~p~'_print_progress') %}"
                 "{% if pct in ['unknown','unavailable','None',''] %}{% set pct = states('sensor.'~p~'_print_percentage') %}{% endif %}"
                 "{% if pct in ['unknown','unavailable','None',''] %}{% set pct = states('sensor.'~p~'_progress') %}{% endif %}"
                 "{% if pct in ['unknown','unavailable','None',''] %}{% set pct = states('sensor.'~p~'_percentage') %}{% endif %}"
                 "{% if pct in ['unknown','unavailable','None',''] %}{% set pct = states('sensor.'~p~'_print_percent') %}{% endif %}"
                 "{% if pct in ['unknown','unavailable','None',''] %}{% set pct = states('sensor.'~p~'_mc_print_percent') %}{% endif %}"
                 "{% if pct in ['unknown','unavailable','None',''] %}{% set pct = states('sensor.'~p~'_mc_percent') %}{% endif %}"
                 "{% if pct in ['unknown','unavailable','None',''] %}{% set pct = state_attr('sensor.'~p~'_print_status', 'print_progress') or state_attr('sensor.'~p~'_print_status', 'print_percentage') or state_attr('sensor.'~p~'_print_status', 'progress') or state_attr('sensor.'~p~'_print_status', 'percentage') or state_attr('sensor.'~p~'_print_status', 'mc_percent') or state_attr('sensor.'~p~'_print_job', 'print_percentage') or state_attr('sensor.'~p~'_print_job', 'progress') or '' %}{% endif %}"
                 "{% set pct_val = (pct | string | replace('%', '') | trim | float(0)) | round(0) | int %}"
                 "{% set cur_l = states('sensor.'~p~'_current_layer') | int(0) %}"
                 "{% if cur_l == 0 %}{% set cur_l = state_attr('sensor.'~p~'_print_status', 'current_layer') | int(0) %}{% endif %}"
                 "{% set tot_l = states('sensor.'~p~'_total_layer_count') | int(0) %}"
                 "{% if tot_l == 0 %}{% set tot_l = states('sensor.'~p~'_total_layers') | int(0) %}{% endif %}"
                 "{% if tot_l == 0 %}{% set tot_l = state_attr('sensor.'~p~'_print_status', 'total_layer_count') | int(0) %}{% endif %}"
                 "{% if tot_l == 0 %}{% set tot_l = state_attr('sensor.'~p~'_print_status', 'total_layers') | int(0) %}{% endif %}"
                 "{% if pct_val == 0 and tot_l > 0 and cur_l > 0 %}{% set pct_val = ((cur_l * 100) / tot_l) | round(0) | int %}{% endif %}"
                 "{% if pct_val < 0 %}{% set pct_val = 0 %}{% elif pct_val > 100 %}{% set pct_val = 100 %}{% endif %}"
                 "{% set nt = states('number.'~p~'_nozzle_target_temperature') | float(0) %}"
                 "{% if nt == 0 %}{% set nt = states('sensor.'~p~'_nozzle_target_temperature') | float(0) %}{% endif %}"
                 "{% if nt == 0 %}{% set nt = states('sensor.'~p~'_target_nozzle_temperature') | float(0) %}{% endif %}"
                 "{% set bt = states('number.'~p~'_bed_target_temperature') | float(0) %}"
                 "{% if bt == 0 %}{% set bt = states('sensor.'~p~'_bed_target_temperature') | float(0) %}{% endif %}"
                 "{% if bt == 0 %}{% set bt = states('sensor.'~p~'_target_bed_temperature') | float(0) %}{% endif %}"
                 "{% set t1 = state_attr('sensor.'~p~'_ams_1_tray_1', 'color') or state_attr('sensor.'~p~'_tray_1', 'color') or '' %}"
                 "{% set t2 = state_attr('sensor.'~p~'_ams_1_tray_2', 'color') or state_attr('sensor.'~p~'_tray_2', 'color') or '' %}"
                 "{% set t3 = state_attr('sensor.'~p~'_ams_1_tray_3', 'color') or state_attr('sensor.'~p~'_tray_3', 'color') or '' %}"
                 "{% set t4 = state_attr('sensor.'~p~'_ams_1_tray_4', 'color') or state_attr('sensor.'~p~'_tray_4', 'color') or '' %}"
                 "{% set rs = 'sensor.'~p~'_remaining_time' %}"
                 "{% set rv = states(rs) | float(-1) %}"
                 "{% set ru = (state_attr(rs, 'unit_of_measurement') or 'min') | lower %}"
                 "{% if rv >= 0 %}{% set rem = rv * 60 if ru in ['h','hr','hours'] else (rv / 60 if ru in ['s','sec'] else (rv * 1440 if ru in ['d','days'] else rv)) %}"
                 "{% else %}{% set et = as_timestamp(states('sensor.'~p~'_end_time'), 0) %}{% set rem = ((et - as_timestamp(now())) / 60) if et > 0 else 0 %}{% endif %}"
                 "{% if rem < 0 %}{% set rem = 0 %}{% endif %}"
                 "{% set cov_s = states('image.'~p~'_cover_image') %}"
                 "{% set cov_u = state_attr('image.'~p~'_cover_image', 'entity_picture') or '' %}"
                 "{% set ct = states('sensor.'~p~'_chamber_temperature') | float(0) %}"
                 "{% if ct == 0 %}{% set ct = states('sensor.'~p~'_chamber_temp') | float(0) %}{% endif %}"
                 "{% if ct == 0 %}{% set ct = states('sensor.'~p~'_enclosure_temperature') | float(0) %}{% endif %}"
                 "{% if ct == 0 %}{% set ct = states('sensor.bambudeck_chamber_temperature') | float(0) %}{% endif %}"
                 "{% set l_noz = states('sensor.'~p~'_left_nozzle_temperature') | float(states('sensor.'~p~'_left_nozzle_temp') | float(0)) %}"
                 "{% set r_noz = states('sensor.'~p~'_right_nozzle_temperature') | float(states('sensor.'~p~'_right_nozzle_temp') | float(0)) %}"
                 "{% set l_nt = states('number.'~p~'_left_nozzle_target_temperature') | float(states('sensor.'~p~'_left_target_nozzle_temperature') | float(0)) %}"
                 "{% set r_nt = states('number.'~p~'_right_nozzle_target_temperature') | float(states('sensor.'~p~'_right_target_nozzle_temperature') | float(0)) %}"
                 "{% set noz2 = r_noz if (l_noz > 0 and r_noz > 0) else states('sensor.'~p~'_nozzle_temperature_2') | float(states('sensor.'~p~'_nozzle_2_temperature') | float(0)) %}"
                 "{% set noz2_t = r_nt if (l_noz > 0 and r_noz > 0) else states('number.'~p~'_nozzle_target_temperature_2') | float(states('sensor.'~p~'_nozzle_target_temperature_2') | float(0)) %}"
                 "{% set noz_typ = states('sensor.'~p~'_nozzle_type') %}"
                 "{% set noz_dia = states('sensor.'~p~'_nozzle_diameter') %}"
                 "{% set act_noz = states('sensor.'~p~'_active_nozzle') or states('sensor.'~p~'_active_extruder') or '0' %}"
                 "{% set pe = states('sensor.'~p~'_print_error') %}"
                 "{% set ped = state_attr('sensor.'~p~'_print_error', 'description') or state_attr('sensor.'~p~'_print_error', 'message') or '' %}"
                 "{% set pec = state_attr('sensor.'~p~'_print_error', 'code') or '' %}"
                 "{% set hms = states('sensor.'~p~'_hms') | int(0) %}"
                 "{% set hmsd = state_attr('sensor.'~p~'_hms', 'description') or state_attr('sensor.'~p~'_hms', 'errors') or '' %}"
                 "{% set std = state_attr('sensor.'~p~'_print_status', 'message') or state_attr('sensor.'~p~'_print_status', 'error') or '' %}"
                 "{{ {"
                 "'dev': d,"
                 "'state': st | lower,"
                 "'stage': states('sensor.'~p~'_current_stage') | lower,"
                 "'gcode': gc | lower if gc not in ['unknown','unavailable','None'] else '',"
                 "'pct': pct_val,"
                 "'rem': rem | round(0) | int,"
                 "'layer': cur_l,"
                 "'layers': tot_l,"
                 "'subtask': sub,"
                 "'noz': states('sensor.'~p~'_nozzle_temperature') | float(0),"
                 "'noz_t': nt,"
                 "'noz2': noz2,"
                 "'noz2_t': noz2_t,"
                 "'noz_typ': noz_typ if noz_typ not in ['unknown','unavailable','None'] else '',"
                 "'noz_dia': noz_dia if noz_dia not in ['unknown','unavailable','None'] else '',"
                 "'act_noz': act_noz | int(0),"
                 "'bed': states('sensor.'~p~'_bed_temperature') | float(0),"
                 "'bed_t': bt,"
                 "'cham': ct,"
                 "'spd': spd | lower,"
                 "'fan1': states('sensor.'~p~'_cooling_fan_speed') | int(0),"
                 "'fan2': states('sensor.'~p~'_aux_fan_speed') | int(states('sensor.'~p~'_big_fan_1_speed') | int(0)),"
                 "'fan3': states('sensor.'~p~'_chamber_fan_speed') | int(states('sensor.'~p~'_big_fan_2_speed') | int(0)),"
                 "'light': 1 if states('light.'~p~'_chamber_light') == 'on' else 0,"
                 "'cov_s': cov_s,"
                 "'cov_u': cov_u,"
                 "'trays': [t1, t2, t3, t4],"
                 "'pe': pe if pe not in ['unknown','unavailable','None','ok','OK','none',''] else '0',"
                 "'ped': ped | string,"
                 "'pec': pec | string,"
                 "'hms': hms,"
                 "'hmsd': hmsd | string,"
                 "'std': std | string"
                 "} | tojson }}";

    // Pass tpl.c_str() (const char*) so ArduinoJson stores a zero-copy pointer and never overflows reqDoc!
    StaticJsonDocument<128> reqDoc;
    reqDoc["template"] = tpl.c_str();
    String reqBody;
    serializeJson(reqDoc, reqBody);

    String respBody;
    int httpCode = bambudeck_ha_post("/api/template", reqBody, &respBody, 3000);
    if (httpCode < 200 || httpCode >= 300)
    {
        bambudeck_has_real_subtask = false;
        snprintf(bambuStatus.subtask_name, sizeof(bambuStatus.subtask_name), "HA Err %d (%s)", httpCode, p.c_str());
        bambudeck_ha_ui_dirty = true;
        return false;
    }

    DynamicJsonDocument doc(2048);
    DeserializationError err = deserializeJson(doc, respBody);
    if (err)
    {
        bambudeck_has_real_subtask = false;
        snprintf(bambuStatus.subtask_name, sizeof(bambuStatus.subtask_name), "HA JSON Err (%s)", p.c_str());
        bambudeck_ha_ui_dirty = true;
        return false;
    }

    const char *devId = doc["dev"] | "";
    if (strlen(devId) > 0)
    {
        strncpy(bambudeck_selected_device_id, devId, sizeof(bambudeck_selected_device_id) - 1);
    }

    String st = doc["state"] | "idle";
    String stage = doc["stage"] | "";
    String gcode = doc["gcode"] | "";

    st.toLowerCase();
    stage.toLowerCase();
    gcode.toLowerCase();

    const char *mappedState = "IDLE";
    if (gcode == "running" || st == "running" || st == "printing" || stage == "printing")
    {
        mappedState = "RUNNING";
    }
    else if (gcode == "pause" || gcode == "paused" || st == "pause" || st == "paused" || stage.indexOf("pause") >= 0)
    {
        mappedState = "PAUSE";
    }
    else if (gcode == "finish" || gcode == "finished" || st == "finish" || st == "finished" || st == "complete" || st == "completed")
    {
        mappedState = "FINISH";
    }
    else if (gcode == "failed" || st == "failed" || st == "error")
    {
        mappedState = "FAILED";
    }
    else if (gcode == "prepare" || gcode == "preparing" ||
             st == "prepare" || st == "preparing" || st == "slicing" || st == "init" ||
             st == "heating" || st == "heatbed_preheating" || st == "bed_heating" || st == "nozzle_heating" ||
             st == "auto_bed_leveling" || st == "bed_leveling" || st == "calibrating" ||
             st == "cleaning_nozzle" || st == "cleaning_nozzle_tip" || st == "homing" || st == "homing_toolhead" ||
             st == "filament_loading" ||
             stage == "prepare" || stage == "preparing" || stage == "slicing" ||
             stage == "heating" || stage == "heatbed_preheating" || stage == "auto_bed_leveling" ||
             stage == "bed_leveling" || stage == "cleaning_nozzle_tip" ||
             stage == "calibrating_micro_lidar" || stage == "calibrating_extrusion_flow" ||
             stage == "calibrating_motor_noise" || stage == "homing" || stage == "homing_toolhead" ||
             stage == "filament_loading" || stage == "cooling_chamber" || stage == "chamber_heating")
    {
        mappedState = "PREPARE";
    }
    else if (bambudeck_has_real_subtask && (bambuStatus.current_layer > 0 || (doc["rem"] | 0) > 0))
    {
        mappedState = "RUNNING";
    }

    strncpy(bambudeck_ha_pending_state, mappedState, sizeof(bambudeck_ha_pending_state) - 1);

    bambuStatus.current_layer = doc["layer"] | 0;
    bambuStatus.total_layers = doc["layers"] | 0;

    int parsedPct = 0;
    if (doc["pct"].is<float>())
    {
        parsedPct = (int)round(doc["pct"].as<float>());
    }
    else if (doc["pct"].is<int>())
    {
        parsedPct = doc["pct"].as<int>();
    }
    else if (doc["pct"].is<const char *>())
    {
        parsedPct = atoi(doc["pct"].as<const char *>());
    }

    if (parsedPct == 0 && bambuStatus.total_layers > 0 && bambuStatus.current_layer > 0)
    {
        parsedPct = (bambuStatus.current_layer * 100) / bambuStatus.total_layers;
    }
    if (parsedPct < 0) parsedPct = 0;
    if (parsedPct > 100) parsedPct = 100;
    bambuStatus.mc_print_percent = parsedPct;
    bambuStatus.mc_left_time = (doc["rem"] | 0) * 60;

    const char *subtask = doc["subtask"] | "";
    if (strlen(subtask) > 0 && strcmp(subtask, "unknown") != 0 && strcmp(subtask, "unavailable") != 0 && strcmp(subtask, "None") != 0)
    {
        bambudeck_has_real_subtask = true;
        strncpy(bambuStatus.subtask_name, subtask, sizeof(bambuStatus.subtask_name) - 1);
    }
    else
    {
        bambudeck_has_real_subtask = false;
        bambuStatus.subtask_name[0] = '\0';
    }

    const char *covState = doc["cov_s"] | "";
    const char *covUrl = doc["cov_u"] | "";
    snprintf(bambudeck_ha_cover_entity, sizeof(bambudeck_ha_cover_entity), "image.%s_cover_image", p.c_str());
    if (strlen(covState) > 0 && strcmp(covState, "unknown") != 0 && strcmp(covState, "unavailable") != 0 && strcmp(covState, "None") != 0)
    {
        strncpy(bambudeck_ha_cover_state, covState, sizeof(bambudeck_ha_cover_state) - 1);
        strncpy(bambudeck_ha_cover_url, covUrl, sizeof(bambudeck_ha_cover_url) - 1);
    }
    else
    {
        bambudeck_ha_cover_state[0] = '\0';
        bambudeck_ha_cover_url[0] = '\0';
    }

    bambuStatus.nozzle_temper = doc["noz"] | 0.0;
    bambuStatus.nozzle_target_temper = doc["noz_t"] | 0.0;
    bambuStatus.nozzle_temper_2 = doc["noz2"] | 0.0;
    bambuStatus.nozzle_target_temper_2 = doc["noz2_t"] | 0.0;
    bambuStatus.active_nozzle = doc["act_noz"] | 0;
    const char *nTyp = doc["noz_typ"] | "";
    if (nTyp != NULL && strlen(nTyp) > 0)
    {
        strncpy(bambuStatus.nozzle_type, nTyp, sizeof(bambuStatus.nozzle_type) - 1);
    }
    bambuStatus.nozzle_diameter = doc["noz_dia"] | 0.4f;
    bambuStatus.bed_temper = doc["bed"] | 0.0;
    bambuStatus.bed_target_temper = doc["bed_t"] | 0.0;

    if (!bambudeck_has_ds18b20)
    {
        bambuStatus.chamber_temper = doc["cham"] | 0.0;
    }

    int fan1Pct = doc["fan1"] | 0;
    int fan2Pct = doc["fan2"] | 0;
    int fan3Pct = doc["fan3"] | 0;
    bambuStatus.cooling_fan_speed = round(fan1Pct * 255.0f / 100.0f);
    bambuStatus.big_fan1_speed = round(fan2Pct * 255.0f / 100.0f);
    bambuStatus.big_fan2_speed = round(fan3Pct * 255.0f / 100.0f);

    bambuStatus.chamberLed = (doc["light"] | 0) == 1;

    String spd = doc["spd"] | "standard";
    if (spd == "silent" || spd == "quiet")
    {
        bambuStatus.printing_speed_lvl = 1;
        bambuStatus.printing_speed_mag = 50;
    }
    else if (spd == "sport")
    {
        bambuStatus.printing_speed_lvl = 3;
        bambuStatus.printing_speed_mag = 124;
    }
    else if (spd == "ludicrous")
    {
        bambuStatus.printing_speed_lvl = 4;
        bambuStatus.printing_speed_mag = 166;
    }
    else
    {
        bambuStatus.printing_speed_lvl = 2;
        bambuStatus.printing_speed_mag = 100;
    }

    bambuStatus.wifi_signal = abs(WiFi.RSSI());
    strncpy(bambuDeckConfig.localIP, WiFi.localIP().toString().c_str(), sizeof(bambuDeckConfig.localIP) - 1);

    if (doc.containsKey("trays") && doc["trays"].is<JsonArray>())
    {
        JsonArray trays = doc["trays"].as<JsonArray>();
        int validCount = 0;
        for (int i = 0; i < 4 && i < (int)trays.size(); i++)
        {
            const char *cStr = trays[i] | "";
            if (strlen(cStr) > 0)
            {
                bambudeck_ams_colors[i] = bambudeck_parse_hex_color(cStr);
                validCount = i + 1;
            }
            else
            {
                bambudeck_ams_colors[i] = 0x333333;
            }
        }
        bambudeck_ams_tray_count = validCount;
        bambuStatus.ams = validCount > 0;
    }

    const char *peRaw = doc["pe"] | "0";
    const char *peDesc = doc["ped"] | "";
    const char *peCode = doc["pec"] | "";
    int hmsCnt = doc["hms"] | 0;
    const char *hmsDesc = doc["hmsd"] | "";
    const char *stDesc = doc["std"] | "";

    uint32_t errNum = 0;
    if (peRaw != NULL && peRaw[0] != '\0' && strcmp(peRaw, "0") != 0)
    {
        char *endptr = NULL;
        errNum = (uint32_t)strtoul(peRaw, &endptr, 10);
        if (errNum == 0 && endptr == peRaw)
        {
            errNum = (uint32_t)strtoul(peRaw, &endptr, 16);
        }
    }
    if (errNum == 0 && peCode != NULL && peCode[0] != '\0')
    {
        String cleanCode = String(peCode);
        cleanCode.replace(" ", "");
        errNum = (uint32_t)strtoul(cleanCode.c_str(), NULL, 16);
    }

    bool hasError = false;
    char errorText[256] = "";
    enum BambuDeckDialogButtonStyle btnStyle = ONLY_CONFIRM;

    if (errNum > 0 || (peDesc && strlen(peDesc) > 0) || hmsCnt > 0 || (st == "failed" && stDesc && strlen(stDesc) > 0))
    {
        hasError = true;
        bambuStatus.print_error = errNum;

        char hexStr[9];
        snprintf(hexStr, sizeof(hexStr), "%08X", (unsigned int)errNum);

        bool hasRetry = (errNum > 0) ? bambudeck_errors_deviceErrorHasRetry(hexStr) : false;
        bool hasDone = (errNum > 0) ? bambudeck_errors_deviceErrorHasDone(hexStr) : false;

        const char *dictMsg = (errNum > 0) ? bambudeck_errors_getDeviceError(hexStr) : NULL;
        if (dictMsg != NULL && strlen(dictMsg) > 0)
        {
            strncpy(errorText, dictMsg, sizeof(errorText) - 1);
        }
        else if (peDesc && strlen(peDesc) > 0)
        {
            strncpy(errorText, peDesc, sizeof(errorText) - 1);
        }
        else if (hmsDesc && strlen(hmsDesc) > 0)
        {
            strncpy(errorText, hmsDesc, sizeof(errorText) - 1);
        }
        else if (stDesc && strlen(stDesc) > 0)
        {
            strncpy(errorText, stDesc, sizeof(errorText) - 1);
        }
        else if (errNum > 0)
        {
            snprintf(errorText, sizeof(errorText), "Printer Error:\n0x%08X", (unsigned int)errNum);
        }
        else
        {
            strncpy(errorText, "Printer Error: Print failed or paused.", sizeof(errorText) - 1);
        }

        if (strcmp(mappedState, "PAUSE") == 0 || strcmp(mappedState, "FAILED") == 0)
        {
            hasRetry = true;
            hasDone = true;
        }

        btnStyle = (hasRetry && hasDone) ? DONE_AND_RETRY : (hasRetry ? CONFIRM_AND_RETRY : (hasDone ? CONFIRM_AND_DONE : ONLY_CONFIRM));
    }

    if (hasError)
    {
        // Don't re-show immediately if user just dismissed it in the last 5 seconds
        if (millis() - bambudeck_ha_error_dismissed_time > 5000)
        {
            if (strcmp(bambudeck_ha_active_error, errorText) != 0)
            {
                strncpy(bambudeck_ha_active_error, errorText, sizeof(bambudeck_ha_active_error) - 1);
                strncpy(bambudeck_ha_pending_error_msg, errorText, sizeof(bambudeck_ha_pending_error_msg) - 1);
                bambudeck_ha_pending_error_style = btnStyle;
                bambudeck_ha_pending_error_show = true;
                bambudeck_ha_pending_error_hide = false;
            }
        }
    }
    else
    {
        if (bambudeck_ha_active_error[0] != '\0')
        {
            bambudeck_ha_active_error[0] = '\0';
            bambuStatus.print_error = 0;
            bambudeck_ha_pending_error_hide = true;
            bambudeck_ha_pending_error_show = false;
        }
    }

    bambudeck_ha_ui_dirty = true;
    return true;
}

// ---------------- LCD PRINTER SELECTOR MODAL ----------------
static lv_obj_t *bambudeck_selector_modal = NULL;
static lv_obj_t *bambudeck_selector_list = NULL;
static volatile bool bambudeck_selector_done = false;
static volatile bool bambudeck_selector_rescan_req = false;
static volatile bool bambudeck_selector_portal_req = false;

static void bambudeck_on_printer_item_clicked(lv_event_t *e)
{
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    if (idx >= 0 && idx < bambudeck_printer_count)
    {
        ConsoleInfo.printf("[BambuDeck][HA] User selected printer on LCD: %s (%s)\n",
                           bambudeck_printers[idx].name, bambudeck_printers[idx].prefix);
        bambudeck_save_selected_printer(bambudeck_printers[idx].prefix);
        memset(bambudeck_selected_device_id, 0, sizeof(bambudeck_selected_device_id));
        strncpy(bambudeck_selected_device_id, bambudeck_printers[idx].device_id, sizeof(bambudeck_selected_device_id) - 1);
        bambuStatus.subtask_name[0] = '\0';
        bambudeck_thumbnail_clear();
        bambudeck_ha_poll_now = true;
        bambudeck_ha_ui_dirty = true;
    }
    bambudeck_selector_done = true;
    if (bambudeck_selector_modal != NULL)
    {
        lv_obj_del(bambudeck_selector_modal);
        bambudeck_selector_modal = NULL;
        bambudeck_selector_list = NULL;
    }
    if (bambuDeckConfig.currentScreenIndex == 4)
    {
        loadScreen(0);
    }
}

static void bambudeck_on_selector_close_clicked(lv_event_t *e)
{
    bambudeck_selector_done = true;
    if (bambudeck_selector_modal != NULL)
    {
        lv_obj_del(bambudeck_selector_modal);
        bambudeck_selector_modal = NULL;
        bambudeck_selector_list = NULL;
    }
}

static void bambudeck_on_selector_rescan_clicked(lv_event_t *e)
{
    bambudeck_selector_rescan_req = true;
}

static void bambudeck_on_selector_portal_clicked(lv_event_t *e)
{
    bambudeck_selector_portal_req = true;
}

inline void bambudeck_populate_selector_list()
{
    if (bambudeck_selector_list == NULL)
        return;

    lv_obj_clean(bambudeck_selector_list);

    if (bambudeck_printer_count == 0)
    {
        lv_obj_t *emptyLbl = lv_label_create(bambudeck_selector_list);
        lv_obj_set_width(emptyLbl, lv_pct(100));
        lv_label_set_text_fmt(emptyLbl,
                              LV_SYMBOL_WARNING " No Bambu printers found\n(HA HTTP Code: %d)\nTap Rescan or Setup WiFi/HA",
                              bambudeck_last_http_code);
        lv_obj_set_style_text_align(emptyLbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(emptyLbl, lv_color_hex(0xFFAA00), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(emptyLbl, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }

    for (int i = 0; i < bambudeck_printer_count; i++)
    {
        bool isCurrent = (strcasecmp(bambuDeckConfig.printerName, bambudeck_printers[i].prefix) == 0);

        lv_obj_t *btn = lv_obj_create(bambudeck_selector_list);
        lv_obj_set_width(btn, lv_pct(100));
        lv_obj_set_height(btn, 46);
        lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(btn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(btn, isCurrent ? BAMBU_COLOR_BG_GREEN_TINT() : BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(btn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
        lv_obj_set_style_border_width(btn, isCurrent ? 2 : 1, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(btn, isCurrent ? BAMBU_COLOR_GREEN() : BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(btn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(btn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *nameLbl = lv_label_create(btn);
        lv_label_set_text_fmt(nameLbl, "%s%s (%s)",
                              isCurrent ? LV_SYMBOL_OK " " : "",
                              bambudeck_printers[i].name,
                              bambudeck_printers[i].prefix);
        lv_obj_set_style_text_color(nameLbl, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(nameLbl, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *stateLbl = lv_label_create(btn);
        lv_label_set_text(stateLbl, bambudeck_printers[i].state);
        lv_obj_set_style_text_color(stateLbl, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(stateLbl, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_add_event_cb(btn, bambudeck_on_printer_item_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }
}

inline void bambudeck_show_printer_selector(bool isBootModal)
{
    if (bambudeck_selector_modal != NULL)
    {
        lv_obj_del(bambudeck_selector_modal);
        bambudeck_selector_modal = NULL;
        bambudeck_selector_list = NULL;
    }

    bambudeck_selector_done = false;
    bambudeck_selector_rescan_req = false;
    bambudeck_selector_portal_req = false;

    bambudeck_selector_modal = lv_obj_create(lv_layer_top());
    lv_obj_set_size(bambudeck_selector_modal, 320, 240);
    lv_obj_set_pos(bambudeck_selector_modal, 0, 0);
    lv_obj_set_flex_flow(bambudeck_selector_modal, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(bambudeck_selector_modal, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(bambudeck_selector_modal, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(bambudeck_selector_modal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bambudeck_selector_modal, BAMBU_COLOR_BG_SCREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bambudeck_selector_modal, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bambudeck_selector_modal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bambudeck_selector_modal, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(bambudeck_selector_modal, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Header row
    lv_obj_t *header = lv_obj_create(bambudeck_selector_modal);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_height(header, 32);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, LV_SYMBOL_LIST " Select Bambu Printer");
    lv_obj_set_style_text_color(title, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    if (!isBootModal || bambudeck_printer_count > 0)
    {
        lv_obj_t *closeBtn = lv_label_create(header);
        lv_label_set_text(closeBtn, LV_SYMBOL_CLOSE " Close");
        lv_obj_add_flag(closeBtn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(closeBtn, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(closeBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(closeBtn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(closeBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(closeBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(closeBtn, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(closeBtn, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(closeBtn, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_event_cb(closeBtn, bambudeck_on_selector_close_clicked, LV_EVENT_CLICKED, NULL);
    }

    // Printer list container
    bambudeck_selector_list = lv_obj_create(bambudeck_selector_modal);
    lv_obj_set_width(bambudeck_selector_list, lv_pct(100));
    lv_obj_set_flex_grow(bambudeck_selector_list, 1);
    lv_obj_set_flex_flow(bambudeck_selector_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(bambudeck_selector_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(bambudeck_selector_list, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bambudeck_selector_list, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bambudeck_selector_list, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(bambudeck_selector_list, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bambudeck_selector_list, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bambudeck_selector_list, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(bambudeck_selector_list, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    bambudeck_populate_selector_list();

    // Footer action buttons
    lv_obj_t *footer = lv_obj_create(bambudeck_selector_modal);
    lv_obj_set_width(footer, lv_pct(100));
    lv_obj_set_height(footer, 36);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(footer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(footer, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *rescanBtn = lv_label_create(footer);
    lv_obj_set_flex_grow(rescanBtn, 1);
    lv_label_set_text(rescanBtn, LV_SYMBOL_REFRESH " Rescan HA");
    lv_obj_add_flag(rescanBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_align(rescanBtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(rescanBtn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(rescanBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(rescanBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(rescanBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(rescanBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(rescanBtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(rescanBtn, bambudeck_on_selector_rescan_clicked, LV_EVENT_CLICKED, NULL);

    lv_obj_t *portalBtn = lv_label_create(footer);
    lv_obj_set_flex_grow(portalBtn, 1);
    lv_label_set_text(portalBtn, LV_SYMBOL_WIFI " WiFi / HA Setup");
    lv_obj_add_flag(portalBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_align(portalBtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(portalBtn, BAMBU_COLOR_ORANGE(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(portalBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(portalBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(portalBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(portalBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(portalBtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(portalBtn, bambudeck_on_selector_portal_clicked, LV_EVENT_CLICKED, NULL);

    if (isBootModal)
    {
        while (!bambudeck_selector_done)
        {
            lv_timer_handler();
            lv_task_handler();
            if (bambudeck_selector_rescan_req)
            {
                bambudeck_selector_rescan_req = false;
                bambudeck_ha_discover_printers();
                bambudeck_populate_selector_list();
            }
            if (bambudeck_selector_portal_req)
            {
                bambudeck_selector_portal_req = false;
                if (bambudeck_selector_modal != NULL)
                {
                    lv_obj_del(bambudeck_selector_modal);
                    bambudeck_selector_modal = NULL;
                    bambudeck_selector_list = NULL;
                }
                bambudeck_run_setup_portal();
                extern DynamicJsonDocument bambudeck_load_config();
                bambudeck_load_config();
                bambudeck_ha_discover_printers();
                bambudeck_show_printer_selector(true);
                return;
            }
            delay(10);
        }
    }
}

extern "C" void bambudeck_open_printer_selector(void)
{
    bambudeck_open_selector_req = true;
}

// Dispatches LVGL messages on Core 1 (main UI thread)
inline void bambudeck_ha_process_ui()
{
    if (bambudeck_open_selector_req)
    {
        bambudeck_open_selector_req = false;
        bambudeck_ha_discover_printers();
        bambudeck_show_printer_selector(false);
    }

    if (bambudeck_selector_rescan_req)
    {
        bambudeck_selector_rescan_req = false;
        bambudeck_ha_discover_printers();
        bambudeck_populate_selector_list();
    }

    if (bambudeck_selector_portal_req)
    {
        bambudeck_selector_portal_req = false;
        if (bambudeck_selector_modal != NULL)
        {
            lv_obj_del(bambudeck_selector_modal);
            bambudeck_selector_modal = NULL;
            bambudeck_selector_list = NULL;
        }
        loadScreen(-1);
        bambudeck_run_setup_portal();
        ESP.restart();
        return;
    }

    if (bambudeck_ha_pending_error_hide)
    {
        bambudeck_ha_pending_error_hide = false;
        ui_hmsPanel_hide();
    }

    if (bambudeck_ha_pending_error_show)
    {
        bambudeck_ha_pending_error_show = false;
        ui_hmsPanel_show(bambudeck_ha_pending_error_msg, bambudeck_ha_pending_error_style);
    }

    bambudeck_feed_check_timeout();

    if (!bambudeck_ha_ui_dirty)
        return;
    bambudeck_ha_ui_dirty = false;

    bambudeck_device_set_print_state(String(bambudeck_ha_pending_state));
    bambudeck_send_msg(BAMBUDECK_ON_NOZZLE_TEMP, bambuStatus.nozzle_temper);
    bambudeck_send_msg(BAMBUDECK_ON_NOZZLE_TARGET_TEMP, bambuStatus.nozzle_target_temper);
    bambudeck_send_msg(BAMBUDECK_ON_BED_TEMP, bambuStatus.bed_temper);
    bambudeck_send_msg(BAMBUDECK_ON_BED_TARGET_TEMP, bambuStatus.bed_target_temper);
    bambudeck_send_msg(BAMBUDECK_ON_CHAMBER_TEMP, bambuStatus.chamber_temper);
    bambudeck_send_msg(BAMBUDECK_ON_PART_FAN_SPEED, bambuStatus.cooling_fan_speed);
    bambudeck_send_msg(BAMBUDECK_ON_PART_AUX_SPEED, bambuStatus.big_fan1_speed);
    bambudeck_send_msg(BAMBUDECK_ON_PART_CHAMBER_SPEED, bambuStatus.big_fan2_speed);
    bambudeck_send_msg(BAMBUDECK_ON_LIGHT_REPORT, bambuStatus.chamberLed ? 1 : 0);
    bambudeck_send_msg(BAMBUDECK_ON_WIFI_SIGNAL, bambuStatus.wifi_signal);
    bambudeck_send_msg(BAMBUDECK_ON_AMS, bambuStatus.ams ? 1 : 0);
    bambudeck_update_ams_ui_colors();
    bambudeck_thumbnail_update_ui();
    bambudeck_send_msg(BAMBUDECK_ON_PRINT_STATUS, 0);
}

inline void bambudeck_ha_worker_task(void *param)
{
    unsigned long lastPoll = millis();
    for (;;)
    {
        if (bambudeck_ha_cmd_pending)
        {
            bambudeck_ha_cmd_pending = false;
            String path = String(bambudeck_ha_cmd_path);
            String body = String(bambudeck_ha_cmd_body);
            bambudeck_ha_post(path, body, NULL, 3000);
            bambudeck_ha_poll_now = true;
        }

        if (bambudeck_ha_pending_chamber_temp > -50 && bambudeck_ha_pending_chamber_temp < 125)
        {
            int temp = bambudeck_ha_pending_chamber_temp;
            bambudeck_ha_pending_chamber_temp = -999;
            DynamicJsonDocument doc(384);
            doc["state"] = String(temp);
            JsonObject attrs = doc.createNestedObject("attributes");
            attrs["unit_of_measurement"] = "°C";
            attrs["device_class"] = "temperature";
            attrs["state_class"] = "measurement";
            attrs["friendly_name"] = "BambuDeck Chamber Temperature";
            String payload;
            serializeJson(doc, payload);
            bambudeck_ha_post("/api/states/sensor.bambudeck_chamber_temperature", payload, NULL, 2500);
        }

        if (bambudeck_selector_modal == NULL && (bambudeck_ha_poll_now || (millis() - lastPoll >= 3000)))
        {
            bambudeck_ha_poll_now = false;
            lastPoll = millis();
            if (bambudeck_ha_poll_status_worker())
            {
                bambudeck_thumbnail_check_download();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

inline void bambudeck_ha_start_task()
{
    bambudeck_ha_init_mutex();
    if (bambudeck_ha_task_handle == NULL)
    {
        // Pin to Core 1 (APP_CPU) with 16KB stack so it time-slices cleanly with loopTask without cross-core LWIP issues
        xTaskCreatePinnedToCore(
            bambudeck_ha_worker_task,
            "bambudeck_ha",
            16384,
            NULL,
            1,
            &bambudeck_ha_task_handle,
            1);
    }
}

inline void bambudeck_ha_setup()
{
    ConsoleInfo.println(F("[BambuDeck][HA] Direct Home Assistant REST API mode enabled"));
    lv_label_set_text(introScreenCaption, LV_SYMBOL_CHARGE " Discovering Printers in HA...");
    lv_timer_handler();
    lv_task_handler();
    delay(50);

    int count = bambudeck_ha_discover_printers();
    bool hasSaved = strlen(bambuDeckConfig.printerName) > 0;
    bool savedFound = false;
    for (int i = 0; hasSaved && i < count; i++)
    {
        if (strcasecmp(bambudeck_printers[i].prefix, bambuDeckConfig.printerName) == 0)
            savedFound = true;
    }

    if (!hasSaved && count == 1)
    {
        bambudeck_save_selected_printer(bambudeck_printers[0].prefix);
    }
    else if (!hasSaved || (count > 0 && !savedFound))
    {
        bambudeck_show_printer_selector(true);
    }
    else
    {
        ConsoleInfo.printf("[BambuDeck][HA] Using last selected printer: %s\n", bambuDeckConfig.printerName);
    }

    bambudeck_ha_poll_status_worker();
    ui_homeScreen_screen_init();
    lv_disp_load_scr(ui_homeScreen);
    bambudeck_screen_startScreenTimer();
    lv_timer_handler();
    lv_task_handler();
    bambudeck_ha_process_ui();
    bambudeck_ha_start_task();
}
