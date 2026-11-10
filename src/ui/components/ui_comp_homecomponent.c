#include <Arduino.h>
#include <stdio.h>
#include "../ui.h"
#include "bambudeck/globals.h"

extern void bambudeck_thumbnail_update_ui(void);

const char *bambudeck_device_get_print_state()
{
    switch (bambuStatus.print_status)
    {
    case BAMBUDECK_PRINT_STATUS_IDLE:
        return LV_SYMBOL_OK " Ready";
    case BAMBUDECK_PRINT_STATUS_FINISHED:
        return LV_SYMBOL_OK " Finished";
    case BAMBUDECK_PRINT_STATUS_FAILED:
        return LV_SYMBOL_WARNING " Failed";
    default:
        return "Standby";
    }
}

void ui_event_comp_homeComponent_mainScreenPlayPauseButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onHomeControllerPlayPause(e);
    }
}
void ui_event_comp_homeComponent_mainScreenStopButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onHomeControllerStop(e);
    }
}
void ui_event_comp_homeComponent_mainScreenSpeedDropDown(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        onHomeSpeedSelection(e);
    }
}
void ui_event_comp_homeComponent_mainScreenLightButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t **comp_homeComponent = lv_event_get_user_data(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onHomeLight(e);
        _ui_state_modify(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENLIGHTBUTTON], LV_STATE_CHECKED, _UI_MODIFY_STATE_TOGGLE);
    }
}
void ui_event_comp_homeComponent_mainScreenBedTemp(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onHomeBedTemp(e);
    }
}
void ui_event_comp_homeComponent_mainScreenNozzleTemp(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onHomeNozzleTemp(e, 0);
    }
}

void ui_event_comp_homeComponent_mainScreenSpeedChange(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t index = lv_dropdown_get_selected(target);
        switch (index)
        {
        case 0:
            bambuStatus.printing_speed_lvl = BAMBUDECK_SPEED_LEVEL_SILENCE;
            break;
        case 1:
            bambuStatus.printing_speed_lvl = BAMBUDECK_SPEED_LEVEL_NORMAL;
            break;
        case 2:
            bambuStatus.printing_speed_lvl = BAMBUDECK_SPEED_LEVEL_RAPID;
            break;
        case 3:
            bambuStatus.printing_speed_lvl = BAMBUDECK_SPEED_LEVEL_RAMPAGE;
            break;
        }
        lv_msg_send(BAMBUDECK_COMMAND_PRINT_SPEED, NULL);
    }
}

void onBambuDeckLightMessage(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;

    if (message->data == 1)
    {
        lv_obj_add_state(target, LV_STATE_CHECKED);
    }
    else
    {
        lv_obj_clear_state(target, LV_STATE_CHECKED);
    }
}

void onBambuDeckBedTemp(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;

    char value[10];
    itoa(message->data, value, 10);
    lv_label_set_text(target, value);
}

void onBambuDeckChamberTemp(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;

    char value[10];
    itoa(message->data, value, 10);
    lv_label_set_text(target, value);
}

