#include "ui.h"
#include "../../src/bambudeck/types.h"

extern void bambudeck_send_gcode(const char *cmd);
extern void bambudeck_cooldown_nozzle(void);
extern void bambudeck_start_preheat_nozzle(int temp);
extern void bambudeck_feed_cut_and_retract(void);
extern void bambudeck_feed_move_to_purge(void);
extern void bambudeck_feed_purge_more(void);
extern void bambudeck_feed_retract_more(void);
extern void bambudeck_feed_wipe_and_shake(void);
extern void bambudeck_feed_done_notify(void);
extern void bambudeck_feed_retry_notify(void);

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

void onSettingsRecalibrateTouchConfirm()
{
    lv_msg_send(BAMBUDECK_SETTINGS_RESET_TOUCH, NULL);
}

void onSettingsRecalibrateTouch(lv_event_t *e)
{
    ui_confirmPanel_show(LV_SYMBOL_REFRESH " Recalibrate Touch?\nReboots into calibration", onSettingsRecalibrateTouchConfirm);
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
static int bambudeck_feed_last_temp = -1;
static bool bambudeck_feed_was_heated = false;

static void bambudeck_feed_on_close(lv_event_t *e)
{
    bambudeck_cooldown_nozzle();
    bambudeck_hide_feed_dialog();
}

static void bambudeck_feed_on_purge_retract(lv_event_t *e)
{
    bambudeck_feed_modal_open_time = millis();
    if (bambudeck_feed_is_load)
    {
        bambudeck_feed_purge_more();
        if (bambudeck_feed_status_lbl != NULL)
        {
            char buf[128];
            snprintf(buf, sizeof(buf), "Purging +25mm (%d°C)...\nHas filament extruded?", (int)bambuStatus.nozzle_temper);
            lv_label_set_text(bambudeck_feed_status_lbl, buf);
        }
    }
    else
    {
        bambudeck_feed_retract_more();
        if (bambudeck_feed_status_lbl != NULL)
        {
            char buf[128];
            snprintf(buf, sizeof(buf), "Retracting -25mm (%d°C)...\nHas filament pulled out?", (int)bambuStatus.nozzle_temper);
            lv_label_set_text(bambudeck_feed_status_lbl, buf);
        }
    }
}

static void bambudeck_feed_on_retry(lv_event_t *e)
{
    bambudeck_feed_modal_open_time = millis();
    bambudeck_feed_was_heated = false;
    bambudeck_feed_last_temp = -1;
    bambudeck_start_preheat_nozzle(250);
    bambudeck_feed_retry_notify();
    if (bambudeck_feed_is_load)
    {
        lv_msg_send(BAMBUDECK_COMMAND_LOAD_FILAMENT, NULL);
        if (bambudeck_feed_status_lbl != NULL)
        {
            char buf[128];
            snprintf(buf, sizeof(buf), "Re-heating nozzle: %d°C / 250°C\nPlease wait...", (int)bambuStatus.nozzle_temper);
            lv_label_set_text(bambudeck_feed_status_lbl, buf);
        }
    }
    else
    {
        lv_msg_send(BAMBUDECK_COMMAND_UNLOAD_FILAMENT, NULL);
        if (bambudeck_feed_status_lbl != NULL)
        {
            char buf[128];
            snprintf(buf, sizeof(buf), "Re-heating nozzle: %d°C / 250°C\nPlease wait...", (int)bambuStatus.nozzle_temper);
            lv_label_set_text(bambudeck_feed_status_lbl, buf);
        }
    }
}

static void bambudeck_feed_on_done(lv_event_t *e)
{
    if (bambudeck_feed_is_load)
    {
        bambudeck_feed_wipe_and_shake();
    }
    bambudeck_feed_done_notify();
    bambudeck_cooldown_nozzle();
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
    if (isLoad && strlen(bambudeck_chosen_target_label) > 0)
    {
        char titleBuf[64];
        snprintf(titleBuf, sizeof(titleBuf), LV_SYMBOL_PLAY " %s", bambudeck_chosen_target_label);
        lv_label_set_text(title, titleBuf);
    }
    else
    {
        lv_label_set_text(title, isLoad ? (LV_SYMBOL_PLAY " Filament Load") : (LV_SYMBOL_REFRESH " Filament Unload"));
    }
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
    char initBuf[128];
    if (bambuStatus.nozzle_temper < 235)
    {
        snprintf(initBuf, sizeof(initBuf), "Heating nozzle: %d°C / 250°C\nPlease wait for nozzle to heat up...", (int)bambuStatus.nozzle_temper);
    }
    else
    {
        if (isLoad)
        {
            if (bambudeck_chosen_target_is_ams)
                snprintf(initBuf, sizeof(initBuf), "Nozzle heated (%d°C)!\nAMS loading %s...\nHas filament extruded?", (int)bambuStatus.nozzle_temper, bambudeck_chosen_target_label);
            else
                snprintf(initBuf, sizeof(initBuf), "Nozzle heated (%d°C)!\nInsert filament into extruder.\nHas filament extruded?", (int)bambuStatus.nozzle_temper);
        }
        else
        {
            snprintf(initBuf, sizeof(initBuf), "Nozzle heated (%d°C)!\nCutting & retracting filament...\nHas filament pulled out?", (int)bambuStatus.nozzle_temper);
        }
    }
    lv_label_set_text(bambudeck_feed_status_lbl, initBuf);
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
        bambudeck_feed_was_heated = false;
        bambudeck_feed_last_temp = -1;
    }
}

