#include "../ui.h"

static lv_obj_t *s_axisModeLabel = NULL;
static lv_obj_t *s_upHintLabel = NULL;
static lv_obj_t *s_downHintLabel = NULL;
static lv_obj_t *s_leftHintLabel = NULL;
static lv_obj_t *s_rightHintLabel = NULL;

void ui_event_comp_controlComponent_controlScreenRange(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        controlMode.inc = controlMode.inc == 1 ? 10 : 1;
        onControlRange(e);
    }
}
void ui_event_comp_controlComponent_controlScreenLeft(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onControlLeft(e);
    }
}
void ui_event_comp_controlComponent_controlScreenUp(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onControlUp(e);
    }
}

void ui_event_comp_controlComponent_controlScreenHomeConfirm() { onControlHome(NULL); }
void ui_event_comp_controlComponent_controlScreenHome(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        ui_confirmPanel_show(LV_SYMBOL_WARNING " Home All Axes?", ui_event_comp_controlComponent_controlScreenHomeConfirm);
    }
}
void ui_event_comp_controlComponent_controlScreenDown(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onControlDown(e);
    }
}
void ui_event_comp_controlComponent_controlScreenAxixToggle(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onControlAxis(e);
    }
}
void ui_event_comp_controlComponent_controlScreenRight(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onControlRight(e);
    }
}

void onBambuDeckRangeChange(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_label_set_text(target, controlMode.inc == 1 ? "1mm" : "10mm");
}