void onBambuDeckBedTempTarget(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;
    lv_obj_set_style_text_color(target, message->data > 0 ? BAMBU_COLOR_ORANGE() : BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

void onBambuDeckAMS(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;
    if (message->data == 1)
    {
        lv_obj_clear_flag(target, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(target, LV_OBJ_FLAG_HIDDEN);
    }
}

void onBambuDeckWifiSignal(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;
    lv_obj_set_style_text_color(target, message->data < 65 ? BAMBU_COLOR_GREEN() : BAMBU_COLOR_ORANGE(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

void onBambuDeckIPCam(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    if (bambuStatus.has_ipcam)
    {
        lv_obj_clear_flag(target, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_text_color(target, (bambuStatus.camera_recording_when_printing || bambuStatus.camera_timelapse) ? BAMBU_COLOR_GREEN() : BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(target, bambuStatus.camera_timelapse ? "y 2" : "y");
    }
    else
    {
        lv_obj_add_flag(target, LV_OBJ_FLAG_HIDDEN);
    }
}

#define BAMBUDECK_THUMB_PRINTING_SIZE 80

static lv_obj_t *ui_mainScreenPercentLabel = NULL;

static void ui_mainScreenPercentLabel_onDelete(lv_event_t *e)
{
    ui_mainScreenPercentLabel = NULL;
}

int printingLevelToIndex(int lvl)
{
    switch (lvl)
    {
    case BAMBUDECK_SPEED_LEVEL_INVALID:
        return 1;
    case BAMBUDECK_SPEED_LEVEL_SILENCE:
        return 0;
    case BAMBUDECK_SPEED_LEVEL_NORMAL:
        return 1;
    case BAMBUDECK_SPEED_LEVEL_RAPID:
        return 2;
    case BAMBUDECK_SPEED_LEVEL_RAMPAGE:
        return 3;
    }
    return 1;
}

void onBambuDeckPrintStatus(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t **comp_homeComponent = lv_event_get_user_data(e);

    ui_confirmPanel_hide();

    int displayPct = bambuStatus.mc_print_percent;
    if (displayPct == 0 && bambuStatus.total_layers > 0 && bambuStatus.current_layer > 0)
    {
        displayPct = (bambuStatus.current_layer * 100) / bambuStatus.total_layers;
    }
    if (displayPct < 0) displayPct = 0;
    if (displayPct > 100) displayPct = 100;

    char layerText[32];
    sprintf(layerText, "%d/%d", bambuStatus.current_layer, bambuStatus.total_layers);
    lv_label_set_text(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER2_MAINSCREENLAYER], layerText);
    lv_slider_set_value(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENPROGRESSBAR], displayPct, LV_ANIM_ON);

    char remainingTimeText[48];
    _ui_seconds_to_timeleft(bambuStatus.mc_left_time, remainingTimeText);

    lv_label_set_text(target, remainingTimeText);
    if (ui_mainScreenPercentLabel != NULL)
    {
        lv_label_set_text_fmt(ui_mainScreenPercentLabel, "%d%%", displayPct);
    }

    lv_obj_t *playPauseButton = comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER_MAINSCREENPLAYPAUSEBUTTON];
    lv_obj_t *dropDown = comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENCENTRAL_MAINSCREENSPEEDDROPDOWN];

    switch (bambuStatus.print_status)
    {
    case BAMBUDECK_PRINT_STATUS_PAUSED:
        lv_label_set_text(playPauseButton, "z");
        lv_obj_clear_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENCENTRAL], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENIDLE], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(dropDown, LV_STATE_DISABLED);
        break;
    case BAMBUDECK_PRINT_STATUS_RUNNING:
        lv_label_set_text(playPauseButton, "0");
        lv_obj_clear_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENCENTRAL], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENIDLE], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(dropDown, LV_STATE_DISABLED);
        break;
    case BAMBUDECK_PRINT_STATUS_PREPARE:
        lv_obj_clear_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENCENTRAL], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENIDLE], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_state(dropDown, LV_STATE_DISABLED);
        break;

    case BAMBUDECK_PRINT_STATUS_IDLE:
    case BAMBUDECK_PRINT_STATUS_FINISHED:
    case BAMBUDECK_PRINT_STATUS_FAILED:
        lv_obj_add_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENCENTRAL], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(comp_homeComponent[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENIDLE], LV_OBJ_FLAG_HIDDEN);
        if (ui_mainStatusComponent != NULL)
        {
            lv_obj_t *statusCaption = ui_comp_get_child(ui_mainStatusComponent, UI_COMP_MAINSCREENSTATUS_MAINSCREENSTATUSCAPTION);
            if (statusCaption)
            {
                lv_label_set_text(statusCaption, bambudeck_device_get_print_state());
            }
            lv_obj_t *statusSubtask = ui_comp_get_child(ui_mainStatusComponent, UI_COMP_MAINSCREENSTATUS_MAINSCREENSTATUSSUBTASK);
            if (statusSubtask)
            {
                lv_label_set_text(statusSubtask, bambuStatus.subtask_name);
            }
        }
        lv_obj_add_state(dropDown, LV_STATE_DISABLED);
        break;
    }

    lv_obj_clear_state(playPauseButton, LV_STATE_DISABLED);
    lv_dropdown_set_selected(dropDown, printingLevelToIndex(bambuStatus.printing_speed_lvl));
    bambudeck_thumbnail_update_ui();
}