void bambudeck_feed_check_timeout(void)
{
    if (bambudeck_feed_modal != NULL)
    {
        if (millis() - bambudeck_feed_modal_open_time > 300000)
        {
            bambudeck_cooldown_nozzle();
            if (bambudeck_feed_status_lbl != NULL)
            {
                lv_label_set_text(bambudeck_feed_status_lbl, "Safety Timeout: Heater cooled down.\nTap Close or Retry.");
            }
            return;
        }

        if (bambudeck_feed_status_lbl != NULL)
        {
            int currentTemp = (int)bambuStatus.nozzle_temper;
            if (currentTemp < 235)
            {
                if (currentTemp != bambudeck_feed_last_temp || bambudeck_feed_was_heated)
                {
                    bambudeck_feed_last_temp = currentTemp;
                    bambudeck_feed_was_heated = false;
                    char buf[128];
                    snprintf(buf, sizeof(buf), "Heating nozzle: %d°C / 250°C\nPlease wait for nozzle to heat up...", currentTemp);
                    lv_label_set_text(bambudeck_feed_status_lbl, buf);
                }
            }
            else
            {
                if (!bambudeck_feed_was_heated)
                {
                    bambudeck_feed_was_heated = true;
                    bambudeck_feed_last_temp = currentTemp;
                    char buf[128];
                    if (bambudeck_feed_is_load)
                    {
                        if (bambudeck_chosen_target_is_ams)
                        {
                            snprintf(buf, sizeof(buf), "Nozzle heated (%d°C)!\nAMS loading %s...\nHas filament extruded?", currentTemp, bambudeck_chosen_target_label);
                            bambudeck_feed_move_to_purge();
                        }
                        else
                        {
                            snprintf(buf, sizeof(buf), "Nozzle heated (%d°C)!\nInsert filament into extruder.\nHas filament extruded?", currentTemp);
                            bambudeck_feed_move_to_purge();
                        }
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "Nozzle heated (%d°C)!\nCutting & retracting filament...\nHas filament pulled out?", currentTemp);
                        bambudeck_feed_cut_and_retract();
                    }
                    lv_label_set_text(bambudeck_feed_status_lbl, buf);
                }
            }
        }
    }
}

