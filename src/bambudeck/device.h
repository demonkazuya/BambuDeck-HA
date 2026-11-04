#ifndef _BAMBUDECK_DEVICE
#define _BAMBUDECK_DEVICE

#include <Arduino.h>
#include <ArduinoJson.h>
#include "types.h"
#include "ha_api.h"
#include "debug.h"

#define BAMBUDECK_DEVICE_CONTROL_MOVE_SPEED_XY 3000
#define BAMBUDECK_DEVICE_CONTROL_MOVE_SPEED_Z  1500

static String lastPrintState = "IDLE";

inline void bambudeck_device_set_print_state(String state)
{
    if (state == "IDLE")
        bambuStatus.print_status = BAMBUDECK_PRINT_STATUS_IDLE;
    else if (state == "RUNNING")
        bambuStatus.print_status = BAMBUDECK_PRINT_STATUS_RUNNING;
    else if (state == "PAUSE")
        bambuStatus.print_status = BAMBUDECK_PRINT_STATUS_PAUSED;
    else if (state == "FINISH")
        bambuStatus.print_status = BAMBUDECK_PRINT_STATUS_FINISHED;
    else if (state == "PREPARE")
        bambuStatus.print_status = BAMBUDECK_PRINT_STATUS_PREPARE;
    else if (state == "FAILED")
        bambuStatus.print_status = BAMBUDECK_PRINT_STATUS_FAILED;

    if (lastPrintState != state && bambuDeckConfig.wakeOnPrint && state != "IDLE" && state != "FINISH" && state != "FAILED")
    {
        bambudeck_screen_wakeUp();
    }

    lastPrintState = state;
}

inline void bambudeck_device_set_printing_speed(int lvl)
{
    if (strlen(bambuDeckConfig.printerName) == 0)
        return;

    const char *opt = "standard";
    if (lvl == 1)
        opt = "silent";
    else if (lvl == 3)
        opt = "sport";
    else if (lvl == 4)
        opt = "ludicrous";

    DynamicJsonDocument doc(256);
    doc["entity_id"] = String("select.") + String(bambuDeckConfig.printerName) + "_printing_speed";
    doc["option"] = opt;
    bambudeck_ha_call_service("select", "select_option", doc);
    bambuStatus.printing_speed_lvl = lvl;
    bambudeck_ha_poll_now = true;
}

inline void bambudeck_device_gcode_line(String line)
{
    bambudeck_ha_send_gcode(line);
}

extern "C" void bambudeck_send_gcode(const char *cmd)
{
    if (cmd != NULL)
    {
        bambudeck_device_gcode_line(String(cmd));
    }
}

inline void bambudeck_device_move_axis(String axis, float value, int speed = BAMBUDECK_DEVICE_CONTROL_MOVE_SPEED_XY)
{
    (void)speed;
    bambudeck_ha_move_axis(axis, (int)round(value));
}

inline void bambudeck_device_toggle_light()
{
    if (strlen(bambuDeckConfig.printerName) == 0)
        return;

    DynamicJsonDocument doc(256);
    doc["entity_id"] = String("light.") + String(bambuDeckConfig.printerName) + "_chamber_light";
    bambudeck_ha_call_service("light", "toggle", doc);
    bambuStatus.chamberLed = !bambuStatus.chamberLed;
    bambudeck_ha_poll_now = true;
}

inline void bambudeck_device_set_bed_target_temp()
{
    if (strlen(bambuDeckConfig.printerName) == 0)
        return;

    DynamicJsonDocument doc(256);
    doc["entity_id"] = String("number.") + String(bambuDeckConfig.printerName) + "_bed_target_temperature";
    doc["value"] = controlMode.target_bed_temper;
    bambudeck_ha_call_service("number", "set_value", doc);
    bambudeck_ha_poll_now = true;
}

inline void bambudeck_device_set_nozzle_target_temp()
{
    if (strlen(bambuDeckConfig.printerName) == 0)
        return;

    DynamicJsonDocument doc(256);
    doc["entity_id"] = String("number.") + String(bambuDeckConfig.printerName) + "_nozzle_target_temperature";
    doc["value"] = controlMode.target_nozzle_temper;
    bambudeck_ha_call_service("number", "set_value", doc);
    bambudeck_ha_poll_now = true;
}