static lv_obj_t *create_ams_slot_pill(lv_obj_t *parent, uint32_t default_hex)
{
    lv_obj_t *pill = lv_obj_create(parent);
    lv_obj_set_height(pill, lv_pct(100));
    lv_obj_set_flex_grow(pill, 1);
    lv_obj_clear_flag(pill, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(pill, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(pill, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(pill, lv_color_hex(default_hex), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(pill, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(pill, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(pill, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(pill, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    return pill;
}

// COMPONENT homeComponent

lv_obj_t *ui_homeComponent_create(lv_obj_t *comp_parent)
{
    lv_obj_t *cui_homeComponent;
    cui_homeComponent = lv_obj_create(ui_homeScreen);
    lv_obj_set_width(cui_homeComponent, 272);
    lv_obj_set_height(cui_homeComponent, lv_pct(100));
    lv_obj_set_flex_flow(cui_homeComponent, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_homeComponent, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_homeComponent, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_homeComponent, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_homeComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_homeComponent, BAMBU_COLOR_BG_SCREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_homeComponent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_homeComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_homeComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_homeComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_homeComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_homeComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_homeComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cui_homeComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_homeComponent, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_homeComponent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenLeft;
    cui_mainScreenLeft = lv_obj_create(cui_homeComponent);
    lv_obj_set_width(cui_mainScreenLeft, 192);
    lv_obj_set_height(cui_mainScreenLeft, lv_pct(100));
    lv_obj_set_flex_flow(cui_mainScreenLeft, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_mainScreenLeft, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cui_mainScreenLeft, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenLeft, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenLeft, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenLeft, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenLeft, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_mainScreenLeft, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_mainScreenLeft, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenLeft, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_mainScreenLeft, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_mainScreenLeft, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cui_mainScreenLeft, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenLeft, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_mainScreenLeft, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenStatusBar;
    cui_mainScreenStatusBar = lv_obj_create(cui_mainScreenLeft);
    lv_obj_set_height(cui_mainScreenStatusBar, 28);
    lv_obj_set_width(cui_mainScreenStatusBar, lv_pct(100));
    lv_obj_set_flex_flow(cui_mainScreenStatusBar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_mainScreenStatusBar, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_mainScreenStatusBar, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenStatusBar, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenStatusBar, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenStatusBar, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenStatusBar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenStatusBar, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_mainScreenStatusBar, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_mainScreenStatusBar, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_mainScreenStatusBar, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenStatusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_mainScreenStatusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_mainScreenStatusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cui_mainScreenStatusBar, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenStatusBar, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_mainScreenStatusBar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenWifiIcon;
    cui_mainScreenWifiIcon = lv_label_create(cui_mainScreenStatusBar);
    lv_obj_set_width(cui_mainScreenWifiIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenWifiIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_mainScreenWifiIcon, "x");
    lv_obj_clear_flag(cui_mainScreenWifiIcon, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenWifiIcon, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_font(cui_mainScreenWifiIcon, &ui_font_bambudeck_min, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenWifiIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenCameraIcon;
    cui_mainScreenCameraIcon = lv_label_create(cui_mainScreenStatusBar);
    lv_obj_set_height(cui_mainScreenCameraIcon, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cui_mainScreenCameraIcon, 1);
    lv_label_set_text(cui_mainScreenCameraIcon, "y");
    lv_obj_clear_flag(cui_mainScreenCameraIcon, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenCameraIcon, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_font(cui_mainScreenCameraIcon, &ui_font_bambudeck_min, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenCameraIcon, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_mainScreenCameraIcon, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_mainScreenCameraIcon, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenCameraIcon, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_mainScreenCameraIcon, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenAMS;
    cui_mainScreenAMS = lv_obj_create(cui_mainScreenStatusBar);
    lv_obj_set_height(cui_mainScreenAMS, lv_pct(70));
    lv_obj_set_flex_grow(cui_mainScreenAMS, 1);
    lv_obj_set_flex_flow(cui_mainScreenAMS, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_mainScreenAMS, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cui_mainScreenAMS, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenAMS, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenAMS, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenAMS, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenAMS, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenAMS, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_mainScreenAMS, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cui_mainScreenAMS, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(cui_mainScreenAMS, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *cui_mainScreenAMSColor0 = create_ams_slot_pill(cui_mainScreenAMS, 0xFFFFFF);
    lv_obj_t *cui_mainScreenAMSColor1 = create_ams_slot_pill(cui_mainScreenAMS, 0xFF0000);
    lv_obj_t *cui_mainScreenAMSColor2 = create_ams_slot_pill(cui_mainScreenAMS, 0x00AE42);
    lv_obj_t *cui_mainScreenAMSColor3 = create_ams_slot_pill(cui_mainScreenAMS, 0x0066FF);

    ui_mainStatusComponent = ui_mainScreenStatus_create(cui_mainScreenLeft);

    /* ---- Printing card: [thumbnail | % / time / layers] + progress + controls ---- */
    lv_obj_t *cui_mainScreenPlayer;
    cui_mainScreenPlayer = lv_obj_create(cui_mainScreenLeft);
    lv_obj_set_width(cui_mainScreenPlayer, lv_pct(100));
    lv_obj_set_flex_grow(cui_mainScreenPlayer, 1);
    lv_obj_set_flex_flow(cui_mainScreenPlayer, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_mainScreenPlayer, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_mainScreenPlayer, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenPlayer, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenPlayer, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenPlayer, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenPlayer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenPlayer, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_mainScreenPlayer, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_mainScreenPlayer, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_mainScreenPlayer, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenPlayer, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_mainScreenPlayer, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_mainScreenPlayer, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenPlayer, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenController = lv_obj_create(cui_mainScreenPlayer);
    lv_obj_clear_flag(cui_mainScreenController, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenController, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_mainScreenController, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenController, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenController, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenController, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(cui_mainScreenController, lv_pct(100));
    lv_obj_set_height(cui_mainScreenController, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_mainScreenController, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_mainScreenController, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(cui_mainScreenController, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenThumbFrame = lv_obj_create(cui_mainScreenController);
    lv_obj_set_size(cui_mainScreenThumbFrame, BAMBUDECK_THUMB_PRINTING_SIZE, BAMBUDECK_THUMB_PRINTING_SIZE);
    lv_obj_set_flex_flow(cui_mainScreenThumbFrame, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_mainScreenThumbFrame, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_mainScreenThumbFrame, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenThumbFrame, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenThumbFrame, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenThumbFrame, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenThumbFrame, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenThumbFrame, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenThumbFrame, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(cui_mainScreenThumbFrame, true, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenNozzleIcon;
    cui_mainScreenNozzleIcon = lv_img_create(cui_mainScreenThumbFrame);
    lv_img_set_size_mode(cui_mainScreenNozzleIcon, LV_IMG_SIZE_MODE_REAL);
    lv_img_set_src(cui_mainScreenNozzleIcon, LV_SYMBOL_IMAGE);
    lv_obj_set_style_text_font(cui_mainScreenNozzleIcon, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenNozzleIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenStats = lv_obj_create(cui_mainScreenController);
    lv_obj_clear_flag(cui_mainScreenStats, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenStats, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_mainScreenStats, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenStats, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenStats, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenStats, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_height(cui_mainScreenStats, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cui_mainScreenStats, 1);
    lv_obj_set_flex_flow(cui_mainScreenStats, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_mainScreenStats, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(cui_mainScreenStats, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    int initPct = bambuStatus.mc_print_percent;
    if (initPct == 0 && bambuStatus.total_layers > 0 && bambuStatus.current_layer > 0)
    {
        initPct = (bambuStatus.current_layer * 100) / bambuStatus.total_layers;
    }
    if (initPct < 0) initPct = 0;
    if (initPct > 100) initPct = 100;

    ui_mainScreenPercentLabel = lv_label_create(cui_mainScreenStats);
    lv_label_set_text_fmt(ui_mainScreenPercentLabel, "%d%%", initPct);
    lv_obj_set_style_text_font(ui_mainScreenPercentLabel, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_mainScreenPercentLabel, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_mainScreenPercentLabel, ui_mainScreenPercentLabel_onDelete, LV_EVENT_DELETE, NULL);

    lv_obj_t *cui_mainScreenController1 = lv_obj_create(cui_mainScreenStats);
    lv_obj_clear_flag(cui_mainScreenController1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenController1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_mainScreenController1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenController1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenController1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenController1, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(cui_mainScreenController1, lv_pct(100));
    lv_obj_set_height(cui_mainScreenController1, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_mainScreenController1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_mainScreenController1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(cui_mainScreenController1, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenTimeLeftIcon;
    cui_mainScreenTimeLeftIcon = lv_label_create(cui_mainScreenController1);
    lv_label_set_text(cui_mainScreenTimeLeftIcon, "2");
    lv_obj_set_style_text_font(cui_mainScreenTimeLeftIcon, &ui_font_bambudeck_min, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenTimeLeftIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenTimeLeft;
    cui_mainScreenTimeLeft = lv_label_create(cui_mainScreenController1);
    lv_label_set_text(cui_mainScreenTimeLeft, "--");
    lv_obj_set_style_text_font(cui_mainScreenTimeLeft, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenController2 = lv_obj_create(cui_mainScreenStats);
    lv_obj_clear_flag(cui_mainScreenController2, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenController2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_mainScreenController2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenController2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenController2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenController2, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(cui_mainScreenController2, lv_pct(100));
    lv_obj_set_height(cui_mainScreenController2, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_mainScreenController2, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_mainScreenController2, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(cui_mainScreenController2, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenLayerIcon;
    cui_mainScreenLayerIcon = lv_label_create(cui_mainScreenController2);
    lv_label_set_text(cui_mainScreenLayerIcon, "3");
    lv_obj_set_style_text_font(cui_mainScreenLayerIcon, &ui_font_bambudeck_min, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenLayerIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenLayer;
    cui_mainScreenLayer = lv_label_create(cui_mainScreenController2);
    char layerInitText[32];
    snprintf(layerInitText, sizeof(layerInitText), "%d/%d", bambuStatus.current_layer, bambuStatus.total_layers);
    lv_label_set_text(cui_mainScreenLayer, layerInitText);
    lv_obj_set_style_text_font(cui_mainScreenLayer, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenProgressBar;
    cui_mainScreenProgressBar = lv_slider_create(cui_mainScreenPlayer);
    lv_slider_set_value(cui_mainScreenProgressBar, initPct, LV_ANIM_OFF);
    lv_obj_set_width(cui_mainScreenProgressBar, lv_pct(100));
    lv_obj_set_height(cui_mainScreenProgressBar, 8);
    lv_obj_clear_flag(cui_mainScreenProgressBar, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_set_style_radius(cui_mainScreenProgressBar, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenProgressBar, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenProgressBar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(cui_mainScreenProgressBar, 4, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenProgressBar, BAMBU_COLOR_GREEN(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenProgressBar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenProgressBar, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenProgressBar, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenButtons = lv_obj_create(cui_mainScreenPlayer);
    lv_obj_clear_flag(cui_mainScreenButtons, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenButtons, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_mainScreenButtons, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenButtons, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenButtons, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenButtons, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(cui_mainScreenButtons, lv_pct(100));
    lv_obj_set_height(cui_mainScreenButtons, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_mainScreenButtons, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_mainScreenButtons, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(cui_mainScreenButtons, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenPlayPauseButton = lv_label_create(cui_mainScreenButtons);
    lv_obj_set_height(cui_mainScreenPlayPauseButton, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cui_mainScreenPlayPauseButton, 1);
    lv_label_set_text(cui_mainScreenPlayPauseButton, "0");
    lv_obj_add_flag(cui_mainScreenPlayPauseButton, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cui_mainScreenPlayPauseButton, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenPlayPauseButton, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_align(cui_mainScreenPlayPauseButton, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_mainScreenPlayPauseButton, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(cui_mainScreenPlayPauseButton, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenPlayPauseButton, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenPlayPauseButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_mainScreenPlayPauseButton, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenPlayPauseButton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenPlayPauseButton, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_mainScreenPlayPauseButton, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenPlayPauseButton, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cui_mainScreenPlayPauseButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(cui_mainScreenPlayPauseButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_t *cui_mainScreenStopButton = lv_label_create(cui_mainScreenButtons);
    lv_obj_set_height(cui_mainScreenStopButton, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cui_mainScreenStopButton, 1);
    lv_label_set_text(cui_mainScreenStopButton, "1");
    lv_obj_add_flag(cui_mainScreenStopButton, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cui_mainScreenStopButton, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenStopButton, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_align(cui_mainScreenStopButton, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_mainScreenStopButton, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(cui_mainScreenStopButton, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenStopButton, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenStopButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_mainScreenStopButton, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenStopButton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenStopButton, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_mainScreenStopButton, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenStopButton, BAMBU_COLOR_RED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cui_mainScreenStopButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(cui_mainScreenStopButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_t *cui_mainScreenCentral;
    cui_mainScreenCentral = lv_obj_create(cui_mainScreenLeft);
    lv_obj_set_width(cui_mainScreenCentral, lv_pct(100));
    lv_obj_set_height(cui_mainScreenCentral, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cui_mainScreenCentral, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_mainScreenCentral, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_mainScreenCentral, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenCentral, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenCentral, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenCentral, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenCentral, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenCentral, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_mainScreenCentral, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenCentral, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenCentral, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenSpeedIcon;
    cui_mainScreenSpeedIcon = lv_label_create(cui_mainScreenCentral);
    lv_obj_set_width(cui_mainScreenSpeedIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenSpeedIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_mainScreenSpeedIcon, "h");
    lv_obj_set_style_text_font(cui_mainScreenSpeedIcon, &ui_font_bambudeck_min, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenSpeedIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenSpeedDropDown;
    cui_mainScreenSpeedDropDown = lv_dropdown_create(cui_mainScreenCentral);
    lv_dropdown_set_options(cui_mainScreenSpeedDropDown, "Silent\nStandard\nSport\nLudicrous");
    lv_obj_set_width(cui_mainScreenSpeedDropDown, 146);
    lv_obj_set_height(cui_mainScreenSpeedDropDown, LV_SIZE_CONTENT);
    lv_obj_add_flag(cui_mainScreenSpeedDropDown, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_bg_color(cui_mainScreenSpeedDropDown, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenSpeedDropDown, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenSpeedDropDown, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenSpeedDropDown, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_dropdown_set_selected(cui_mainScreenSpeedDropDown, 1);

    lv_obj_set_style_text_color(cui_mainScreenSpeedDropDown, BAMBU_COLOR_GREEN(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_mainScreenSpeedDropDown, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_t *dd_list = lv_dropdown_get_list(cui_mainScreenSpeedDropDown);
    lv_obj_set_style_text_line_space(dd_list, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(dd_list, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(dd_list, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(dd_list, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(dd_list, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(dd_list, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(dd_list, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(dd_list, BAMBU_COLOR_GREEN(), LV_PART_SELECTED | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(dd_list, 255, LV_PART_SELECTED | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(dd_list, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(dd_list, BAMBU_COLOR_GREEN_PRESSED(), LV_PART_SELECTED | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(dd_list, 255, LV_PART_SELECTED | LV_STATE_PRESSED);

    lv_obj_t *cui_mainScreenRight;
    cui_mainScreenRight = lv_obj_create(cui_homeComponent);
    lv_obj_set_width(cui_mainScreenRight, 68);
    lv_obj_set_height(cui_mainScreenRight, lv_pct(100));
    lv_obj_set_flex_flow(cui_mainScreenRight, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_mainScreenRight, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cui_mainScreenRight, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenRight, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenRight, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenRight, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenRight, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenRight, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_mainScreenRight, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenRight, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenLightButton;
    cui_mainScreenLightButton = lv_obj_create(cui_mainScreenRight);
    lv_obj_set_width(cui_mainScreenLightButton, lv_pct(100));
    lv_obj_set_flex_grow(cui_mainScreenLightButton, 1);
    lv_obj_set_flex_flow(cui_mainScreenLightButton, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_mainScreenLightButton, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_mainScreenLightButton, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenLightButton, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenLightButton, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenLightButton, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenLightButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenLightButton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_mainScreenLightButton, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenLightButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenLightButton, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_mainScreenLightButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Checked state (Chamber Light ON): Bambu Green tint & border */
    lv_obj_set_style_bg_color(cui_mainScreenLightButton, BAMBU_COLOR_BG_GREEN_TINT(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(cui_mainScreenLightButton, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_opa(cui_mainScreenLightButton, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(cui_mainScreenLightButton, 2, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(cui_mainScreenLightButton, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(cui_mainScreenLightButton, 255, LV_PART_MAIN | LV_STATE_CHECKED);

    lv_obj_t *cui_mainScreenLightButtonIcon;
    cui_mainScreenLightButtonIcon = lv_label_create(cui_mainScreenLightButton);
    lv_obj_set_width(cui_mainScreenLightButtonIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenLightButtonIcon, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenLightButtonIcon, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenLightButtonIcon, "w");
    lv_obj_set_style_text_font(cui_mainScreenLightButtonIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenTemps;
    cui_mainScreenTemps = lv_obj_create(cui_mainScreenRight);
    lv_obj_set_height(cui_mainScreenTemps, 184);
    lv_obj_set_width(cui_mainScreenTemps, lv_pct(100));
    lv_obj_set_flex_flow(cui_mainScreenTemps, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_mainScreenTemps, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cui_mainScreenTemps, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenTemps, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenTemps, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenTemps, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenTemps, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenTemps, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_mainScreenTemps, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_mainScreenTemps, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_mainScreenTemps, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenTemps, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_mainScreenTemps, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_mainScreenTemps, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenTemps, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenNozzleTemp;
    cui_mainScreenNozzleTemp = lv_obj_create(cui_mainScreenTemps);
    lv_obj_set_width(cui_mainScreenNozzleTemp, lv_pct(100));
    lv_obj_set_flex_grow(cui_mainScreenNozzleTemp, 2);
    lv_obj_set_flex_flow(cui_mainScreenNozzleTemp, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_mainScreenNozzleTemp, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_mainScreenNozzleTemp, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenNozzleTemp, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenNozzleTemp, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenNozzleTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenNozzleTemp, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(cui_mainScreenNozzleTemp, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_mainScreenNozzleTemp, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenNozzleTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_mainScreenNozzleTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenNozzleTemp, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenNozzleTemp, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cui_mainScreenNozzleTemp, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(cui_mainScreenNozzleTemp, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_t *cui_mainScreenNozzleTempIcon;
    cui_mainScreenNozzleTempIcon = lv_label_create(cui_mainScreenNozzleTemp);
    lv_obj_set_width(cui_mainScreenNozzleTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenNozzleTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenNozzleTempIcon, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenNozzleTempIcon, "f");
    lv_obj_set_style_text_color(cui_mainScreenNozzleTempIcon, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_mainScreenNozzleTempIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenNozzleTempValue;
    cui_mainScreenNozzleTempValue = lv_label_create(cui_mainScreenNozzleTemp);
    lv_obj_set_width(cui_mainScreenNozzleTempValue, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenNozzleTempValue, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenNozzleTempValue, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenNozzleTempValue, "--");
    lv_obj_set_style_text_font(cui_mainScreenNozzleTempValue, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenNozzleTempValue, -4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenBedTemp;
    cui_mainScreenBedTemp = lv_obj_create(cui_mainScreenTemps);
    lv_obj_set_width(cui_mainScreenBedTemp, lv_pct(100));
    lv_obj_set_flex_grow(cui_mainScreenBedTemp, 2);
    lv_obj_set_flex_flow(cui_mainScreenBedTemp, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_mainScreenBedTemp, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_mainScreenBedTemp, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenBedTemp, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_mainScreenBedTemp, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenBedTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenBedTemp, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(cui_mainScreenBedTemp, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_mainScreenBedTemp, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenBedTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_mainScreenBedTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenBedTemp, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_mainScreenBedTemp, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cui_mainScreenBedTemp, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(cui_mainScreenBedTemp, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_t *cui_mainScreenBedTempIcon;
    cui_mainScreenBedTempIcon = lv_label_create(cui_mainScreenBedTemp);
    lv_obj_set_width(cui_mainScreenBedTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenBedTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenBedTempIcon, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenBedTempIcon, "e");
    lv_obj_set_style_text_color(cui_mainScreenBedTempIcon, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_mainScreenBedTempIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenBedTempValue;
    cui_mainScreenBedTempValue = lv_label_create(cui_mainScreenBedTemp);
    lv_obj_set_width(cui_mainScreenBedTempValue, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenBedTempValue, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenBedTempValue, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenBedTempValue, "--");
    lv_obj_set_style_text_font(cui_mainScreenBedTempValue, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenBedTempValue, -4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenChamberTemp;
    cui_mainScreenChamberTemp = lv_obj_create(cui_mainScreenTemps);
    lv_obj_set_width(cui_mainScreenChamberTemp, lv_pct(100));
    lv_obj_set_flex_grow(cui_mainScreenChamberTemp, 2);
    lv_obj_set_flex_flow(cui_mainScreenChamberTemp, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_mainScreenChamberTemp, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_mainScreenChamberTemp, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenChamberTemp, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_mainScreenChamberTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenChamberTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenChamberTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_mainScreenChamberTemp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenChamberTemp, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    if (bambudeck_bblp_is_p1Series() && !bambuDeckConfig.chamberSensorEnabled)
    {
        lv_obj_add_flag(cui_mainScreenChamberTemp, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_t *cui_mainScreenChamberTempIcon;
    cui_mainScreenChamberTempIcon = lv_label_create(cui_mainScreenChamberTemp);
    lv_obj_set_width(cui_mainScreenChamberTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenChamberTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenChamberTempIcon, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenChamberTempIcon, "g");
    lv_obj_set_style_text_color(cui_mainScreenChamberTempIcon, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_mainScreenChamberTempIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenChamberTempValue;
    cui_mainScreenChamberTempValue = lv_label_create(cui_mainScreenChamberTemp);
    lv_obj_set_width(cui_mainScreenChamberTempValue, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenChamberTempValue, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenChamberTempValue, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenChamberTempValue, "--");
    lv_obj_set_style_text_font(cui_mainScreenChamberTempValue, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_mainScreenChamberTempValue, -4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t **children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_HOMECOMPONENT_NUM);
    children[UI_COMP_HOMECOMPONENT_HOMECOMPONENT] = cui_homeComponent;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT] = cui_mainScreenLeft;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR] = cui_mainScreenStatusBar;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENWIFIICON] = cui_mainScreenWifiIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENCAMERAICON] = cui_mainScreenCameraIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENAMS] = cui_mainScreenAMS;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENAMS_MAINSCREENAMSCOLOR0] = cui_mainScreenAMSColor0;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENAMS_MAINSCREENAMSCOLOR1] = cui_mainScreenAMSColor1;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENAMS_MAINSCREENAMSCOLOR2] = cui_mainScreenAMSColor2;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENSTATUSBAR_MAINSCREENAMS_MAINSCREENAMSCOLOR3] = cui_mainScreenAMSColor3;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENIDLE] = ui_mainStatusComponent;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER] = cui_mainScreenPlayer;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER] = cui_mainScreenController;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER_MAINSCREENNOZZLEICON] = cui_mainScreenNozzleIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER_MAINSCREENPLAYPAUSEBUTTON] = cui_mainScreenPlayPauseButton;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER_MAINSCREENSTOPBUTTON] = cui_mainScreenStopButton;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENPROGRESSBAR] = cui_mainScreenProgressBar;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER1] = cui_mainScreenController1;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER1_MAINSCREENTIMELEFTICON] = cui_mainScreenTimeLeftIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER1_MAINSCREENTIMELEFT] = cui_mainScreenTimeLeft;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER2] = cui_mainScreenController2;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER2_MAINSCREENLAYERICON] = cui_mainScreenLayerIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER2_MAINSCREENLAYER] = cui_mainScreenLayer;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENCENTRAL] = cui_mainScreenCentral;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENCENTRAL_MAINSCREENSPEEDICON] = cui_mainScreenSpeedIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENCENTRAL_MAINSCREENSPEEDDROPDOWN] = cui_mainScreenSpeedDropDown;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT] = cui_mainScreenRight;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENLIGHTBUTTON] = cui_mainScreenLightButton;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENLIGHTBUTTON_MAINSCREENLIGHTBUTTONICON] = cui_mainScreenLightButtonIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS] = cui_mainScreenTemps;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS_MAINSCREENNOZZLETEMP] = cui_mainScreenNozzleTemp;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS_MAINSCREENNOZZLETEMP_MAINSCREENNOZZLETEMPICON] = cui_mainScreenNozzleTempIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS_MAINSCREENNOZZLETEMP_MAINSCREENNOZZLETEMPVALUE] = cui_mainScreenNozzleTempValue;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS_MAINSCREENBEDTEMP] = cui_mainScreenBedTemp;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS_MAINSCREENBEDTEMP_MAINSCREENBEDTEMPICON] = cui_mainScreenBedTempIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS_MAINSCREENBEDTEMP_MAINSCREENBEDTEMPVALUE] = cui_mainScreenBedTempValue;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS_MAINSCREENCHAMBERTEMP] = cui_mainScreenChamberTemp;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS_MAINSCREENCHAMBERTEMP_MAINSCREENCHAMBERTEMPICON] = cui_mainScreenChamberTempIcon;
    children[UI_COMP_HOMECOMPONENT_MAINSCREENRIGHT_MAINSCREENTEMPS_MAINSCREENCHAMBERTEMP_MAINSCREENCHAMBERTEMPVALUE] = cui_mainScreenChamberTempValue;
    lv_obj_add_event_cb(cui_homeComponent, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_homeComponent, del_component_child_event_cb, LV_EVENT_DELETE, children);
    lv_obj_add_event_cb(cui_mainScreenPlayPauseButton, ui_event_comp_homeComponent_mainScreenPlayPauseButton, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_mainScreenStopButton, ui_event_comp_homeComponent_mainScreenStopButton, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_mainScreenSpeedDropDown, ui_event_comp_homeComponent_mainScreenSpeedDropDown, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_mainScreenLightButton, ui_event_comp_homeComponent_mainScreenLightButton, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_mainScreenBedTemp, ui_event_comp_homeComponent_mainScreenBedTemp, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_mainScreenNozzleTemp, ui_event_comp_homeComponent_mainScreenNozzleTemp, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_mainScreenSpeedDropDown, ui_event_comp_homeComponent_mainScreenSpeedChange, LV_EVENT_ALL, children);

    lv_obj_add_event_cb(cui_mainScreenLightButton, onBambuDeckLightMessage, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_LIGHT_REPORT, cui_mainScreenLightButton, NULL);

    lv_obj_add_event_cb(cui_mainScreenBedTempValue, onBambuDeckBedTemp, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_BED_TEMP, cui_mainScreenBedTempValue, NULL);

    lv_obj_add_event_cb(cui_mainScreenBedTempIcon, onBambuDeckBedTempTarget, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_BED_TARGET_TEMP, cui_mainScreenBedTempIcon, NULL);

    lv_obj_add_event_cb(cui_mainScreenNozzleTempValue, onBambuDeckBedTemp, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_NOZZLE_TEMP, cui_mainScreenNozzleTempValue, NULL);

    lv_obj_add_event_cb(cui_mainScreenChamberTempValue, onBambuDeckChamberTemp, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_CHAMBER_TEMP, cui_mainScreenChamberTempValue, NULL);

    lv_obj_add_event_cb(cui_mainScreenNozzleTempIcon, onBambuDeckBedTempTarget, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_NOZZLE_TARGET_TEMP, cui_mainScreenNozzleTempIcon, NULL);

    lv_obj_add_event_cb(cui_mainScreenAMS, onBambuDeckAMS, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_AMS, cui_mainScreenAMS, NULL);

    lv_obj_add_event_cb(cui_mainScreenWifiIcon, onBambuDeckWifiSignal, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_WIFI_SIGNAL, cui_mainScreenWifiIcon, NULL);

    lv_obj_add_event_cb(cui_mainScreenCameraIcon, onBambuDeckIPCam, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_IPCAM, cui_mainScreenCameraIcon, NULL);

    lv_obj_add_event_cb(cui_mainScreenTimeLeft, onBambuDeckPrintStatus, LV_EVENT_MSG_RECEIVED, children);
    lv_msg_subsribe_obj(BAMBUDECK_ON_PRINT_STATUS, cui_mainScreenTimeLeft, NULL);

    ui_comp_homeComponent_create_hook(cui_homeComponent);
    return cui_homeComponent;
}