struct BambuDeckFilamentSources bambudeck_filament_sources = {0};
char bambudeck_chosen_target_entity_id[64] = "";
bool bambudeck_chosen_target_is_ams = false;
char bambudeck_chosen_target_label[48] = "";
bool bambudeck_is_ams_active = false;
bool bambudeck_is_ext_spool_active = false;

void bambudeck_set_chosen_target(const char *entity_id, bool is_ams, const char *label)
{
    if (entity_id)
        strncpy(bambudeck_chosen_target_entity_id, entity_id, sizeof(bambudeck_chosen_target_entity_id) - 1);
    else
        bambudeck_chosen_target_entity_id[0] = '\0';

    bambudeck_chosen_target_is_ams = is_ams;

    if (label)
        strncpy(bambudeck_chosen_target_label, label, sizeof(bambudeck_chosen_target_label) - 1);
    else
        bambudeck_chosen_target_label[0] = '\0';
}

static lv_obj_t *bambudeck_load_selector_modal = NULL;

void bambudeck_hide_load_selector(void)
{
    if (bambudeck_load_selector_modal != NULL)
    {
        lv_obj_del(bambudeck_load_selector_modal);
        bambudeck_load_selector_modal = NULL;
    }
}

static void bambudeck_on_load_selector_cancel(lv_event_t *e)
{
    (void)e;
    bambudeck_hide_load_selector();
}

void onFilamentLoadConfirm(void);
void onFilamentUnloadConfirm(void);

static void bambudeck_on_trigger_unload_ext(void)
{
    bambudeck_is_ams_active = false;
    onFilamentUnloadConfirm();
}

static void bambudeck_on_trigger_unload_ams(void)
{
    bambudeck_is_ams_active = true;
    onFilamentUnloadConfirm();
}

static void bambudeck_on_select_ext_spool(lv_event_t *e)
{
    (void)e;
    if (bambudeck_is_ams_active)
    {
        bambudeck_hide_load_selector();
        ui_confirmPanel_show_custom(
            LV_SYMBOL_WARNING " AMS Loaded\n\nAMS filament is in extruder.\nPlease unload it first.",
            "Unload",
            "Cancel",
            bambudeck_on_trigger_unload_ams);
        return;
    }

    char entityId[64] = {0};
    if (strlen(bambudeck_filament_sources.ext_entity_id) > 0)
        strncpy(entityId, bambudeck_filament_sources.ext_entity_id, sizeof(entityId) - 1);
    else
        snprintf(entityId, sizeof(entityId), "sensor.%s_externalspool_external_spool", bambuDeckConfig.printerName);

    char label[48];
    if (strlen(bambudeck_filament_sources.ext_type) > 0)
        snprintf(label, sizeof(label), "External Spool (%s)", bambudeck_filament_sources.ext_type);
    else
        strncpy(label, "External Spool", sizeof(label) - 1);

    bambudeck_set_chosen_target(entityId, false, label);
    bambudeck_hide_load_selector();

    char confirmMsg[128];
    snprintf(confirmMsg, sizeof(confirmMsg), LV_SYMBOL_PLAY " Load %s?\n\nInsert filament into extruder.\nNozzle will heat to 250°C.", label);
    ui_confirmPanel_show(confirmMsg, onFilamentLoadConfirm);
}