void onBambuDeckAxisChange(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    controlMode.axis = controlMode.axis == 0 ? 1 : 0;
    lv_label_set_text(target, controlMode.axis == 0 ? "l" : "m");
    if (s_axisModeLabel)
    {
        lv_label_set_text(s_axisModeLabel, controlMode.axis == 0 ? "XY" : "Z");
    }

    lv_obj_t *left = ui_comp_get_child(ui_controlComponent, UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLA_CONTROLSCREENLEFT);
    lv_obj_t *leftIcon = ui_comp_get_child(ui_controlComponent, UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLA_CONTROLSCREENLEFT_CONTROLSCREENLEFTICON);
    lv_obj_t *right = ui_comp_get_child(ui_controlComponent, UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLC_CONTROLSCREENRIGHT);
    lv_obj_t *rightIcon = ui_comp_get_child(ui_controlComponent, UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLC_CONTROLSCREENRIGHT_CONTROLSCREENRIGHTICON);

    if (controlMode.axis == 0)
    {
        lv_obj_clear_state(left, LV_STATE_DISABLED);
        lv_obj_clear_state(right, LV_STATE_DISABLED);
        lv_obj_clear_state(leftIcon, LV_STATE_DISABLED);
        lv_obj_clear_state(rightIcon, LV_STATE_DISABLED);
        if (s_upHintLabel) lv_label_set_text(s_upHintLabel, "Y+");
        if (s_downHintLabel) lv_label_set_text(s_downHintLabel, "Y-");
        if (s_leftHintLabel) lv_obj_clear_flag(s_leftHintLabel, LV_OBJ_FLAG_HIDDEN);
        if (s_rightHintLabel) lv_obj_clear_flag(s_rightHintLabel, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_state(left, LV_STATE_DISABLED);
        lv_obj_add_state(right, LV_STATE_DISABLED);
        lv_obj_add_state(leftIcon, LV_STATE_DISABLED);
        lv_obj_add_state(rightIcon, LV_STATE_DISABLED);
        if (s_upHintLabel) lv_label_set_text(s_upHintLabel, "Z-");
        if (s_downHintLabel) lv_label_set_text(s_downHintLabel, "Z+");
        if (s_leftHintLabel) lv_obj_add_flag(s_leftHintLabel, LV_OBJ_FLAG_HIDDEN);
        if (s_rightHintLabel) lv_obj_add_flag(s_rightHintLabel, LV_OBJ_FLAG_HIDDEN);
    }
}

static void style_dpad_card(lv_obj_t *btn, bool is_mode_toggle)
{
    lv_obj_set_width(btn, lv_pct(100));
    lv_obj_set_flex_grow(btn, 2);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(btn, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(btn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, is_mode_toggle ? BAMBU_COLOR_BG_ELEVATED() : BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, is_mode_toggle ? BAMBU_COLOR_GREEN() : BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(btn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(btn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(btn, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_set_style_bg_opa(btn, 60, LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_border_opa(btn, 60, LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_text_color(btn, BAMBU_COLOR_TEXT_MUTED(), LV_PART_MAIN | LV_STATE_DISABLED);
}

// COMPONENT controlComponent

lv_obj_t *ui_controlComponent_create(lv_obj_t *comp_parent)
{
    lv_obj_t *cui_controlComponent;
    cui_controlComponent = lv_obj_create(comp_parent);
    lv_obj_set_height(cui_controlComponent, lv_pct(100));
    lv_obj_set_flex_grow(cui_controlComponent, 1);
    lv_obj_set_flex_flow(cui_controlComponent, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_controlComponent, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_controlComponent, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_controlComponent, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_controlComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_controlComponent, BAMBU_COLOR_BG_SCREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_controlComponent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_controlComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_controlComponent, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_controlComponent, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cui_controlComponent, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_controlComponent, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ================= COLUMN A (Left) ================= */
    lv_obj_t *cui_controlScreenColA;
    cui_controlScreenColA = lv_obj_create(cui_controlComponent);
    lv_obj_set_height(cui_controlScreenColA, lv_pct(100));
    lv_obj_set_flex_grow(cui_controlScreenColA, 1);
    lv_obj_set_flex_flow(cui_controlScreenColA, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenColA, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cui_controlScreenColA, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_controlScreenColA, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_controlScreenColA, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_controlScreenColA, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_controlScreenColA, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_controlScreenColA, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Top-Left: Step Size Toggle (1mm / 10mm) */
    lv_obj_t *cui_controlScreenRange = lv_obj_create(cui_controlScreenColA);
    style_dpad_card(cui_controlScreenRange, true);
    lv_obj_set_flex_flow(cui_controlScreenRange, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenRange, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cui_controlScreenRange, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_controlScreenRangeIcon = lv_label_create(cui_controlScreenRange);
    lv_obj_set_width(cui_controlScreenRangeIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_controlScreenRangeIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_controlScreenRangeIcon, "k");
    lv_obj_set_style_text_font(cui_controlScreenRangeIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_controlScreenRangeIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_controlScreenRangeValue = lv_label_create(cui_controlScreenRange);
    lv_obj_set_width(cui_controlScreenRangeValue, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_controlScreenRangeValue, LV_SIZE_CONTENT);
    lv_label_set_text(cui_controlScreenRangeValue, "1mm");
    lv_obj_set_style_text_font(cui_controlScreenRangeValue, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_controlScreenRangeValue, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Middle-Left: X- Left Jog Button */
    lv_obj_t *cui_controlScreenLeft = lv_obj_create(cui_controlScreenColA);
    style_dpad_card(cui_controlScreenLeft, false);
    lv_obj_set_flex_flow(cui_controlScreenLeft, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenLeft, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cui_controlScreenLeft, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_controlScreenLeftIcon = lv_label_create(cui_controlScreenLeft);
    lv_obj_set_width(cui_controlScreenLeftIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_controlScreenLeftIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_controlScreenLeftIcon, "u");
    lv_obj_set_style_text_font(cui_controlScreenLeftIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_controlScreenLeftIcon, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_controlScreenLeftIcon, BAMBU_COLOR_TEXT_MUTED(), LV_PART_MAIN | LV_STATE_DISABLED);

    s_leftHintLabel = lv_label_create(cui_controlScreenLeft);
    lv_label_set_text(s_leftHintLabel, "X-");
    lv_obj_set_style_text_font(s_leftHintLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(s_leftHintLabel, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Bottom-Left Spacer (Clean Cross D-Pad Layout) */
    lv_obj_t *cui_controlScreenDummy0 = lv_obj_create(cui_controlScreenColA);
    lv_obj_set_width(cui_controlScreenDummy0, lv_pct(100));
    lv_obj_set_flex_grow(cui_controlScreenDummy0, 2);
    lv_obj_clear_flag(cui_controlScreenDummy0, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(cui_controlScreenDummy0, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_controlScreenDummy0, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ================= COLUMN B (Center) ================= */
    lv_obj_t *cui_controlScreenColB;
    cui_controlScreenColB = lv_obj_create(cui_controlComponent);
    lv_obj_set_height(cui_controlScreenColB, lv_pct(100));
    lv_obj_set_flex_grow(cui_controlScreenColB, 1);
    lv_obj_set_flex_flow(cui_controlScreenColB, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenColB, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cui_controlScreenColB, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_controlScreenColB, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_controlScreenColB, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_controlScreenColB, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_controlScreenColB, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_controlScreenColB, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Top-Center: Up Jog Button (Y+ / Z-) */
    lv_obj_t *cui_controlScreenUp = lv_obj_create(cui_controlScreenColB);
    style_dpad_card(cui_controlScreenUp, false);
    lv_obj_set_flex_flow(cui_controlScreenUp, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenUp, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cui_controlScreenUp, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_controlScreenUpIcon = lv_label_create(cui_controlScreenUp);
    lv_obj_set_width(cui_controlScreenUpIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_controlScreenUpIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_controlScreenUpIcon, "s");
    lv_obj_set_style_text_font(cui_controlScreenUpIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    s_upHintLabel = lv_label_create(cui_controlScreenUp);
    lv_label_set_text(s_upHintLabel, "Y+");
    lv_obj_set_style_text_font(s_upHintLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(s_upHintLabel, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Middle-Center: Home All Button (Highlighted in Bambu Green) */
    lv_obj_t *cui_controlScreenHome = lv_obj_create(cui_controlScreenColB);
    style_dpad_card(cui_controlScreenHome, false);
    lv_obj_set_flex_flow(cui_controlScreenHome, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenHome, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cui_controlScreenHome, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_controlScreenHome, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_controlScreenHome, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_controlScreenHome, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_controlScreenHome, BAMBU_COLOR_GREEN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_t *cui_controlScreenHomeIcon = lv_label_create(cui_controlScreenHome);
    lv_obj_set_width(cui_controlScreenHomeIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_controlScreenHomeIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_controlScreenHomeIcon, "a");
    lv_obj_set_style_text_font(cui_controlScreenHomeIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *homeHint = lv_label_create(cui_controlScreenHome);
    lv_label_set_text(homeHint, "HOME");
    lv_obj_set_style_text_font(homeHint, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(homeHint, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Bottom-Center: Down Jog Button (Y- / Z+) */
    lv_obj_t *cui_controlScreenDown = lv_obj_create(cui_controlScreenColB);
    style_dpad_card(cui_controlScreenDown, false);
    lv_obj_set_flex_flow(cui_controlScreenDown, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenDown, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cui_controlScreenDown, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    s_downHintLabel = lv_label_create(cui_controlScreenDown);
    lv_label_set_text(s_downHintLabel, "Y-");
    lv_obj_set_style_text_font(s_downHintLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(s_downHintLabel, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_controlScreenDownIcon = lv_label_create(cui_controlScreenDown);
    lv_obj_set_width(cui_controlScreenDownIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_controlScreenDownIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_controlScreenDownIcon, "t");
    lv_obj_set_style_text_font(cui_controlScreenDownIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ================= COLUMN C (Right) ================= */
    lv_obj_t *cui_controlScreenColC;
    cui_controlScreenColC = lv_obj_create(cui_controlComponent);
    lv_obj_set_height(cui_controlScreenColC, lv_pct(100));
    lv_obj_set_flex_grow(cui_controlScreenColC, 1);
    lv_obj_set_flex_flow(cui_controlScreenColC, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenColC, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cui_controlScreenColC, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_controlScreenColC, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_controlScreenColC, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_controlScreenColC, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_controlScreenColC, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_controlScreenColC, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Top-Right: Axis Mode Toggle (XY / Z) */
    lv_obj_t *cui_controlScreenAxixToggle = lv_obj_create(cui_controlScreenColC);
    style_dpad_card(cui_controlScreenAxixToggle, true);
    lv_obj_set_flex_flow(cui_controlScreenAxixToggle, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenAxixToggle, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cui_controlScreenAxixToggle, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_controlScreenAxixToggleIcon = lv_label_create(cui_controlScreenAxixToggle);
    lv_obj_set_width(cui_controlScreenAxixToggleIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_controlScreenAxixToggleIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_controlScreenAxixToggleIcon, "l");
    lv_obj_set_style_text_font(cui_controlScreenAxixToggleIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_controlScreenAxixToggleIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);

    s_axisModeLabel = lv_label_create(cui_controlScreenAxixToggle);
    lv_label_set_text(s_axisModeLabel, "XY");
    lv_obj_set_style_text_font(s_axisModeLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(s_axisModeLabel, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Middle-Right: X+ Right Jog Button */
    lv_obj_t *cui_controlScreenRight = lv_obj_create(cui_controlScreenColC);
    style_dpad_card(cui_controlScreenRight, false);
    lv_obj_set_flex_flow(cui_controlScreenRight, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_controlScreenRight, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cui_controlScreenRight, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_controlScreenRightIcon = lv_label_create(cui_controlScreenRight);
    lv_obj_set_width(cui_controlScreenRightIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_controlScreenRightIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_controlScreenRightIcon, "v");
    lv_obj_set_style_text_font(cui_controlScreenRightIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_controlScreenRightIcon, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_controlScreenRightIcon, BAMBU_COLOR_TEXT_MUTED(), LV_PART_MAIN | LV_STATE_DISABLED);

    s_rightHintLabel = lv_label_create(cui_controlScreenRight);
    lv_label_set_text(s_rightHintLabel, "X+");
    lv_obj_set_style_text_font(s_rightHintLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(s_rightHintLabel, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Bottom-Right Spacer (Clean Cross D-Pad Layout) */
    lv_obj_t *cui_controlScreenDummy1 = lv_obj_create(cui_controlScreenColC);
    lv_obj_set_width(cui_controlScreenDummy1, lv_pct(100));
    lv_obj_set_flex_grow(cui_controlScreenDummy1, 2);
    lv_obj_clear_flag(cui_controlScreenDummy1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(cui_controlScreenDummy1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_controlScreenDummy1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t **children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_CONTROLCOMPONENT_NUM);
    children[UI_COMP_CONTROLCOMPONENT_CONTROLCOMPONENT] = cui_controlComponent;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLA] = cui_controlScreenColA;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLA_CONTROLSCREENRANGE] = cui_controlScreenRange;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLA_CONTROLSCREENRANGE_CONTROLSCREENRANGEICON] = cui_controlScreenRangeIcon;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLA_CONTROLSCREENRANGE_CONTROLSCREENRANGEVALUE] = cui_controlScreenRangeValue;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLA_CONTROLSCREENLEFT] = cui_controlScreenLeft;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLA_CONTROLSCREENLEFT_CONTROLSCREENLEFTICON] = cui_controlScreenLeftIcon;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLA_CONTROLSCREENDUMMY0] = cui_controlScreenDummy0;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLB] = cui_controlScreenColB;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLB_CONTROLSCREENUP] = cui_controlScreenUp;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLB_CONTROLSCREENUP_CONTROLSCREENUPICON] = cui_controlScreenUpIcon;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLB_CONTROLSCREENHOME] = cui_controlScreenHome;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLB_CONTROLSCREENHOME_CONTROLSCREENHOMEICON] = cui_controlScreenHomeIcon;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLB_CONTROLSCREENDOWN] = cui_controlScreenDown;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLB_CONTROLSCREENDOWN_CONTROLSCREENDOWNICON] = cui_controlScreenDownIcon;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLC] = cui_controlScreenColC;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLC_CONTROLSCREENAXIXTOGGLE] = cui_controlScreenAxixToggle;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLC_CONTROLSCREENAXIXTOGGLE_CONTROLSCREENAXIXTOGGLEICON] = cui_controlScreenAxixToggleIcon;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLC_CONTROLSCREENRIGHT] = cui_controlScreenRight;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLC_CONTROLSCREENRIGHT_CONTROLSCREENRIGHTICON] = cui_controlScreenRightIcon;
    children[UI_COMP_CONTROLCOMPONENT_CONTROLSCREENCOLC_CONTROLSCREENDUMMY1] = cui_controlScreenDummy1;
    lv_obj_add_event_cb(cui_controlComponent, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_controlComponent, del_component_child_event_cb, LV_EVENT_DELETE, children);
    lv_obj_add_event_cb(cui_controlScreenRange, ui_event_comp_controlComponent_controlScreenRange, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_controlScreenLeft, ui_event_comp_controlComponent_controlScreenLeft, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_controlScreenUp, ui_event_comp_controlComponent_controlScreenUp, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_controlScreenHome, ui_event_comp_controlComponent_controlScreenHome, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_controlScreenDown, ui_event_comp_controlComponent_controlScreenDown, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_controlScreenAxixToggle, ui_event_comp_controlComponent_controlScreenAxixToggle, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_controlScreenRight, ui_event_comp_controlComponent_controlScreenRight, LV_EVENT_ALL, children);

    lv_obj_add_event_cb(cui_controlScreenRangeValue, onBambuDeckRangeChange, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_CONTROL_INC_SWITCH, cui_controlScreenRangeValue, NULL);

    lv_obj_add_event_cb(cui_controlScreenAxixToggleIcon, onBambuDeckAxisChange, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_CONTROL_AXIS_SWITCH, cui_controlScreenAxixToggleIcon, NULL);

    ui_comp_controlComponent_create_hook(cui_controlComponent);
    return cui_controlComponent;
}