inline void bambudeck_device_stop_print()
{
    bambudeck_ha_press_button("stop_printing");
}

inline void bambudeck_device_pause_print()
{
    bambudeck_ha_press_button("pause_printing");
}

inline void bambudeck_device_resume_print()
{
    bambudeck_ha_press_button("resume_printing");
}

inline void bambudeck_device_onNozzleUp(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_gcode_line("M83 \nG0 E-10.0 F900\n");
}

inline void bambudeck_device_onNozzleDown(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_gcode_line("M83 \nG0 E10.0 F900\n");
}

inline bool bambudeck_can_load_filament()
{
    if (bambuStatus.ams_exist_bits == 0)
        return true;
    return (bambuStatus.ams_status_main == AMS_STATUS_MAIN_IDLE && bambuStatus.m_tray_now == 255);
}

inline void bambudeck_device_onLoadFilament(lv_msg_t *m)
{
    (void)m;
    if (bambudeck_can_load_filament())
    {
        bambudeck_device_gcode_line("M620 S254\nM106 S255\nM104 S250\nM17 S\nM17 X0.5 Y0.5\nG91\nG1 Y-5 F1200\nG1 Z3\nG90\nG28 X\nM17 R\nG1 X70 F21000\nG1 Y245\nG1 Y265 F3000\nG4\nM106 S0\nM109 S250\nG1 X90\nG1 Y255\nG1 X120\nG1 X20 Y50 F21000\nG1 Y-3\nT254\nG1 X54\nG1 Y265\nG92 E0\nG1 E40 F180\nG4\nG1 X70 F15000\nG1 X76\nG1 X65\nG1 X76\nG1 X65\nG1 X90 F3000\nG1 Y255\nG1 X100\nG1 Y265\nG1 X70 F10000\nG1 X100 F5000\nG1 X70 F10000\nG1 X100 F5000\nG1 X165 F12000\nG1 Y245\nG1 X70\nG1 Y265 F3000\nG91\nG1 Z-3 F1200\nG90\nM621 S254\n\n");
    }
}

inline void bambudeck_device_onUnloadFilament(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_gcode_line("M620 S255\nM106 P1 S255\nM104 S250\nM17 S\nM17 X0.5 Y0.5\nG91\nG1 Y-5 F3000\nG1 Z3 F1200\nG90\nG28 X\nM17 R\nG1 X70 F21000\nG1 Y245\nG1 Y265 F3000\nG4\nM106 P1 S0\nM109 S250\nG1 X90 F3000\nG1 Y255 F4000\nG1 X100 F5000\nG1 X120 F21000\nG1 X20 Y50\nG1 Y-3\nT255\nG4\nG1 X70 F3000\n\nG91\nG1 Z-3 F1200\nG90\nM621 S255\n\n");
}

inline void bambudeck_device_onAmsRefresh(lv_msg_t *m)
{
    (void)m;
    bambudeck_ha_press_button("reread_rfid");
    bambudeck_ha_poll_now = true;
}

inline void bambudeck_device_command_ams_control(lv_msg_t *m)
{
    const char *action = (const char *)lv_msg_get_user_data(m);
    if (!action)
        return;

    if (strcmp(action, "resume") == 0 || strcmp(action, "done") == 0)
    {
        bambudeck_ha_press_button("resume_printing");
    }
    else if (strcmp(action, "pause") == 0)
    {
        bambudeck_ha_press_button("pause_printing");
    }

    DynamicJsonDocument doc(256);
    if (strlen(bambudeck_selected_device_id) > 0)
        doc["device_id"] = (const char *)bambudeck_selected_device_id;
    else if (strlen(bambuDeckConfig.printerName) > 0)
        doc["entity_id"] = String("sensor.") + String(bambuDeckConfig.printerName) + "_print_status";
    doc["command"] = "ams_control";
    doc["param"] = action;
    bambudeck_ha_call_service("bambu_lab", "send_command", doc);
    bambudeck_ha_poll_now = true;
}

