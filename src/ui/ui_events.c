#include "ui.h"
#include "../../src/bambudeck/types.h"

void initialActions(lv_event_t *e) {}

/* -----------Sidebar-------------- */
void onSidebarHome(lv_event_t *e) { loadScreen(0); }
void onSidebarTemp(lv_event_t *e) { loadScreen(1); }
void onSidebarControl(lv_event_t *e) { loadScreen(2); }
void onSidebarNozzle(lv_event_t *e) { loadScreen(3); }
void onSidebarSettings(lv_event_t *e) { loadScreen(4); }

/* -----------Home-------------- */
void onHomeControllerPlayPauseConfirm()
{
    lv_obj_t *target = ui_comp_get_child(ui_homeComponent, UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER_MAINSCREENPLAYPAUSEBUTTON);
    lv_msg_send(BAMBUDECK_COMMAND_PAUSE, NULL);
    lv_obj_add_state(target, LV_STATE_DISABLED);
}
void onHomeControllerPlayPause(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    switch (bambuStatus.print_status)
    {
    case BAMBUDECK_PRINT_STATUS_PAUSED:
        lv_msg_send(BAMBUDECK_COMMAND_RESUME, NULL);
        lv_obj_add_state(target, LV_STATE_DISABLED);
        break;
    case BAMBUDECK_PRINT_STATUS_RUNNING:
    case BAMBUDECK_PRINT_STATUS_PREPARE:
        ui_confirmPanel_show(LV_SYMBOL_WARNING " Pause Print?", onHomeControllerPlayPauseConfirm);
        break;
    }

    // cui_mainScreenPlayPauseButton
}

void onHomeControllerStopConfirm() { lv_msg_send(BAMBUDECK_COMMAND_STOP, NULL); }
void onHomeControllerStop(lv_event_t *e)
{
    ui_confirmPanel_show(LV_SYMBOL_WARNING " Cancel Print?", onHomeControllerStopConfirm);
}
void onHomeSpeedSelection(lv_event_t *e) {}
void onHomeLight(lv_event_t *e) { lv_msg_send(BAMBUDECK_COMMAND_LIGHT_TOGGLE, NULL); }
void onHomeBedTemp(lv_event_t *e)
{
    loadScreen(1);

    ui_temperatureComponent_show_keypad_and_back(0, 0, 0);

    lv_obj_t *target = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED);
    lv_obj_add_state(target, LV_STATE_CHECKED);
}
void onHomeNozzleTemp(lv_event_t *e, int backToScreen)
{
    loadScreen(1);
    ui_temperatureComponent_show_keypad_and_back(0, 1, backToScreen);

    lv_obj_t *target = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE);
    lv_obj_add_state(target, LV_STATE_CHECKED);
}

/* -----------Temps-------------- */
void onTemperatureBed(const char *value)
{
    LV_LOG_USER("onTemperatureBed: %s", value);

    lv_msg_send(BAMBUDECK_COMMAND_BED_TARGET_TEMP, value);
}
void onTemperatureNozzle(int32_t value) { lv_msg_send(BAMBUDECK_COMMAND_NOZZLE_TARGET_TEMP, &value); }
void onTemperaturePart(lv_event_t *e) { lv_msg_send(BAMBUDECK_CONTROL_INC_SWITCH, NULL); }
void onTemperatureAux(lv_event_t *e) { lv_msg_send(BAMBUDECK_CONTROL_INC_SWITCH, NULL); }
void onTemperatureKeypad(lv_event_t *e) { lv_msg_send(BAMBUDECK_CONTROL_INC_SWITCH, NULL); }

/* -----------Control-------------- */
void onControlRange(lv_event_t *e) { lv_msg_send(BAMBUDECK_CONTROL_INC_SWITCH, NULL); }
void onControlHome(lv_event_t *e) { lv_msg_send(BAMBUDECK_COMMAND_HOME, NULL); }
void onControlLeft(lv_event_t *e) { lv_msg_send(BAMBUDECK_COMMAND_LEFT, NULL); }
void onControlRight(lv_event_t *e) { lv_msg_send(BAMBUDECK_COMMAND_RIGHT, NULL); }
void onControlUp(lv_event_t *e) { lv_msg_send(BAMBUDECK_COMMAND_UP, NULL); }
void onControlDown(lv_event_t *e) { lv_msg_send(BAMBUDECK_COMMAND_DOWN, NULL); }
void onControlAxis(lv_event_t *e) { lv_msg_send(BAMBUDECK_CONTROL_AXIS_SWITCH, NULL); }

/* Settings */

void onSettingsResetDeviceConfirm() { lv_msg_send(BAMBUDECK_SETTINGS_RESET_DEVICE, NULL); }
void onSettingsResetDevice(lv_event_t *e)
{
    ui_confirmPanel_show(LV_SYMBOL_WARNING " REBOOT", onSettingsResetDeviceConfirm);
}