static void bambudeck_on_select_ams_slot(lv_event_t *e)
{
    if (bambudeck_is_ext_spool_active)
    {
        bambudeck_hide_load_selector();
        ui_confirmPanel_show_custom(
            LV_SYMBOL_WARNING " External Spool Loaded\n\nExternal filament is in extruder.\nPlease unload it first.",
            "Unload",
            "Cancel",
            bambudeck_on_trigger_unload_ext);
        return;
    }

    uintptr_t code = (uintptr_t)lv_event_get_user_data(e);
    uint8_t a = code / 4;
    uint8_t s = code % 4;
    uint8_t amsNum = a + 1;
    uint8_t slotNum = s + 1;

    struct BambuDeckSlot *slot = &bambudeck_filament_sources.ams[a].slots[s];
    char entityId[64] = {0};
    if (strlen(slot->entity_id) > 0)
        strncpy(entityId, slot->entity_id, sizeof(entityId) - 1);
    else
        snprintf(entityId, sizeof(entityId), "sensor.%s_ams_%d_tray_%d", bambuDeckConfig.printerName, amsNum, slotNum);

    char label[48];
    if (strlen(slot->type) > 0)
        snprintf(label, sizeof(label), "AMS %d Slot %d (%s)", amsNum, slotNum, slot->type);
    else
        snprintf(label, sizeof(label), "AMS %d Slot %d", amsNum, slotNum);

    bambudeck_set_chosen_target(entityId, true, label);
    bambudeck_hide_load_selector();

    char confirmMsg[128];
    snprintf(confirmMsg, sizeof(confirmMsg), LV_SYMBOL_PLAY " Load %s?\n\nNozzle will heat to 250°C.", label);
    ui_confirmPanel_show(confirmMsg, onFilamentLoadConfirm);
}

