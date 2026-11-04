#include <stdio.h>
#include "../ui.h"
#include "../ui_msgs.h"

extern void bambudeck_open_printer_selector(void);
extern void bambudeck_thumbnail_invalidate(void);

static void ui_event_comp_settingsComponent_selectPrinterButton(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        bambudeck_open_printer_selector();
    }
}

static void bambudeck_reload_settings_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    bambuDeckConfig.currentScreenIndex = -1;
    loadScreen(4);
}

static void ui_event_comp_settingsComponent_onLightMode(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED)
    {
        bambuDeckConfig.lightMode = !bambuDeckConfig.lightMode;
        lv_msg_send(BAMBUDECK_SETTINGS_SAVE, NULL);
        bambudeck_thumbnail_invalidate();
        lv_timer_t *t = lv_timer_create(bambudeck_reload_settings_timer_cb, 30, NULL);
        lv_timer_set_repeat_count(t, 1);
    }
}


void ui_event_comp_settingsComponent_checkUpdateButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        lv_msg_send(BAMBUDECK_SETTINGS_CHECK_UPDATE, NULL);
    }
}

void ui_event_comp_settingsComponent_reconfigWifiConfirm()
{
    lv_msg_send(BAMBUDECK_SETTINGS_RECONFIG_WIFI, NULL);
}

void ui_event_comp_settingsComponent_reconfigWifiButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        ui_confirmPanel_show(LV_SYMBOL_WIFI " Open Setup Portal?", ui_event_comp_settingsComponent_reconfigWifiConfirm);
    }
}

void ui_event_comp_settingsComponent_resetDeviceButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onSettingsResetDevice(e);
    }
}

void ui_event_comp_settingsComponent_onBackLight(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        lv_msg_send(BAMBUDECK_SETTINGS_BACKLIGHT, NULL);
    }
    if (event_code == LV_EVENT_RELEASED)
    {
        lv_msg_send(BAMBUDECK_SETTINGS_BACKLIGHT_SET, NULL);
    }
}

void ui_event_comp_settingsComponent_tftInvertInput(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        lv_msg_send(BAMBUDECK_SETTINGS_TFT_INVERT, NULL);
    }
}

void ui_event_comp_settingsComponent_onTFTOFF(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        uint32_t value = lv_slider_get_value(ui_settingsTFTOFFSlider);
        if (value < BAMBUDECK_LCD_MIN_SLEEP_TIME)
        {
            lv_label_set_text(ui_settingsTFTOFFValue, LV_SYMBOL_POWER);
        }
        else
        {
            lv_label_set_text_fmt(ui_settingsTFTOFFValue, "%dm", value);
        }
    }
    if (event_code == LV_EVENT_RELEASED)
    {
        lv_msg_send(BAMBUDECK_SETTINGS_TFTOFF_SET, NULL);
    }
}

void ui_event_comp_settingsComponent_onWOP(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        onSettingsWOP(e);
    }
}

void ui_event_comp_settingsComponent_onTFTFlip(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        onSettingsTFTFlip(e);
    }
}

void ui_event_comp_settingsComponent_onOTA(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        onSettingsOTA(e);
    }
}

void ui_event_comp_settingsComponent_onAuxFan(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        onSettingsAuxFan(e);
    }
}

void ui_event_comp_settingsComponent_onChamberFan(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        onSettingsChamberFan(e);
    }
}

void ui_event_comp_settingsComponent_onChamberTemp(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        onSettingsChamberSensor(e);
    }
}