void onSettingsWOP(lv_event_t *e)
{
    bambuDeckConfig.wakeOnPrint = !bambuDeckConfig.wakeOnPrint;
    lv_msg_send(BAMBUDECK_SETTINGS_SAVE, NULL);
}

void onSettingsAuxFan(lv_event_t *e)
{
    bambuDeckConfig.auxFanEnabled = !bambuDeckConfig.auxFanEnabled;
    lv_msg_send(BAMBUDECK_SETTINGS_SAVE, NULL);
}

void onSettingsChamberFan(lv_event_t *e)
{
    bambuDeckConfig.chamberFanEnabled = !bambuDeckConfig.chamberFanEnabled;
    lv_msg_send(BAMBUDECK_SETTINGS_SAVE, NULL);
}

void onSettingsChamberSensor(lv_event_t *e)
{
    bambuDeckConfig.chamberSensorEnabled = !bambuDeckConfig.chamberSensorEnabled;
    lv_msg_send(BAMBUDECK_SETTINGS_CHAMBER_TEMP, NULL);
    lv_msg_send(BAMBUDECK_SETTINGS_SAVE, NULL);
}

void onSettingsTFTFlipConfirm()
{
    lv_msg_send(BAMBUDECK_SETTINGS_TFT_FLIP, NULL);
}

void onSettingsOTA(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);
    bambuDeckConfig.otaEnabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    lv_msg_send(BAMBUDECK_SETTINGS_SAVE, NULL);
}

void onSettingsTFTFlip(lv_event_t *e)
{
    ui_confirmPanel_show(LV_SYMBOL_WARNING " Flip LCD\n" LV_SYMBOL_REFRESH " Reboot", onSettingsTFTFlipConfirm);
}

/* filament */
void onNozzleUp(lv_event_t *e)
{
    lv_msg_send(BAMBUDECK_COMMAND_EXTRUDE_UP, NULL);
}

void onNozzleDown(lv_event_t *e)
{
    lv_msg_send(BAMBUDECK_COMMAND_EXTRUDE_DOWN, NULL);
}

static lv_obj_t *bambudeck_feed_modal = NULL;
static lv_obj_t *bambudeck_feed_status_lbl = NULL;
static bool bambudeck_feed_is_load = true;
static uint32_t bambudeck_feed_modal_open_time = 0;

static void bambudeck_feed_on_close(lv_event_t *e)
{
    bambudeck_hide_feed_dialog();
}

static void bambudeck_feed_on_purge_retract(lv_event_t *e)
{
    bambudeck_feed_modal_open_time = millis();
    if (bambudeck_feed_is_load)
    {
        bambudeck_send_gcode("G1 X54 Y265 F3000\nG92 E0\nG1 E25 F180\nG1 X70 F10000\nG1 X100 F5000\nG1 X70 F10000\nG1 X100 F5000\n");
        if (bambudeck_feed_status_lbl != NULL)
        {
            lv_label_set_text(bambudeck_feed_status_lbl, "Purging +25mm...\nHas filament extruded?");
        }
    }
    else
    {
        bambudeck_send_gcode("M83\nG0 E-25.0 F900\n");
        if (bambudeck_feed_status_lbl != NULL)
        {
            lv_label_set_text(bambudeck_feed_status_lbl, "Retracting -25mm...\nHas filament pulled out?");
        }
    }
}

static void bambudeck_feed_on_retry(lv_event_t *e)
{
    bambudeck_feed_modal_open_time = millis();
    if (bambudeck_feed_is_load)
    {
        lv_msg_send(BAMBUDECK_COMMAND_LOAD_FILAMENT, NULL);
        if (bambudeck_feed_status_lbl != NULL)
        {
            lv_label_set_text(bambudeck_feed_status_lbl, "Retrying Load...\nHas filament extruded?");
        }
    }
    else
    {
        lv_msg_send(BAMBUDECK_COMMAND_UNLOAD_FILAMENT, NULL);
        if (bambudeck_feed_status_lbl != NULL)
        {
            lv_label_set_text(bambudeck_feed_status_lbl, "Retrying Unload...\nHas filament pulled out?");
        }
    }
}

static void bambudeck_feed_on_done(lv_event_t *e)
{
    int32_t zero = 0;
    onTemperatureNozzle(zero);
    bambudeck_send_gcode("M104 S0\n");
    bambudeck_hide_feed_dialog();
}