void bambudeck_show_load_selector(void)
{
    if (bambudeck_load_selector_modal != NULL)
    {
        lv_obj_del(bambudeck_load_selector_modal);
        bambudeck_load_selector_modal = NULL;
    }

    bambudeck_load_selector_modal = lv_obj_create(lv_layer_top());
    lv_obj_set_size(bambudeck_load_selector_modal, 320, 240);
    lv_obj_set_pos(bambudeck_load_selector_modal, 0, 0);
    lv_obj_set_flex_flow(bambudeck_load_selector_modal, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bambudeck_load_selector_modal, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(bambudeck_load_selector_modal, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(bambudeck_load_selector_modal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bambudeck_load_selector_modal, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bambudeck_load_selector_modal, 210, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bambudeck_load_selector_modal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *card = lv_obj_create(bambudeck_load_selector_modal);
    lv_obj_set_width(card, 304);
    lv_obj_set_height(card, 226);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(card, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(card, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(card, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(card, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(card, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(card, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Header
    lv_obj_t *header = lv_obj_create(card);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_height(header, 26);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, LV_SYMBOL_DOWNLOAD " Load Filament");
    lv_obj_set_style_text_color(title, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cancelBtn = lv_label_create(header);
    lv_label_set_text(cancelBtn, LV_SYMBOL_CLOSE " Cancel");
    lv_obj_add_flag(cancelBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(cancelBtn, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cancelBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(cancelBtn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cancelBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cancelBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cancelBtn, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cancelBtn, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cancelBtn, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cancelBtn, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(cancelBtn, bambudeck_on_load_selector_cancel, LV_EVENT_CLICKED, NULL);

    // Scrollable content area
    lv_obj_t *content = lv_obj_create(card);
    lv_obj_set_width(content, lv_pct(100));
    lv_obj_set_flex_grow(content, 1);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(content, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(content, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(content, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(content, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(content, LV_OBJ_FLAG_SCROLLABLE);

    // 1. External Spool Button
    lv_obj_t *extBtn = lv_obj_create(content);
    lv_obj_set_width(extBtn, lv_pct(100));
    lv_obj_set_height(extBtn, 36);
    lv_obj_set_flex_flow(extBtn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(extBtn, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(extBtn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(extBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(extBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(extBtn, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(extBtn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(extBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(extBtn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(extBtn, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(extBtn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(extBtn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(extBtn, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(extBtn, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(extBtn, bambudeck_on_select_ext_spool, LV_EVENT_CLICKED, NULL);

    lv_obj_t *extLabel = lv_label_create(extBtn);
    lv_label_set_text(extLabel, LV_SYMBOL_REFRESH " External Spool");
    lv_obj_set_style_text_font(extLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(extLabel, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(extLabel, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *extRight = lv_obj_create(extBtn);
    lv_obj_set_flex_flow(extRight, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(extRight, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(extRight, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(extRight, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(extRight, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(extRight, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(extRight, LV_OBJ_FLAG_CLICKABLE);

    if (strlen(bambudeck_filament_sources.ext_type) > 0)
    {
        lv_obj_t *extTypeLbl = lv_label_create(extRight);
        lv_label_set_text(extTypeLbl, bambudeck_filament_sources.ext_type);
        lv_obj_set_style_text_font(extTypeLbl, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(extTypeLbl, BAMBU_COLOR_TEXT_MUTED(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_clear_flag(extTypeLbl, LV_OBJ_FLAG_CLICKABLE);
    }

    lv_obj_t *extSwatch = lv_obj_create(extRight);
    lv_obj_set_size(extSwatch, 16, 16);
    lv_obj_set_style_radius(extSwatch, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    uint32_t extC = (bambudeck_filament_sources.ext_color != 0) ? bambudeck_filament_sources.ext_color : 0xFFFFFF;
    lv_obj_set_style_bg_color(extSwatch, lv_color_hex(extC), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(extSwatch, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(extSwatch, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(extSwatch, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(extSwatch, LV_OBJ_FLAG_CLICKABLE);

    // 2. AMS Units & Slots
    int amsCount = bambudeck_filament_sources.ams_count;
    if (amsCount == 0 && (bambuStatus.ams || strlen(bambuDeckConfig.printerName) > 0))
        amsCount = 1;

    for (int a = 0; a < amsCount && a < 4; a++)
    {
        lv_obj_t *amsHeader = lv_label_create(content);
        char amsTitle[32];
        snprintf(amsTitle, sizeof(amsTitle), "AMS %d", a + 1);
        lv_label_set_text(amsHeader, amsTitle);
        lv_obj_set_style_text_font(amsHeader, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(amsHeader, BAMBU_COLOR_TEXT_MUTED(), LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *row = lv_obj_create(content);
        lv_obj_set_width(row, lv_pct(100));
        lv_obj_set_height(row, 50);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_opa(row, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(row, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(row, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_column(row, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

        for (int s = 0; s < 4; s++)
        {
            struct BambuDeckSlot *slot = &bambudeck_filament_sources.ams[a].slots[s];

            lv_obj_t *slotBtn = lv_obj_create(row);
            lv_obj_set_width(slotBtn, lv_pct(23));
            lv_obj_set_height(slotBtn, 48);
            lv_obj_set_flex_flow(slotBtn, LV_FLEX_FLOW_COLUMN);
            lv_obj_set_flex_align(slotBtn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
            lv_obj_clear_flag(slotBtn, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_add_flag(slotBtn, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_radius(slotBtn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(slotBtn, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(slotBtn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_opa(slotBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(slotBtn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(slotBtn, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_all(slotBtn, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(slotBtn, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

            // Color Swatch
            lv_obj_t *swatch = lv_obj_create(slotBtn);
            lv_obj_set_size(swatch, 24, 8);
            lv_obj_set_style_radius(swatch, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            uint32_t c = (slot->color != 0) ? slot->color : 0x555555;
            lv_obj_set_style_bg_color(swatch, lv_color_hex(c), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(swatch, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(swatch, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(swatch, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_clear_flag(swatch, LV_OBJ_FLAG_CLICKABLE);

            // Text Label
            lv_obj_t *lbl = lv_label_create(slotBtn);
            char slotText[16];
            if (strlen(slot->type) > 0)
                snprintf(slotText, sizeof(slotText), "%d:%s", s + 1, slot->type);
            else
                snprintf(slotText, sizeof(slotText), "Slot %d", s + 1);
            lv_label_set_text(lbl, slotText);
            lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(lbl, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_clear_flag(lbl, LV_OBJ_FLAG_CLICKABLE);

            uintptr_t userData = (uintptr_t)(a * 4 + s);
            lv_obj_add_event_cb(slotBtn, bambudeck_on_select_ams_slot, LV_EVENT_CLICKED, (void *)userData);
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
    (void)e;
    bambudeck_show_load_selector();
}
