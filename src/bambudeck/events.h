#ifndef _BAMBUDECK_EVENTS
#define _BAMBUDECK_EVENTS

#include <Arduino.h>
#include <LittleFS.h>
#include "ui/ui.h"
#include "ui/ui_msgs.h"
#include "types.h"
#include "paths.h"
#include "settings.h"
#include "firmware.h"
#include "device.h"
#include "sensors/chamber.h"

inline void bambudeck_send_msg(BAMBUDECK_MESSAGE message, unsigned long long data = 0)
{
    BAMBUDECK_MESSAGE_DATA eventData;
    eventData.data = data;
    lv_msg_send(message, &eventData);
}

// Backward-compatible alias
inline void bambudeck_mqtt_sendMsg(BAMBUDECK_MESSAGE message, unsigned long long data = 0)
{
    bambudeck_send_msg(message, data);
}

inline void bambudeck_events_onResetDevice(lv_msg_t *m)
{
    (void)m;
    ESP.restart();
}

inline void bambudeck_events_onReconfigWifi(lv_msg_t *m)
{
    (void)m;
    LittleFS.remove(bambudeck_paths_config);
    ESP.restart();
}

inline void bambudeck_events_onBackLight(lv_msg_t *m)
{
    (void)m;
    int32_t value = lv_slider_get_value(ui_settingsBackLightPanelSlider);
    bambuDeckConfig.backlightLevel = value;
    bambudeck_screen_setBrightness(value);
}

inline void bambudeck_events_onBackLightSet(lv_msg_t *m)
{
    (void)m;
    int32_t value = lv_slider_get_value(ui_settingsBackLightPanelSlider);
    bambuDeckConfig.backlightLevel = value;
    bambudeck_settings_save();
    bambudeck_screen_setBrightness(value);
}

inline void bambudeck_events_onTFTTimerSet(lv_msg_t *m)
{
    (void)m;
    int32_t value = lv_slider_get_value(ui_settingsTFTOFFSlider);
    bambudeck_screen_setScreenTimer(value * 1000 * 60);
    bambuDeckConfig.tftOffValue = value;
    bambudeck_settings_save();
}

inline void bambudeck_events_onTFTInvert(lv_msg_t *m)
{
    (void)m;
    bool value = lv_obj_has_state(ui_settingsTFTInvertSwitch, LV_STATE_CHECKED);
    bambuDeckConfig.tftInvert = value;
    bambudeck_settings_save();
    bambudeck_screen_invertColors();
}

inline void bambudeck_events_onSettingsSave(lv_msg_t *m)
{
    (void)m;
    bambudeck_settings_save();
}

inline void bambudeck_events_onTFTFlip(lv_msg_t *m)
{
    (void)m;
    bambudeck_screen_toggleTFTFlip();
}

inline void bambudeck_events_onResetTouch(lv_msg_t *m)
{
    (void)m;
    bambudeck_resetTouchConfig();
}

inline void bambudeck_events_onChamberTempSwitch(lv_msg_t *m)
{
    (void)m;
    if (bambuDeckConfig.chamberSensorEnabled)
    {
        bambudeck_chamber_timer_start();
    }
    else
    {
        bambudeck_chamber_timer_stop();
    }
}

inline void bambudeck_events_onCheckUpdate(lv_msg_t *m)
{
    (void)m;
    bambudeck_firmware_interactive_check();
}