void bambudeck_show_feed_dialog(bool isLoad)
{
    if (bambudeck_feed_modal != NULL)
    {
        lv_obj_del(bambudeck_feed_modal);
        bambudeck_feed_modal = NULL;
    }

    bambudeck_feed_is_load = isLoad;
    bambudeck_feed_modal_open_time = millis();

    bambudeck_feed_modal = lv_obj_create(lv_layer_top());
    lv_obj_set_size(bambudeck_feed_modal, 320, 240);
    lv_obj_set_pos(bambudeck_feed_modal, 0, 0);
    lv_obj_set_flex_flow(bambudeck_feed_modal, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bambudeck_feed_modal, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(bambudeck_feed_modal, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(bambudeck_feed_modal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bambudeck_feed_modal, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bambudeck_feed_modal, 210, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bambudeck_feed_modal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *card = lv_obj_create(bambudeck_feed_modal);
    lv_obj_set_width(card, 296);
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(card, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(card, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(card, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(card, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(card, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(card, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *header = lv_obj_create(card);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_height(header, 30);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, isLoad ? (LV_SYMBOL_PLAY " Filament Load") : (LV_SYMBOL_REFRESH " Filament Unload"));
    lv_obj_set_style_text_color(title, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

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
    lv_obj_set_style_text_font(closeBtn, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(closeBtn, bambudeck_feed_on_close, LV_EVENT_CLICKED, NULL);

    bambudeck_feed_status_lbl = lv_label_create(card);
    lv_obj_set_width(bambudeck_feed_status_lbl, lv_pct(100));
    lv_label_set_text(bambudeck_feed_status_lbl,
                      isLoad ? "Has filament extruded from nozzle?\n(Nozzle heated to 250°C)"
                             : "Has filament been pulled out?\n(Nozzle heated to 250°C)");
    lv_obj_set_style_text_align(bambudeck_feed_status_lbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(bambudeck_feed_status_lbl, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(bambudeck_feed_status_lbl, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *btnRow = lv_obj_create(card);
    lv_obj_set_width(btnRow, lv_pct(100));
    lv_obj_set_height(btnRow, 42);
    lv_obj_set_flex_flow(btnRow, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btnRow, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(btnRow, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(btnRow, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btnRow, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(btnRow, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(btnRow, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *actionBtn = lv_label_create(btnRow);
    lv_obj_set_flex_grow(actionBtn, 1);
    lv_label_set_text(actionBtn, isLoad ? (LV_SYMBOL_DOWN " Purge More") : (LV_SYMBOL_UP " Retract More"));
    lv_obj_add_flag(actionBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_align(actionBtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(actionBtn, lv_color_hex(0x1976D2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(actionBtn, lv_color_hex(0x1565C0), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(actionBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(actionBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(actionBtn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(actionBtn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(actionBtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(actionBtn, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(actionBtn, bambudeck_feed_on_purge_retract, LV_EVENT_CLICKED, NULL);

    lv_obj_t *retryBtn = lv_label_create(btnRow);
    lv_obj_set_flex_grow(retryBtn, 1);
    lv_label_set_text(retryBtn, LV_SYMBOL_REFRESH " Retry");
    lv_obj_add_flag(retryBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_align(retryBtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(retryBtn, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(retryBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(retryBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(retryBtn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(retryBtn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(retryBtn, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(retryBtn, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(retryBtn, bambudeck_feed_on_retry, LV_EVENT_CLICKED, NULL);

    lv_obj_t *doneBtn = lv_label_create(btnRow);
    lv_obj_set_flex_grow(doneBtn, 1);
    lv_label_set_text(doneBtn, LV_SYMBOL_OK " Done");
    lv_obj_add_flag(doneBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_align(doneBtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(doneBtn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(doneBtn, BAMBU_COLOR_GREEN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(doneBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(doneBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(doneBtn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(doneBtn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(doneBtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(doneBtn, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(doneBtn, bambudeck_feed_on_done, LV_EVENT_CLICKED, NULL);
}

void bambudeck_hide_feed_dialog(void)
{
    if (bambudeck_feed_modal != NULL)
    {
        lv_obj_del(bambudeck_feed_modal);
        bambudeck_feed_modal = NULL;
        bambudeck_feed_status_lbl = NULL;
    }
}

void bambudeck_feed_check_timeout(void)
{
    if (bambudeck_feed_modal != NULL && (millis() - bambudeck_feed_modal_open_time > 300000))
    {
        bambudeck_send_gcode("M104 S0\n");
        if (bambudeck_feed_status_lbl != NULL)
        {
            lv_label_set_text(bambudeck_feed_status_lbl, "Safety Timeout: Heater cooled down.\nTap Close or Retry.");
        }
    }
}

void onFilamentUnloadConfirm()
{
    lv_msg_send(BAMBUDECK_COMMAND_UNLOAD_FILAMENT, NULL);
    bambudeck_show_feed_dialog(false);
}

void onFilamentUnload(lv_event_t *e)
{
    ui_confirmPanel_show(LV_SYMBOL_CUT " Unload Filament?\n\nNozzle will heat to 250°C\nand cut filament.", onFilamentUnloadConfirm);
}

void onFilamentLoadConfirm()
{
    lv_msg_send(BAMBUDECK_COMMAND_LOAD_FILAMENT, NULL);
    bambudeck_show_feed_dialog(true);
}

void onFilamentLoad(lv_event_t *e)
{
    ui_confirmPanel_show(LV_SYMBOL_PLAY " Load Filament?\n\nInsert filament into extruder.\nNozzle will heat to 250°C.", onFilamentLoadConfirm);
}