inline void bambudeck_device_command_clean_print_error(lv_msg_t *m)
{
    (void)m;
    bambudeck_ha_press_button("resume_printing");
    DynamicJsonDocument doc(256);
    if (strlen(bambudeck_selected_device_id) > 0)
        doc["device_id"] = (const char *)bambudeck_selected_device_id;
    else if (strlen(bambuDeckConfig.printerName) > 0)
        doc["entity_id"] = String("sensor.") + String(bambuDeckConfig.printerName) + "_print_status";
    doc["command"] = "clean_print_error";
    bambudeck_ha_call_service("bambu_lab", "send_command", doc);
    bambudeck_ha_poll_now = true;
}

/* UI Event Handlers matching ui_msgs.h */
inline void bambudeck_device_onLightToggleCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_toggle_light();
}

inline void bambudeck_device_onHomeCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_gcode_line("G28 \n");
}

inline void bambudeck_device_onLeftCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_move_axis("X", controlMode.inc == 10 ? -10 : -1, BAMBUDECK_DEVICE_CONTROL_MOVE_SPEED_XY);
}

inline void bambudeck_device_onRightCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_move_axis("X", controlMode.inc == 10 ? 10 : 1, BAMBUDECK_DEVICE_CONTROL_MOVE_SPEED_XY);
}

inline void bambudeck_device_onUpCommand(lv_msg_t *m)
{
    (void)m;
    String axis = controlMode.axis == ControlAxisXY ? "Y" : "Z";
    int multiplier = axis == "Y" ? 1 : -1;
    bambudeck_device_move_axis(axis, controlMode.inc == 10 ? 10 * multiplier : 1 * multiplier, axis == "Y" ? BAMBUDECK_DEVICE_CONTROL_MOVE_SPEED_XY : BAMBUDECK_DEVICE_CONTROL_MOVE_SPEED_Z);
}

inline void bambudeck_device_onDownCommand(lv_msg_t *m)
{
    (void)m;
    String axis = controlMode.axis == ControlAxisXY ? "Y" : "Z";
    int multiplier = axis == "Y" ? -1 : 1;
    bambudeck_device_move_axis(axis, controlMode.inc == 10 ? 10 * multiplier : 1 * multiplier, axis == "Y" ? BAMBUDECK_DEVICE_CONTROL_MOVE_SPEED_XY : BAMBUDECK_DEVICE_CONTROL_MOVE_SPEED_Z);
}

inline void bambudeck_device_onBedTargetTempCommand(lv_msg_t *m)
{
    (void)m;
    bambuStatus.bed_target_temper = controlMode.target_bed_temper;
    bambudeck_device_set_bed_target_temp();
}

inline void bambudeck_device_onNozzleTargetCommand(lv_msg_t *m)
{
    (void)m;
    bambuStatus.nozzle_target_temper = controlMode.target_nozzle_temper;
    bambudeck_device_set_nozzle_target_temp();
}

inline void bambudeck_device_onPartSpeedCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_gcode_line("M106 P1 S" + String(bambuStatus.cooling_fan_speed) + " \n");
}

inline void bambudeck_device_onAuxSpeedCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_gcode_line("M106 P2 S" + String(bambuStatus.big_fan1_speed) + "\n");
}

inline void bambudeck_device_onChamberSpeedCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_gcode_line("M106 P3 S" + String(bambuStatus.big_fan2_speed) + "\n");
}

inline void bambudeck_device_onPrintSpeedCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_set_printing_speed(bambuStatus.printing_speed_lvl);
}

inline void bambudeck_device_onIncSwitchCommand(lv_msg_t *m)
{
    (void)m;
    controlMode.inc = controlMode.inc == 1 ? 10 : 1;
}

inline void bambudeck_device_onStopCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_stop_print();
}

inline void bambudeck_device_onPauseCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_pause_print();
}

inline void bambudeck_device_onResumeCommand(lv_msg_t *m)
{
    (void)m;
    bambudeck_device_resume_print();
}

#endif