inline void bambudeck_setupGlobalEvents()
{
    // Settings events
    lv_msg_subscribe(BAMBUDECK_SETTINGS_RESET_DEVICE, (lv_msg_subscribe_cb_t)bambudeck_events_onResetDevice, NULL);
    lv_msg_subscribe(BAMBUDECK_SETTINGS_BACKLIGHT, (lv_msg_subscribe_cb_t)bambudeck_events_onBackLight, NULL);
    lv_msg_subscribe(BAMBUDECK_SETTINGS_BACKLIGHT_SET, (lv_msg_subscribe_cb_t)bambudeck_events_onBackLightSet, NULL);
    lv_msg_subscribe(BAMBUDECK_SETTINGS_TFTOFF_SET, (lv_msg_subscribe_cb_t)bambudeck_events_onTFTTimerSet, NULL);
    lv_msg_subscribe(BAMBUDECK_SETTINGS_TFT_INVERT, (lv_msg_subscribe_cb_t)bambudeck_events_onTFTInvert, NULL);
    lv_msg_subscribe(BAMBUDECK_SETTINGS_SAVE, (lv_msg_subscribe_cb_t)bambudeck_events_onSettingsSave, NULL);
    lv_msg_subscribe(BAMBUDECK_SETTINGS_CHAMBER_TEMP, (lv_msg_subscribe_cb_t)bambudeck_events_onChamberTempSwitch, NULL);
    lv_msg_subscribe(BAMBUDECK_SETTINGS_TFT_FLIP, (lv_msg_subscribe_cb_t)bambudeck_events_onTFTFlip, NULL);
    lv_msg_subscribe(BAMBUDECK_SETTINGS_RECONFIG_WIFI, (lv_msg_subscribe_cb_t)bambudeck_events_onReconfigWifi, NULL);
    lv_msg_subscribe(BAMBUDECK_SETTINGS_CHECK_UPDATE, (lv_msg_subscribe_cb_t)bambudeck_events_onCheckUpdate, NULL);

    // Device command events
    lv_msg_subscribe(BAMBUDECK_COMMAND_LIGHT_TOGGLE, (lv_msg_subscribe_cb_t)bambudeck_device_onLightToggleCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_STOP, (lv_msg_subscribe_cb_t)bambudeck_device_onStopCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_PAUSE, (lv_msg_subscribe_cb_t)bambudeck_device_onPauseCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_RESUME, (lv_msg_subscribe_cb_t)bambudeck_device_onResumeCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_HOME, (lv_msg_subscribe_cb_t)bambudeck_device_onHomeCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_LEFT, (lv_msg_subscribe_cb_t)bambudeck_device_onLeftCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_RIGHT, (lv_msg_subscribe_cb_t)bambudeck_device_onRightCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_UP, (lv_msg_subscribe_cb_t)bambudeck_device_onUpCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_DOWN, (lv_msg_subscribe_cb_t)bambudeck_device_onDownCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_BED_TARGET_TEMP, (lv_msg_subscribe_cb_t)bambudeck_device_onBedTargetTempCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_NOZZLE_TARGET_TEMP, (lv_msg_subscribe_cb_t)bambudeck_device_onNozzleTargetCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_PART_FAN_SPEED, (lv_msg_subscribe_cb_t)bambudeck_device_onPartSpeedCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_AUX_FAN_SPEED, (lv_msg_subscribe_cb_t)bambudeck_device_onAuxSpeedCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_CHAMBER_FAN_SPEED, (lv_msg_subscribe_cb_t)bambudeck_device_onChamberSpeedCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_PRINT_SPEED, (lv_msg_subscribe_cb_t)bambudeck_device_onPrintSpeedCommand, NULL);
    lv_msg_subscribe(BAMBUDECK_CONTROL_INC_SWITCH, (lv_msg_subscribe_cb_t)bambudeck_device_onIncSwitchCommand, NULL);

    lv_msg_subscribe(BAMBUDECK_COMMAND_UNLOAD_FILAMENT, (lv_msg_subscribe_cb_t)bambudeck_device_onUnloadFilament, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_LOAD_FILAMENT, (lv_msg_subscribe_cb_t)bambudeck_device_onLoadFilament, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_AMS_CONTROL, (lv_msg_subscribe_cb_t)bambudeck_device_command_ams_control, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_AMS_REFRESH, (lv_msg_subscribe_cb_t)bambudeck_device_onAmsRefresh, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_CLEAN_PRINT_ERROR, (lv_msg_subscribe_cb_t)bambudeck_device_command_clean_print_error, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_EXTRUDE_UP, (lv_msg_subscribe_cb_t)bambudeck_device_onNozzleUp, NULL);
    lv_msg_subscribe(BAMBUDECK_COMMAND_EXTRUDE_DOWN, (lv_msg_subscribe_cb_t)bambudeck_device_onNozzleDown, NULL);
}

#endif