static void bambu_style_settings_card(lv_obj_t *card)
{
    lv_obj_set_style_radius(card, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(card, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(card, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(card, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(card, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void bambu_style_settings_switch(lv_obj_t *sw)
{
    lv_obj_set_width(sw, 48);
    lv_obj_set_height(sw, 24);
    lv_obj_set_style_bg_color(sw, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(sw, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sw, BAMBU_COLOR_GREEN(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(sw, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(sw, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(sw, 255, LV_PART_KNOB | LV_STATE_CHECKED);
}

// COMPONENT settingsComponent

lv_obj_t *ui_settingsComponent_create(lv_obj_t *comp_parent)
{
    (void)comp_parent;
    lv_obj_t *cui_settingsComponent;
    cui_settingsComponent = lv_obj_create(ui_settingsScreen);
    lv_obj_set_height(cui_settingsComponent, lv_pct(100));
    lv_obj_set_flex_grow(cui_settingsComponent, 1);
    lv_obj_set_flex_flow(cui_settingsComponent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_settingsComponent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_radius(cui_settingsComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_settingsComponent, BAMBU_COLOR_BG_SCREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_settingsComponent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_settingsComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_settingsComponent, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settingsComponent, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_settingsComponent, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_settingsComponent, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_settingsComponent, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cui_settingsComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_settingsComponent, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_settingsComponent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_scrollbar_mode(cui_settingsComponent, LV_SCROLLBAR_MODE_ACTIVE);

    lv_obj_set_style_bg_color(cui_settingsComponent, BAMBU_COLOR_GREEN(), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_settingsComponent, 255, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    // Header Banner
    lv_obj_t *cui_settingsTitle;
    cui_settingsTitle = lv_label_create(cui_settingsComponent);
    lv_obj_set_width(cui_settingsTitle, lv_pct(100));
    lv_obj_set_height(cui_settingsTitle, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(cui_settingsTitle, LV_SYMBOL_SETTINGS " BAMBUDECK SETTINGS");
    lv_obj_set_scrollbar_mode(cui_settingsTitle, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_font(cui_settingsTitle, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_settingsTitle, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settingsTitle, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_settingsTitle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_settingsTitle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_settingsTitle, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_settingsTitle, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_settingsTitle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(cui_settingsTitle, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_settingsTitle, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Local IP Badge
    lv_obj_t *cui_ipInfoLabel = lv_label_create(cui_settingsComponent);
    lv_obj_set_width(cui_ipInfoLabel, lv_pct(100));
    lv_obj_set_height(cui_ipInfoLabel, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(cui_ipInfoLabel, LV_SYMBOL_WIFI " IP: %s",
                          bambuDeckConfig.localIP[0] != '\0' ? bambuDeckConfig.localIP : "N/A");
    lv_obj_clear_flag(cui_ipInfoLabel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cui_ipInfoLabel, LV_SCROLLBAR_MODE_OFF);
    bambu_style_settings_card(cui_ipInfoLabel);
    lv_obj_set_style_pad_left(cui_ipInfoLabel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_ipInfoLabel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_ipInfoLabel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_ipInfoLabel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_ipInfoLabel, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Select Printer Action Button
    lv_obj_t *cui_selectPrinterBtn = lv_label_create(cui_settingsComponent);
    lv_obj_set_width(cui_selectPrinterBtn, lv_pct(100));
    lv_obj_set_height(cui_selectPrinterBtn, LV_SIZE_CONTENT);
    if (bambuDeckConfig.printerName[0] != '\0')
        lv_label_set_text_fmt(cui_selectPrinterBtn, LV_SYMBOL_LIST " Printer: %s", bambuDeckConfig.printerName);
    else
        lv_label_set_text(cui_selectPrinterBtn, LV_SYMBOL_LIST " Select Printer");
    lv_obj_add_flag(cui_selectPrinterBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cui_selectPrinterBtn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cui_selectPrinterBtn, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_selectPrinterBtn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_selectPrinterBtn, BAMBU_COLOR_BG_GREEN_TINT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_selectPrinterBtn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cui_selectPrinterBtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_selectPrinterBtn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_selectPrinterBtn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_selectPrinterBtn, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_selectPrinterBtn, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_selectPrinterBtn, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_selectPrinterBtn, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_selectPrinterBtn, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(cui_selectPrinterBtn, ui_event_comp_settingsComponent_selectPrinterButton, LV_EVENT_CLICKED, NULL);

    // Display / Theme Section Header
    lv_obj_t *cui_screenTitle;
    cui_screenTitle = lv_label_create(cui_settingsComponent);
    lv_obj_set_width(cui_screenTitle, lv_pct(100));
    lv_obj_set_height(cui_screenTitle, LV_SIZE_CONTENT);
    lv_label_set_text(cui_screenTitle, LV_SYMBOL_IMAGE " DISPLAY & THEME");
    lv_obj_set_scrollbar_mode(cui_screenTitle, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_font(cui_screenTitle, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_screenTitle, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_screenTitle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_screenTitle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_screenTitle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_screenTitle, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_screenTitle, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_screenTitle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(cui_screenTitle, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_screenTitle, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Light / Dark Mode Switch Row
    lv_obj_t *cui_screenLightModeSetting = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_screenLightModeSetting, lv_pct(100));
    lv_obj_set_height(cui_screenLightModeSetting, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_screenLightModeSetting, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_screenLightModeSetting, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(cui_screenLightModeSetting, LV_SCROLLBAR_MODE_OFF);
    bambu_style_settings_card(cui_screenLightModeSetting);
    lv_obj_set_style_pad_left(cui_screenLightModeSetting, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_screenLightModeSetting, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_screenLightModeSetting, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_screenLightModeSetting, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_screenLightModeLabel = lv_label_create(cui_screenLightModeSetting);
    lv_label_set_text(cui_screenLightModeLabel, LV_SYMBOL_EYE_OPEN " Light Mode");
    lv_obj_set_scrollbar_mode(cui_screenLightModeLabel, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *cui_settingsLightModeSwitch = lv_switch_create(cui_screenLightModeSetting);
    bambu_style_settings_switch(cui_settingsLightModeSwitch);
    if (bambuDeckConfig.lightMode)
    {
        lv_obj_add_state(cui_settingsLightModeSwitch, LV_STATE_CHECKED);
    }
    lv_obj_add_event_cb(cui_settingsLightModeSwitch, ui_event_comp_settingsComponent_onLightMode, LV_EVENT_VALUE_CHANGED, NULL);

    // Backlight Slider Row
    lv_obj_t *cui_settingsBackLightPanel;
    cui_settingsBackLightPanel = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_settingsBackLightPanel, lv_pct(100));
    lv_obj_set_height(cui_settingsBackLightPanel, 60);
    lv_obj_set_flex_flow(cui_settingsBackLightPanel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_settingsBackLightPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    bambu_style_settings_card(cui_settingsBackLightPanel);
    lv_obj_set_style_pad_left(cui_settingsBackLightPanel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settingsBackLightPanel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_settingsBackLightPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_settingsBackLightPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_settingsBackLightPanelLabel;
    cui_settingsBackLightPanelLabel = lv_label_create(cui_settingsBackLightPanel);
    lv_obj_set_width(cui_settingsBackLightPanelLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_settingsBackLightPanelLabel, LV_SIZE_CONTENT);
    lv_label_set_text(cui_settingsBackLightPanelLabel, "Bright");
    lv_obj_set_style_text_font(cui_settingsBackLightPanelLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settingsBackLightPanelLabel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsBackLightPanelSlider = lv_slider_create(cui_settingsBackLightPanel);
    lv_slider_set_range(ui_settingsBackLightPanelSlider, 10, 255);
    lv_slider_set_value(ui_settingsBackLightPanelSlider, bambuDeckConfig.backlightLevel, LV_ANIM_OFF);
    lv_obj_set_height(ui_settingsBackLightPanelSlider, 10);
    lv_obj_set_flex_grow(ui_settingsBackLightPanelSlider, 1);
    lv_obj_set_style_bg_color(ui_settingsBackLightPanelSlider, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingsBackLightPanelSlider, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_settingsBackLightPanelSlider, BAMBU_COLOR_GREEN(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingsBackLightPanelSlider, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_settingsBackLightPanelSlider, BAMBU_COLOR_GREEN(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingsBackLightPanelSlider, 255, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Sleep Timer Slider Row
    lv_obj_t *cui_settingsTFTOFFPanel;
    cui_settingsTFTOFFPanel = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_settingsTFTOFFPanel, lv_pct(100));
    lv_obj_set_height(cui_settingsTFTOFFPanel, 60);
    lv_obj_set_flex_flow(cui_settingsTFTOFFPanel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_settingsTFTOFFPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    bambu_style_settings_card(cui_settingsTFTOFFPanel);
    lv_obj_set_style_pad_left(cui_settingsTFTOFFPanel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settingsTFTOFFPanel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_settingsTFTOFFPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_settingsTFTOFFPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_settingsTFTOFFPanelLabel;
    cui_settingsTFTOFFPanelLabel = lv_label_create(cui_settingsTFTOFFPanel);
    lv_obj_set_width(cui_settingsTFTOFFPanelLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_settingsTFTOFFPanelLabel, LV_SIZE_CONTENT);
    lv_label_set_text(cui_settingsTFTOFFPanelLabel, "Sleep");
    lv_obj_set_style_text_font(cui_settingsTFTOFFPanelLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settingsTFTOFFPanelLabel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsTFTOFFSlider = lv_slider_create(cui_settingsTFTOFFPanel);
    lv_slider_set_range(ui_settingsTFTOFFSlider, 4, 60);
    lv_slider_set_value(ui_settingsTFTOFFSlider, bambuDeckConfig.tftOffValue, LV_ANIM_OFF);
    lv_obj_set_height(ui_settingsTFTOFFSlider, 10);
    lv_obj_set_flex_grow(ui_settingsTFTOFFSlider, 1);
    lv_obj_set_style_bg_color(ui_settingsTFTOFFSlider, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingsTFTOFFSlider, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_settingsTFTOFFSlider, BAMBU_COLOR_GREEN(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingsTFTOFFSlider, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_settingsTFTOFFSlider, BAMBU_COLOR_GREEN(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingsTFTOFFSlider, 255, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_settingsTFTOFFValue = lv_label_create(cui_settingsTFTOFFPanel);
    lv_obj_set_width(ui_settingsTFTOFFValue, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_settingsTFTOFFValue, LV_SIZE_CONTENT);
    int32_t value = lv_slider_get_value(ui_settingsTFTOFFSlider);
    lv_label_set_text_fmt(ui_settingsTFTOFFValue, value < BAMBUDECK_LCD_MIN_SLEEP_TIME ? LV_SYMBOL_POWER : "%dm", value);
    lv_obj_set_style_text_font(ui_settingsTFTOFFValue, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_settingsTFTOFFValue, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Wake on Print Row
    lv_obj_t *cui_screenWOPSetting;
    cui_screenWOPSetting = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_screenWOPSetting, lv_pct(100));
    lv_obj_set_height(cui_screenWOPSetting, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_screenWOPSetting, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_screenWOPSetting, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(cui_screenWOPSetting, LV_SCROLLBAR_MODE_OFF);
    bambu_style_settings_card(cui_screenWOPSetting);
    lv_obj_set_style_pad_left(cui_screenWOPSetting, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_screenWOPSetting, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_screenWOPSetting, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_screenWOPSetting, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_screenWOPLabel;
    cui_screenWOPLabel = lv_label_create(cui_screenWOPSetting);
    lv_label_set_text(cui_screenWOPLabel, LV_SYMBOL_BELL " Wake on Print");
    lv_obj_set_scrollbar_mode(cui_screenWOPLabel, LV_SCROLLBAR_MODE_OFF);

    ui_settingsWOPSwitch = lv_switch_create(cui_screenWOPSetting);
    bambu_style_settings_switch(ui_settingsWOPSwitch);
    if (bambuDeckConfig.wakeOnPrint)
    {
        lv_obj_add_state(ui_settingsWOPSwitch, LV_STATE_CHECKED);
    }

    // Invert LCD Hardware Colors Row
    lv_obj_t *cui_screenInvertSetting;
    cui_screenInvertSetting = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_screenInvertSetting, lv_pct(100));
    lv_obj_set_height(cui_screenInvertSetting, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_screenInvertSetting, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_screenInvertSetting, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(cui_screenInvertSetting, LV_SCROLLBAR_MODE_OFF);
    bambu_style_settings_card(cui_screenInvertSetting);
    lv_obj_set_style_pad_left(cui_screenInvertSetting, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_screenInvertSetting, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_screenInvertSetting, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_screenInvertSetting, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_screenInvertLabel;
    cui_screenInvertLabel = lv_label_create(cui_screenInvertSetting);
    lv_label_set_text(cui_screenInvertLabel, LV_SYMBOL_IMAGE " Invert LCD Panel");
    lv_obj_set_scrollbar_mode(cui_screenInvertLabel, LV_SCROLLBAR_MODE_OFF);

    ui_settingsTFTInvertSwitch = lv_switch_create(cui_screenInvertSetting);
    bambu_style_settings_switch(ui_settingsTFTInvertSwitch);
    if (bambuDeckConfig.tftInvert)
    {
        lv_obj_add_state(ui_settingsTFTInvertSwitch, LV_STATE_CHECKED);
    }

    // Flip Screen Row
    lv_obj_t *cui_settingsTFTFlip;
    cui_settingsTFTFlip = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_settingsTFTFlip, lv_pct(100));
    lv_obj_set_height(cui_settingsTFTFlip, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_settingsTFTFlip, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_settingsTFTFlip, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(cui_settingsTFTFlip, LV_SCROLLBAR_MODE_OFF);
    bambu_style_settings_card(cui_settingsTFTFlip);
    lv_obj_set_style_pad_left(cui_settingsTFTFlip, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settingsTFTFlip, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_settingsTFTFlip, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_settingsTFTFlip, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_settingsTFTFlipLabel;
    cui_settingsTFTFlipLabel = lv_label_create(cui_settingsTFTFlip);
    lv_label_set_text(cui_settingsTFTFlipLabel, LV_SYMBOL_SHUFFLE " Flip Screen 180°");
    lv_obj_set_scrollbar_mode(cui_settingsTFTFlipLabel, LV_SCROLLBAR_MODE_OFF);

    ui_settingsTFTFlipSwitch = lv_switch_create(cui_settingsTFTFlip);
    bambu_style_settings_switch(ui_settingsTFTFlipSwitch);
    if (bambuDeckConfig.tftFlip)
    {
        lv_obj_add_state(ui_settingsTFTFlipSwitch, LV_STATE_CHECKED);
    }

    // System Section Header
    lv_obj_t *cui_deviceTitle;
    cui_deviceTitle = lv_label_create(cui_settingsComponent);
    lv_obj_set_width(cui_deviceTitle, lv_pct(100));
    lv_obj_set_height(cui_deviceTitle, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(cui_deviceTitle, LV_SYMBOL_SETTINGS " SYSTEM (v%s)", BAMBUDECK_FIRMWARE_VERSION);
    lv_obj_set_scrollbar_mode(cui_deviceTitle, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_font(cui_deviceTitle, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_deviceTitle, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_deviceTitle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_deviceTitle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_deviceTitle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_deviceTitle, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_deviceTitle, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_deviceTitle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(cui_deviceTitle, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_deviceTitle, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Aux Fan Switch
    lv_obj_t *cui_settings_auxFan;
    cui_settings_auxFan = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_settings_auxFan, lv_pct(100));
    lv_obj_set_height(cui_settings_auxFan, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_settings_auxFan, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_settings_auxFan, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(cui_settings_auxFan, LV_SCROLLBAR_MODE_OFF);
    bambu_style_settings_card(cui_settings_auxFan);
    lv_obj_set_style_pad_left(cui_settings_auxFan, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settings_auxFan, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_settings_auxFan, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_settings_auxFan, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_settings_auxFanLabel;
    cui_settings_auxFanLabel = lv_label_create(cui_settings_auxFan);
    lv_label_set_text(cui_settings_auxFanLabel, "Aux Fan Control");
    lv_obj_set_scrollbar_mode(cui_settings_auxFanLabel, LV_SCROLLBAR_MODE_OFF);

    ui_settings_auxFanSwitch = lv_switch_create(cui_settings_auxFan);
    bambu_style_settings_switch(ui_settings_auxFanSwitch);

    if (!bambudeck_bblp_is_p1p() && !bambudeck_bblp_is_x1())
    {
        lv_obj_add_flag(cui_settings_auxFan, LV_OBJ_FLAG_HIDDEN);
    }
    else if (bambuDeckConfig.auxFanEnabled)
    {
        lv_obj_add_state(ui_settings_auxFanSwitch, LV_STATE_CHECKED);
    }

    // Chamber Fan Switch
    lv_obj_t *cui_settings_chamberFan;
    cui_settings_chamberFan = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_settings_chamberFan, lv_pct(100));
    lv_obj_set_height(cui_settings_chamberFan, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_settings_chamberFan, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_settings_chamberFan, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(cui_settings_chamberFan, LV_SCROLLBAR_MODE_OFF);
    bambu_style_settings_card(cui_settings_chamberFan);
    lv_obj_set_style_pad_left(cui_settings_chamberFan, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settings_chamberFan, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_settings_chamberFan, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_settings_chamberFan, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_settings_chamberFanLabel;
    cui_settings_chamberFanLabel = lv_label_create(cui_settings_chamberFan);
    lv_label_set_text(cui_settings_chamberFanLabel, "Chamber Fan Control");
    lv_obj_set_scrollbar_mode(cui_settings_chamberFanLabel, LV_SCROLLBAR_MODE_OFF);

    ui_settings_chamberFanSwitch = lv_switch_create(cui_settings_chamberFan);
    bambu_style_settings_switch(ui_settings_chamberFanSwitch);

    if (!bambudeck_bblp_is_p1p())
    {
        lv_obj_add_flag(cui_settings_chamberFan, LV_OBJ_FLAG_HIDDEN);
    }
    else if (bambuDeckConfig.chamberFanEnabled)
    {
        lv_obj_add_state(ui_settings_chamberFanSwitch, LV_STATE_CHECKED);
    }

    // Chamber Temp Sensor Switch
    lv_obj_t *cui_settings_chamberSensor;
    cui_settings_chamberSensor = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_settings_chamberSensor, lv_pct(100));
    lv_obj_set_height(cui_settings_chamberSensor, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_settings_chamberSensor, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_settings_chamberSensor, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(cui_settings_chamberSensor, LV_SCROLLBAR_MODE_OFF);
    bambu_style_settings_card(cui_settings_chamberSensor);
    lv_obj_set_style_pad_left(cui_settings_chamberSensor, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settings_chamberSensor, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_settings_chamberSensor, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_settings_chamberSensor, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_settings_chamberSensorLabel;
    cui_settings_chamberSensorLabel = lv_label_create(cui_settings_chamberSensor);
    lv_label_set_text(cui_settings_chamberSensorLabel, "Chamber Temp Sensor");
    lv_obj_set_scrollbar_mode(cui_settings_chamberSensorLabel, LV_SCROLLBAR_MODE_OFF);

    ui_settings_chamberSensorSwitch = lv_switch_create(cui_settings_chamberSensor);
    bambu_style_settings_switch(ui_settings_chamberSensorSwitch);

    if (!bambudeck_bblp_is_p1Series())
    {
        lv_obj_add_flag(cui_settings_chamberSensor, LV_OBJ_FLAG_HIDDEN);
    }
    else if (bambuDeckConfig.chamberSensorEnabled)
    {
        lv_obj_add_state(ui_settings_chamberSensorSwitch, LV_STATE_CHECKED);
    }

    // OTA Switch
    lv_obj_t *cui_settings_ota;
    cui_settings_ota = lv_obj_create(cui_settingsComponent);
    lv_obj_set_width(cui_settings_ota, lv_pct(100));
    lv_obj_set_height(cui_settings_ota, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_settings_ota, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_settings_ota, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(cui_settings_ota, LV_SCROLLBAR_MODE_OFF);
    bambu_style_settings_card(cui_settings_ota);
    lv_obj_set_style_pad_left(cui_settings_ota, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_settings_ota, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_settings_ota, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_settings_ota, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_settings_otaLabel;
    cui_settings_otaLabel = lv_label_create(cui_settings_ota);
    lv_label_set_text(cui_settings_otaLabel, LV_SYMBOL_DOWNLOAD " OTA Updates");
    lv_obj_set_scrollbar_mode(cui_settings_otaLabel, LV_SCROLLBAR_MODE_OFF);

    ui_settings_otaSwitch = lv_switch_create(cui_settings_ota);
    bambu_style_settings_switch(ui_settings_otaSwitch);

    if (bambuDeckConfig.otaEnabled)
    {
        lv_obj_add_state(ui_settings_otaSwitch, LV_STATE_CHECKED);
    }

    // Check for Updates Button
    lv_obj_t *cui_checkUpdateButton;
    cui_checkUpdateButton = lv_label_create(cui_settingsComponent);
    lv_obj_set_width(cui_checkUpdateButton, lv_pct(100));
    lv_obj_set_height(cui_checkUpdateButton, LV_SIZE_CONTENT);
    lv_label_set_text(cui_checkUpdateButton, LV_SYMBOL_REFRESH " Check for Updates");
    lv_obj_add_flag(cui_checkUpdateButton, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cui_checkUpdateButton, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cui_checkUpdateButton, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_checkUpdateButton, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_checkUpdateButton, lv_color_hex(0x2C2C2C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_checkUpdateButton, lv_color_hex(0x444444), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cui_checkUpdateButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_checkUpdateButton, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_checkUpdateButton, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_checkUpdateButton, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_checkUpdateButton, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_checkUpdateButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(cui_checkUpdateButton, ui_event_comp_settingsComponent_checkUpdateButton, LV_EVENT_CLICKED, NULL);

    // WiFi / HA Setup Portal Button
    lv_obj_t *cui_reconfigWifiButton;
    cui_reconfigWifiButton = lv_label_create(cui_settingsComponent);
    lv_obj_set_width(cui_reconfigWifiButton, lv_pct(100));
    lv_obj_set_height(cui_reconfigWifiButton, LV_SIZE_CONTENT);
    lv_label_set_text(cui_reconfigWifiButton, LV_SYMBOL_WIFI " WiFi / HA Setup Portal");
    lv_obj_add_flag(cui_reconfigWifiButton, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cui_reconfigWifiButton, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cui_reconfigWifiButton, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_reconfigWifiButton, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_reconfigWifiButton, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_reconfigWifiButton, BAMBU_COLOR_GREEN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cui_reconfigWifiButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_reconfigWifiButton, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_reconfigWifiButton, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_reconfigWifiButton, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_reconfigWifiButton, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_reconfigWifiButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(cui_reconfigWifiButton, ui_event_comp_settingsComponent_reconfigWifiButton, LV_EVENT_CLICKED, NULL);

    // Recalibrate Touch Button
    lv_obj_t *cui_recalibrateTouchButton;
    cui_recalibrateTouchButton = lv_label_create(cui_settingsComponent);
    lv_obj_set_width(cui_recalibrateTouchButton, lv_pct(100));
    lv_obj_set_height(cui_recalibrateTouchButton, LV_SIZE_CONTENT);
    lv_label_set_text(cui_recalibrateTouchButton, LV_SYMBOL_REFRESH " Recalibrate Touch Screen");
    lv_obj_add_flag(cui_recalibrateTouchButton, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cui_recalibrateTouchButton, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cui_recalibrateTouchButton, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_recalibrateTouchButton, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_recalibrateTouchButton, lv_color_hex(0x2C2C2C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_recalibrateTouchButton, lv_color_hex(0x444444), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cui_recalibrateTouchButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_recalibrateTouchButton, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_recalibrateTouchButton, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_recalibrateTouchButton, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_recalibrateTouchButton, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_recalibrateTouchButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(cui_recalibrateTouchButton, onSettingsRecalibrateTouch, LV_EVENT_CLICKED, NULL);

    // Reboot Device Button
    lv_obj_t *cui_reseDeviceButton;
    cui_reseDeviceButton = lv_label_create(cui_settingsComponent);
    lv_obj_set_width(cui_reseDeviceButton, lv_pct(100));
    lv_obj_set_height(cui_reseDeviceButton, LV_SIZE_CONTENT);
    lv_label_set_text(cui_reseDeviceButton, LV_SYMBOL_POWER " Reboot BambuDeck");
    lv_obj_add_flag(cui_reseDeviceButton, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cui_reseDeviceButton, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cui_reseDeviceButton, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_reseDeviceButton, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_reseDeviceButton, BAMBU_COLOR_ORANGE(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_reseDeviceButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_reseDeviceButton, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_reseDeviceButton, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_reseDeviceButton, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_reseDeviceButton, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_reseDeviceButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t **children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_SETTINGSCOMPONENT_NUM);
    children[UI_COMP_SETTINGSCOMPONENT_SETTINGSCOMPONENT] = cui_settingsComponent;
    children[UI_COMP_SETTINGSCOMPONENT_SCREEN_TITLE] = cui_screenTitle;
    children[UI_COMP_SETTINGSCOMPONENT_BACKLIGHT] = cui_settingsBackLightPanel;
    children[UI_COMP_SETTINGSCOMPONENT_BACKLIGHT_LABEL] = cui_settingsBackLightPanelLabel;
    children[UI_COMP_SETTINGSCOMPONENT_BACKLIGHT_SLIDER] = ui_settingsBackLightPanelSlider;
    children[UI_COMP_SETTINGSCOMPONENT_TFTOFF] = cui_settingsTFTOFFPanel;
    children[UI_COMP_SETTINGSCOMPONENT_TFTOFF_LABEL] = cui_settingsTFTOFFPanelLabel;
    children[UI_COMP_SETTINGSCOMPONENT_TFTOFF_SLIDER] = ui_settingsTFTOFFSlider;
    children[UI_COMP_SETTINGSCOMPONENT_TFTOFF_VALUE] = ui_settingsTFTOFFValue;
    children[UI_COMP_SETTINGSCOMPONENT_TFT_WOP] = cui_screenWOPSetting;
    children[UI_COMP_SETTINGSCOMPONENT_TFT_WOP_LABEL] = cui_screenWOPLabel;
    children[UI_COMP_SETTINGSCOMPONENT_TFT_WOP_INPUT] = ui_settingsWOPSwitch;
    children[UI_COMP_SETTINGSCOMPONENT_TFT_INVERT] = cui_screenInvertSetting;
    children[UI_COMP_SETTINGSCOMPONENT_TFT_INVERT_LABEL] = cui_screenInvertLabel;
    children[UI_COMP_SETTINGSCOMPONENT_TFT_INVERT_INPUT] = ui_settingsTFTInvertSwitch;
    children[UI_COMP_SETTINGSCOMPONENT_TFT_FLIP] = cui_settingsTFTFlip;
    children[UI_COMP_SETTINGSCOMPONENT_TFT_FLIP_LABEL] = cui_settingsTFTFlipLabel;
    children[UI_COMP_SETTINGSCOMPONENT_TFT_FLIP_INPUT] = ui_settingsTFTFlipSwitch;
    children[UI_COMP_SETTINGSCOMPONENT_DEVICE_TITLE] = cui_deviceTitle;
    children[UI_COMP_SETTINGSCOMPONENT_AUXFAN] = cui_settings_auxFan;
    children[UI_COMP_SETTINGSCOMPONENT_AUXFAN_LABEL] = cui_settings_auxFanLabel;
    children[UI_COMP_SETTINGSCOMPONENT_AUXFAN_SWITCH] = ui_settings_auxFanSwitch;
    children[UI_COMP_SETTINGSCOMPONENT_CHAMBERFAN] = cui_settings_chamberFan;
    children[UI_COMP_SETTINGSCOMPONENT_CHAMBERFAN_LABEL] = cui_settings_chamberFanLabel;
    children[UI_COMP_SETTINGSCOMPONENT_CHAMBERFAN_SWITCH] = ui_settings_chamberFanSwitch;
    children[UI_COMP_SETTINGSCOMPONENT_CHAMBER_TEMP] = cui_settings_chamberSensor;
    children[UI_COMP_SETTINGSCOMPONENT_CHAMBER_TEMP_LABEL] = cui_settings_chamberSensorLabel;
    children[UI_COMP_SETTINGSCOMPONENT_CHAMBER_TEMP_SWITCH] = ui_settings_chamberSensorSwitch;
    children[UI_COMP_SETTINGSCOMPONENT_OTA] = cui_settings_ota;
    children[UI_COMP_SETTINGSCOMPONENT_OTA_LABEL] = cui_settings_otaLabel;
    children[UI_COMP_SETTINGSCOMPONENT_OTA_SWITCH] = ui_settings_otaSwitch;
    children[UI_COMP_SETTINGSCOMPONENT_RESETDEVICEBUTTON] = cui_reseDeviceButton;

    lv_obj_add_event_cb(cui_settingsComponent, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_settingsComponent, del_component_child_event_cb, LV_EVENT_DELETE, children);

    lv_obj_add_event_cb(cui_reseDeviceButton, ui_event_comp_settingsComponent_resetDeviceButton, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(ui_settingsTFTOFFSlider, ui_event_comp_settingsComponent_onTFTOFF, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsBackLightPanelSlider, ui_event_comp_settingsComponent_onBackLight, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsWOPSwitch, ui_event_comp_settingsComponent_onWOP, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_settingsTFTInvertSwitch, ui_event_comp_settingsComponent_tftInvertInput, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_add_event_cb(ui_settings_auxFanSwitch, ui_event_comp_settingsComponent_onAuxFan, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_settings_chamberFanSwitch, ui_event_comp_settingsComponent_onChamberFan, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_settings_chamberSensorSwitch, ui_event_comp_settingsComponent_onChamberTemp, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_settingsTFTFlipSwitch, ui_event_comp_settingsComponent_onTFTFlip, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_settings_otaSwitch, ui_event_comp_settingsComponent_onOTA, LV_EVENT_VALUE_CHANGED, NULL);

    ui_comp_settingsComponent_create_hook(cui_settingsComponent);
    return cui_settingsComponent;
}
