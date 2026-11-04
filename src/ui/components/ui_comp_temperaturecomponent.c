#include "../ui.h"

lv_obj_t *ui_temperatureComponent_input_target;
bool ui_temperatureComponent_backOrCancel = true;
int ui_temperatureComponent_backToScreen = -1;

void ui_temperatureComponent_show_backOrCancel()
{
    if (ui_temperatureComponent_input_target != NULL)
    {
        lv_obj_t *back = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEYBACK);
        size_t length = strlen(lv_textarea_get_text(ui_temperatureComponent_input_target));
        lv_label_set_text(back, length == 0 ? LV_SYMBOL_CLOSE : LV_SYMBOL_BACKSPACE);
    }
}
void ui_temperatureComponent_on_keypad(char *key)
{
    lv_textarea_add_char(ui_temperatureComponent_input_target, key[0]);
    ui_temperatureComponent_show_backOrCancel();
}
void ui_temperatureComponent_hide_keypad()
{
    lv_obj_t *keyboard = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD);
    lv_obj_t *temps = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS);
    lv_obj_t *fans = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS);
    lv_obj_t *bed = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED);
    lv_obj_t *nozzle = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE);
    lv_obj_t *part = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTPARTFAN);
    lv_obj_t *aux = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTAUXFAN);
    lv_obj_t *chamber = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTCHAMBERFAN);

    if (ui_temperatureComponent_input_target)
    {
        lv_obj_set_style_text_color(ui_temperatureComponent_input_target, controlMode.target_bed_temper != 0 ? BAMBU_COLOR_ORANGE() : BAMBU_COLOR_TEXT_MUTED(), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    }

    lv_obj_clear_flag(temps, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(fans, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_sidebarComponent, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    lv_obj_clear_state(bed, LV_STATE_CHECKED);
    lv_obj_clear_state(nozzle, LV_STATE_CHECKED);
    lv_obj_clear_state(part, LV_STATE_CHECKED);
    lv_obj_clear_state(aux, LV_STATE_CHECKED);
    lv_obj_clear_state(chamber, LV_STATE_CHECKED);

    lv_obj_clear_state(bed, LV_STATE_DISABLED);
    lv_obj_clear_state(nozzle, LV_STATE_DISABLED);
    lv_obj_clear_state(part, LV_STATE_DISABLED);
    lv_obj_clear_state(aux, LV_STATE_DISABLED);
    lv_obj_clear_state(chamber, LV_STATE_DISABLED);
    ui_temperatureComponent_input_target = NULL;

    if (ui_temperatureComponent_backToScreen != -1)
    {
        loadScreen(ui_temperatureComponent_backToScreen);
        ui_temperatureComponent_backToScreen = -1;
    }
}

void ui_temperatureComponent_show_keypad(int type, int index)
{
    lv_obj_t *keyboard = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD);
    lv_obj_t *temps = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS);
    lv_obj_t *fans = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS);
    lv_obj_add_flag(ui_sidebarComponent, LV_OBJ_FLAG_HIDDEN);
    if (type == 0)
    {
        lv_obj_clear_flag(temps, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(fans, LV_OBJ_FLAG_HIDDEN);
        lv_obj_t *bedInput = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED_MAINSCREENBEDTEMPINPUT);
        lv_obj_t *nozzleInput = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE_MAINSCREENNOZZLETEMPINPUT);
        lv_obj_t *bed = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED);
        lv_obj_t *nozzle = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE);
        ui_temperatureComponent_input_target = index == 0 ? bedInput : nozzleInput;
        lv_obj_add_state(index == 0 ? nozzle : bed, LV_STATE_DISABLED);
    }
    else
    {
        lv_obj_add_flag(temps, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(fans, LV_OBJ_FLAG_HIDDEN);

        lv_obj_t *partInput = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTPARTFAN_TEMPERATURECOMPONENTPARTFANINPUT);
        lv_obj_t *auxInput = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTAUXFAN_TEMPERATURECOMPONENTAUXFANINPUT);
        lv_obj_t *chamberInput = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTCHAMBERFAN_TEMPERATURECOMPONENTCHAMBERFANINPUT);
        lv_obj_t *part = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTPARTFAN);
        lv_obj_t *aux = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTAUXFAN);
        lv_obj_t *chamber = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTCHAMBERFAN);

        switch (index)
        {
        case 0:
            ui_temperatureComponent_input_target = partInput;
            lv_obj_add_state(aux, LV_STATE_DISABLED);
            lv_obj_add_state(chamber, LV_STATE_DISABLED);
            break;
        case 1:
            ui_temperatureComponent_input_target = auxInput;
            lv_obj_add_state(part, LV_STATE_DISABLED);
            lv_obj_add_state(chamber, LV_STATE_DISABLED);
            break;
        case 2:
            ui_temperatureComponent_input_target = chamberInput;
            lv_obj_add_state(part, LV_STATE_DISABLED);
            lv_obj_add_state(aux, LV_STATE_DISABLED);
            break;
        }
    }
    lv_textarea_set_text(ui_temperatureComponent_input_target, "");
    lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
}

void ui_temperatureComponent_show_keypad_and_back(int type, int index, int backToScreen)
{
    ui_temperatureComponent_backToScreen = backToScreen;
    ui_temperatureComponent_show_keypad(type, index);
}

void ui_event_comp_temperatureComponent_temperatureComponentBed(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        ui_temperatureComponent_show_keypad(0, 0);
        lv_obj_add_state(target, LV_STATE_CHECKED);
    }
}
void ui_event_comp_temperatureComponent_temperatureComponentNozzle(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        ui_temperatureComponent_show_keypad(0, 1);
        lv_obj_add_state(target, LV_STATE_CHECKED);
    }
}
void ui_event_comp_temperatureComponent_temperatureComponentPartFan(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        ui_temperatureComponent_show_keypad(1, 0);
        lv_obj_add_state(target, LV_STATE_CHECKED);
    }
}
void ui_event_comp_temperatureComponent_temperatureComponentAuxFan(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        ui_temperatureComponent_show_keypad(1, 1);
        lv_obj_add_state(target, LV_STATE_CHECKED);
    }
}
void ui_event_comp_temperatureComponent_temperatureComponentChamberFan(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        ui_temperatureComponent_show_keypad(1, 2);
        lv_obj_add_state(target, LV_STATE_CHECKED);
    }
}

void ui_event_comp_temperatureComponent_numpadKey(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_CLICKED)
    {
        char *text = lv_label_get_text(target);
        ui_temperatureComponent_on_keypad(text);
    }
}
void ui_event_comp_temperatureComponent_numpadKeyBack(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        size_t length = strlen(lv_textarea_get_text(ui_temperatureComponent_input_target));
        if (length == 0)
        {
            ui_temperatureComponent_hide_keypad();
        }
        else
        {
            lv_textarea_set_cursor_pos(ui_temperatureComponent_input_target, LV_TEXTAREA_CURSOR_LAST);
            lv_textarea_del_char(ui_temperatureComponent_input_target);
        }
        ui_temperatureComponent_show_backOrCancel();
    }
}
void ui_event_comp_temperatureComponent_numpadKeyOk(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        lv_obj_t *bed = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED_MAINSCREENBEDTEMPINPUT);
        lv_obj_t *nozzle = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE_MAINSCREENNOZZLETEMPINPUT);
        lv_obj_t *part = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTPARTFAN_TEMPERATURECOMPONENTPARTFANINPUT);
        lv_obj_t *aux = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTAUXFAN_TEMPERATURECOMPONENTAUXFANINPUT);
        lv_obj_t *chamber = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTCHAMBERFAN_TEMPERATURECOMPONENTCHAMBERFANINPUT);
        if (ui_temperatureComponent_input_target == bed)
        {
            const char *data = lv_textarea_get_text(bed);
            controlMode.target_bed_temper = atoi(data);
            lv_msg_send(BAMBUDECK_COMMAND_BED_TARGET_TEMP, NULL);
            lv_textarea_set_text(bed, "");
        }
        else if (ui_temperatureComponent_input_target == nozzle)
        {
            const char *data = lv_textarea_get_text(nozzle);
            controlMode.target_nozzle_temper = atoi(data);
            lv_msg_send(BAMBUDECK_COMMAND_NOZZLE_TARGET_TEMP, NULL);
            lv_textarea_set_text(nozzle, "");
        }
        else if (ui_temperatureComponent_input_target == part)
        {
            const char *data = lv_textarea_get_text(part);
            bambuStatus.cooling_fan_speed = atoi(data) * 255 / 100;
            lv_msg_send(BAMBUDECK_COMMAND_PART_FAN_SPEED, NULL);
            lv_textarea_set_text(part, "");
        }
        else if (ui_temperatureComponent_input_target == aux)
        {
            const char *data = lv_textarea_get_text(aux);
            bambuStatus.big_fan1_speed = atoi(data) * 255 / 100;
            lv_msg_send(BAMBUDECK_COMMAND_AUX_FAN_SPEED, NULL);
            lv_textarea_set_text(aux, "");
        }
        else if (ui_temperatureComponent_input_target == chamber)
        {
            const char *data = lv_textarea_get_text(chamber);
            bambuStatus.big_fan2_speed = atoi(data) * 255 / 100;
            lv_msg_send(BAMBUDECK_COMMAND_CHAMBER_FAN_SPEED, NULL);
            lv_textarea_set_text(chamber, "");
        }

        ui_temperatureComponent_hide_keypad();
    }
}

void ui_temperatureComponent_onBambuDeckTemp(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;

    char value[10];
    itoa(message->data, value, 10);
    lv_label_set_text(target, value);
}

void ui_temperatureComponent_onBambuDeckTempTarget(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;
    lv_obj_set_style_text_color(target, message->data > 0 ? BAMBU_COLOR_ORANGE() : BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t *inputTarget;
    if (target == ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED_MAINSCREENBEDTEMPICON))
    {
        inputTarget = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED_MAINSCREENBEDTEMPINPUT);
    }
    else
    {
        inputTarget = ui_comp_get_child(ui_temperatureComponent, UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE_MAINSCREENNOZZLETEMPINPUT);
    }

    char value[12];
    itoa(message->data, value, 10);
    lv_textarea_set_placeholder_text(inputTarget, value);
    lv_obj_set_style_text_color(inputTarget, message->data > 0 ? BAMBU_COLOR_ORANGE() : BAMBU_COLOR_TEXT_MUTED(), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
}

void ui_temperatureComponent_onBambuDeckPartFanSpeed(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;
    char value[12];

    itoa(round(message->data * 100 / 255.), value, 10);
    lv_textarea_set_text(target, value);
}

void ui_temperatureComponent_onBambuDeckAuxFanSpeed(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;
    char value[12];

    itoa(round(message->data * 100 / 255.), value, 10);
    lv_textarea_set_text(target, value);
}

void ui_temperatureComponent_onBambuDeckChamberFanSpeed(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);
    struct BAMBUDECK_MESSAGE_DATA *message = (struct BAMBUDECK_MESSAGE_DATA *)m->payload;
    char value[12];

    itoa(round(message->data * 100 / 255.), value, 10);
    lv_textarea_set_text(target, value);
}

static void style_temp_or_fan_card(lv_obj_t *card)
{
    lv_obj_set_scrollbar_mode(card, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(card, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(card, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(card, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(card, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(card, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(card, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Checked (Active keypad editing) state */
    lv_obj_set_style_bg_color(card, BAMBU_COLOR_BG_GREEN_TINT(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(card, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_opa(card, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(card, 2, LV_PART_MAIN | LV_STATE_CHECKED);

    /* Pressed state */
    lv_obj_set_style_bg_color(card, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(card, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(card, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
}

static lv_obj_t *create_numpad_key(lv_obj_t *parent, const char *label_txt, bool new_row)
{
    lv_obj_t *key = lv_label_create(parent);
    lv_obj_set_height(key, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(key, 1);
    lv_label_set_text(key, label_txt);
    lv_obj_add_flag(key, LV_OBJ_FLAG_CLICKABLE | (new_row ? LV_OBJ_FLAG_FLEX_IN_NEW_TRACK : 0));
    lv_obj_clear_flag(key, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(key, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_align(key, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(key, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(key, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(key, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(key, BAMBU_COLOR_BG_CARD(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(key, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(key, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(key, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(key, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(key, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(key, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(key, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(key, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(key, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(key, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    return key;
}

// COMPONENT temperatureComponent

lv_obj_t *ui_temperatureComponent_create(lv_obj_t *comp_parent)
{
    lv_obj_t *cui_temperatureComponent;
    cui_temperatureComponent = lv_obj_create(comp_parent);
    lv_obj_set_height(cui_temperatureComponent, lv_pct(100));
    lv_obj_set_flex_grow(cui_temperatureComponent, 1);
    lv_obj_set_flex_flow(cui_temperatureComponent, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cui_temperatureComponent, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_temperatureComponent, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_temperatureComponent, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_temperatureComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_temperatureComponent, BAMBU_COLOR_BG_SCREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_temperatureComponent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_temperatureComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_temperatureComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_temperatureComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cui_temperatureComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_temperatureComponent, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_temperatureComponent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentTemps;
    cui_temperatureComponentTemps = lv_obj_create(cui_temperatureComponent);
    lv_obj_set_height(cui_temperatureComponentTemps, lv_pct(100));
    lv_obj_set_flex_grow(cui_temperatureComponentTemps, 3);
    lv_obj_set_flex_flow(cui_temperatureComponentTemps, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_temperatureComponentTemps, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cui_temperatureComponentTemps, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_temperatureComponentTemps, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_temperatureComponentTemps, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_temperatureComponentTemps, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_temperatureComponentTemps, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_temperatureComponentTemps, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_temperatureComponentTemps, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentNozzle;
    cui_temperatureComponentNozzle = lv_obj_create(cui_temperatureComponentTemps);
    lv_obj_set_width(cui_temperatureComponentNozzle, lv_pct(100));
    lv_obj_set_flex_grow(cui_temperatureComponentNozzle, 2);
    lv_obj_set_flex_flow(cui_temperatureComponentNozzle, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_temperatureComponentNozzle, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_temperatureComponentNozzle, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    style_temp_or_fan_card(cui_temperatureComponentNozzle);
    lv_obj_set_style_pad_all(cui_temperatureComponentNozzle, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_temperatureComponentNozzle, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenNozzleTempIcon;
    cui_mainScreenNozzleTempIcon = lv_label_create(cui_temperatureComponentNozzle);
    lv_obj_set_width(cui_mainScreenNozzleTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenNozzleTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenNozzleTempIcon, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenNozzleTempIcon, "f");
    lv_obj_set_style_text_color(cui_mainScreenNozzleTempIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_mainScreenNozzleTempIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenNozzleTempValue;
    cui_mainScreenNozzleTempValue = lv_label_create(cui_temperatureComponentNozzle);
    lv_obj_set_width(cui_mainScreenNozzleTempValue, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenNozzleTempValue, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenNozzleTempValue, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenNozzleTempValue, "--");
    lv_obj_set_style_text_font(cui_mainScreenNozzleTempValue, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenNozzleTempInput;
    cui_mainScreenNozzleTempInput = lv_textarea_create(cui_temperatureComponentNozzle);
    lv_obj_set_width(cui_mainScreenNozzleTempInput, lv_pct(100));
    lv_obj_set_height(cui_mainScreenNozzleTempInput, LV_SIZE_CONTENT);
    lv_textarea_set_max_length(cui_mainScreenNozzleTempInput, 3);
    lv_textarea_set_placeholder_text(cui_mainScreenNozzleTempInput, "0");
    lv_textarea_set_one_line(cui_mainScreenNozzleTempInput, true);
    lv_obj_clear_flag(cui_mainScreenNozzleTempInput, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenNozzleTempInput, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_color(cui_mainScreenNozzleTempInput, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenNozzleTempInput, BAMBU_COLOR_TEXT_MUTED(), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(cui_mainScreenNozzleTempInput, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_mainScreenNozzleTempInput, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenNozzleTempInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenNozzleTempInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenNozzleTempInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentBed;
    cui_temperatureComponentBed = lv_obj_create(cui_temperatureComponentTemps);
    lv_obj_set_width(cui_temperatureComponentBed, lv_pct(100));
    lv_obj_set_flex_grow(cui_temperatureComponentBed, 2);
    lv_obj_set_flex_flow(cui_temperatureComponentBed, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_temperatureComponentBed, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_temperatureComponentBed, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    style_temp_or_fan_card(cui_temperatureComponentBed);
    lv_obj_set_style_pad_all(cui_temperatureComponentBed, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_temperatureComponentBed, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenBedTempIcon;
    cui_mainScreenBedTempIcon = lv_label_create(cui_temperatureComponentBed);
    lv_obj_set_width(cui_mainScreenBedTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenBedTempIcon, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenBedTempIcon, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenBedTempIcon, "e");
    lv_obj_set_style_text_color(cui_mainScreenBedTempIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_mainScreenBedTempIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenBedTempValue;
    cui_mainScreenBedTempValue = lv_label_create(cui_temperatureComponentBed);
    lv_obj_set_width(cui_mainScreenBedTempValue, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_mainScreenBedTempValue, LV_SIZE_CONTENT);
    lv_obj_set_align(cui_mainScreenBedTempValue, LV_ALIGN_CENTER);
    lv_label_set_text(cui_mainScreenBedTempValue, "--");
    lv_obj_set_style_text_font(cui_mainScreenBedTempValue, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_mainScreenBedTempInput;
    cui_mainScreenBedTempInput = lv_textarea_create(cui_temperatureComponentBed);
    lv_obj_set_width(cui_mainScreenBedTempInput, lv_pct(100));
    lv_obj_set_height(cui_mainScreenBedTempInput, LV_SIZE_CONTENT);
    lv_textarea_set_max_length(cui_mainScreenBedTempInput, 3);
    lv_textarea_set_placeholder_text(cui_mainScreenBedTempInput, "0");
    lv_textarea_set_one_line(cui_mainScreenBedTempInput, true);
    lv_obj_clear_flag(cui_mainScreenBedTempInput, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_mainScreenBedTempInput, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_color(cui_mainScreenBedTempInput, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_mainScreenBedTempInput, BAMBU_COLOR_TEXT_MUTED(), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(cui_mainScreenBedTempInput, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_mainScreenBedTempInput, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_mainScreenBedTempInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_mainScreenBedTempInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_mainScreenBedTempInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentFans;
    cui_temperatureComponentFans = lv_obj_create(cui_temperatureComponent);
    lv_obj_set_height(cui_temperatureComponentFans, lv_pct(100));
    lv_obj_set_flex_grow(cui_temperatureComponentFans, 3);
    lv_obj_set_flex_flow(cui_temperatureComponentFans, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_temperatureComponentFans, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cui_temperatureComponentFans, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_temperatureComponentFans, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_temperatureComponentFans, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_temperatureComponentFans, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_temperatureComponentFans, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_temperatureComponentFans, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_temperatureComponentFans, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* --- Part Fan Card --- */
    lv_obj_t *cui_temperatureComponentPartFan;
    cui_temperatureComponentPartFan = lv_obj_create(cui_temperatureComponentFans);
    lv_obj_set_width(cui_temperatureComponentPartFan, lv_pct(100));
    lv_obj_set_flex_grow(cui_temperatureComponentPartFan, 1);
    lv_obj_set_flex_flow(cui_temperatureComponentPartFan, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cui_temperatureComponentPartFan, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_temperatureComponentPartFan, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    style_temp_or_fan_card(cui_temperatureComponentPartFan);
    lv_obj_set_style_pad_all(cui_temperatureComponentPartFan, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentPartFanIcon;
    cui_temperatureComponentPartFanIcon = lv_label_create(cui_temperatureComponentPartFan);
    lv_obj_set_width(cui_temperatureComponentPartFanIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_temperatureComponentPartFanIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_temperatureComponentPartFanIcon, "i");
    lv_obj_set_style_text_color(cui_temperatureComponentPartFanIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentPartFanIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentPartFanLabel;
    cui_temperatureComponentPartFanLabel = lv_label_create(cui_temperatureComponentPartFan);
    lv_obj_set_height(cui_temperatureComponentPartFanLabel, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cui_temperatureComponentPartFanLabel, 1);
    lv_label_set_text(cui_temperatureComponentPartFanLabel, "PART");
    lv_obj_set_style_text_align(cui_temperatureComponentPartFanLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentPartFanLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_temperatureComponentPartFanLabel, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentPartFanInput;
    cui_temperatureComponentPartFanInput = lv_textarea_create(cui_temperatureComponentPartFan);
    lv_obj_set_width(cui_temperatureComponentPartFanInput, 68);
    lv_obj_set_height(cui_temperatureComponentPartFanInput, LV_SIZE_CONTENT);
    lv_textarea_set_max_length(cui_temperatureComponentPartFanInput, 3);
    lv_textarea_set_text(cui_temperatureComponentPartFanInput, "0");
    lv_textarea_set_placeholder_text(cui_temperatureComponentPartFanInput, "0");
    lv_textarea_set_one_line(cui_temperatureComponentPartFanInput, true);
    lv_obj_add_flag(cui_temperatureComponentPartFanInput, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_clear_flag(cui_temperatureComponentPartFanInput, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_temperatureComponentPartFanInput, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_color(cui_temperatureComponentPartFanInput, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(cui_temperatureComponentPartFanInput, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentPartFanInput, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_temperatureComponentPartFanInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_temperatureComponentPartFanInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_temperatureComponentPartFanInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentPartFanValue;
    cui_temperatureComponentPartFanValue = lv_label_create(cui_temperatureComponentPartFan);
    lv_obj_set_width(cui_temperatureComponentPartFanValue, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_temperatureComponentPartFanValue, LV_SIZE_CONTENT);
    lv_label_set_text(cui_temperatureComponentPartFanValue, "%");
    lv_obj_set_style_text_align(cui_temperatureComponentPartFanValue, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentPartFanValue, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_temperatureComponentPartFanValue, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* --- Aux Fan Card --- */
    lv_obj_t *cui_temperatureComponentAuxFan;
    cui_temperatureComponentAuxFan = lv_obj_create(cui_temperatureComponentFans);
    lv_obj_set_width(cui_temperatureComponentAuxFan, lv_pct(100));
    lv_obj_set_flex_grow(cui_temperatureComponentAuxFan, 1);
    lv_obj_set_flex_flow(cui_temperatureComponentAuxFan, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cui_temperatureComponentAuxFan, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_temperatureComponentAuxFan, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    style_temp_or_fan_card(cui_temperatureComponentAuxFan);
    lv_obj_set_style_pad_all(cui_temperatureComponentAuxFan, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

    if ((bambudeck_bblp_is_p1p() || bambudeck_bblp_is_x1()) && !bambuDeckConfig.auxFanEnabled)
    {
        lv_obj_add_flag(cui_temperatureComponentAuxFan, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_t *cui_temperatureComponentAuxFanIcon;
    cui_temperatureComponentAuxFanIcon = lv_label_create(cui_temperatureComponentAuxFan);
    lv_obj_set_width(cui_temperatureComponentAuxFanIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_temperatureComponentAuxFanIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_temperatureComponentAuxFanIcon, "i");
    lv_obj_set_style_text_color(cui_temperatureComponentAuxFanIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentAuxFanIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentAuxFanLabel;
    cui_temperatureComponentAuxFanLabel = lv_label_create(cui_temperatureComponentAuxFan);
    lv_obj_set_height(cui_temperatureComponentAuxFanLabel, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cui_temperatureComponentAuxFanLabel, 1);
    lv_label_set_text(cui_temperatureComponentAuxFanLabel, "AUX");
    lv_obj_set_style_text_align(cui_temperatureComponentAuxFanLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentAuxFanLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_temperatureComponentAuxFanLabel, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentAuxFanInput;
    cui_temperatureComponentAuxFanInput = lv_textarea_create(cui_temperatureComponentAuxFan);
    lv_obj_set_width(cui_temperatureComponentAuxFanInput, 68);
    lv_obj_set_height(cui_temperatureComponentAuxFanInput, LV_SIZE_CONTENT);
    lv_textarea_set_max_length(cui_temperatureComponentAuxFanInput, 3);
    lv_textarea_set_text(cui_temperatureComponentAuxFanInput, "0");
    lv_textarea_set_placeholder_text(cui_temperatureComponentAuxFanInput, "0");
    lv_textarea_set_one_line(cui_temperatureComponentAuxFanInput, true);
    lv_obj_add_flag(cui_temperatureComponentAuxFanInput, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_clear_flag(cui_temperatureComponentAuxFanInput, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_temperatureComponentAuxFanInput, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_color(cui_temperatureComponentAuxFanInput, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(cui_temperatureComponentAuxFanInput, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentAuxFanInput, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_temperatureComponentAuxFanInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_temperatureComponentAuxFanInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_temperatureComponentAuxFanInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentAuxFanValue;
    cui_temperatureComponentAuxFanValue = lv_label_create(cui_temperatureComponentAuxFan);
    lv_obj_set_width(cui_temperatureComponentAuxFanValue, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_temperatureComponentAuxFanValue, LV_SIZE_CONTENT);
    lv_label_set_text(cui_temperatureComponentAuxFanValue, "%");
    lv_obj_set_style_text_align(cui_temperatureComponentAuxFanValue, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentAuxFanValue, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_temperatureComponentAuxFanValue, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* --- Chamber Fan Card --- */
    lv_obj_t *cui_temperatureComponentChamberFan;
    cui_temperatureComponentChamberFan = lv_obj_create(cui_temperatureComponentFans);
    lv_obj_set_width(cui_temperatureComponentChamberFan, lv_pct(100));
    lv_obj_set_flex_grow(cui_temperatureComponentChamberFan, 1);
    lv_obj_set_flex_flow(cui_temperatureComponentChamberFan, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cui_temperatureComponentChamberFan, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_temperatureComponentChamberFan, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    style_temp_or_fan_card(cui_temperatureComponentChamberFan);
    lv_obj_set_style_pad_all(cui_temperatureComponentChamberFan, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

    if ((bambudeck_bblp_is_p1p()) && !bambuDeckConfig.chamberFanEnabled)
    {
        lv_obj_add_flag(cui_temperatureComponentChamberFan, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_t *cui_temperatureComponentChamberFanIcon;
    cui_temperatureComponentChamberFanIcon = lv_label_create(cui_temperatureComponentChamberFan);
    lv_obj_set_width(cui_temperatureComponentChamberFanIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_temperatureComponentChamberFanIcon, LV_SIZE_CONTENT);
    lv_label_set_text(cui_temperatureComponentChamberFanIcon, "i");
    lv_obj_set_style_text_color(cui_temperatureComponentChamberFanIcon, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentChamberFanIcon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentChamberFanLabel;
    cui_temperatureComponentChamberFanLabel = lv_label_create(cui_temperatureComponentChamberFan);
    lv_obj_set_height(cui_temperatureComponentChamberFanLabel, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cui_temperatureComponentChamberFanLabel, 1);
    lv_label_set_text(cui_temperatureComponentChamberFanLabel, "CHAMBER");
    lv_obj_set_style_text_align(cui_temperatureComponentChamberFanLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentChamberFanLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_temperatureComponentChamberFanLabel, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentChamberFanInput;
    cui_temperatureComponentChamberFanInput = lv_textarea_create(cui_temperatureComponentChamberFan);
    lv_obj_set_width(cui_temperatureComponentChamberFanInput, 68);
    lv_obj_set_height(cui_temperatureComponentChamberFanInput, LV_SIZE_CONTENT);
    lv_textarea_set_max_length(cui_temperatureComponentChamberFanInput, 3);
    lv_textarea_set_text(cui_temperatureComponentChamberFanInput, "0");
    lv_textarea_set_placeholder_text(cui_temperatureComponentChamberFanInput, "0");
    lv_textarea_set_one_line(cui_temperatureComponentChamberFanInput, true);
    lv_obj_add_flag(cui_temperatureComponentChamberFanInput, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_clear_flag(cui_temperatureComponentChamberFanInput, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_temperatureComponentChamberFanInput, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_color(cui_temperatureComponentChamberFanInput, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(cui_temperatureComponentChamberFanInput, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentChamberFanInput, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_temperatureComponentChamberFanInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_temperatureComponentChamberFanInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cui_temperatureComponentChamberFanInput, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_temperatureComponentChamberFanValue;
    cui_temperatureComponentChamberFanValue = lv_label_create(cui_temperatureComponentChamberFan);
    lv_obj_set_width(cui_temperatureComponentChamberFanValue, LV_SIZE_CONTENT);
    lv_obj_set_height(cui_temperatureComponentChamberFanValue, LV_SIZE_CONTENT);
    lv_label_set_text(cui_temperatureComponentChamberFanValue, "%");
    lv_obj_set_style_text_align(cui_temperatureComponentChamberFanValue, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_temperatureComponentChamberFanValue, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_temperatureComponentChamberFanValue, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* --- Numpad Keyboard --- */
    lv_obj_t *cui_temperatureComponentKeyboard;
    cui_temperatureComponentKeyboard = lv_obj_create(cui_temperatureComponent);
    lv_obj_set_height(cui_temperatureComponentKeyboard, lv_pct(100));
    lv_obj_set_flex_grow(cui_temperatureComponentKeyboard, 7);
    lv_obj_set_flex_flow(cui_temperatureComponentKeyboard, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cui_temperatureComponentKeyboard, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(cui_temperatureComponentKeyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(cui_temperatureComponentKeyboard, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_temperatureComponentKeyboard, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cui_temperatureComponentKeyboard, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_temperatureComponentKeyboard, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_temperatureComponentKeyboard, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_temperatureComponentKeyboard, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_temperatureComponentKeyboard, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_temperatureComponentKeyboard, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_temperatureComponentKeyboard, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cui_temperatureComponentKeyboard, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_numpadInput;
    cui_numpadInput = lv_textarea_create(cui_temperatureComponentKeyboard);
    lv_obj_set_width(cui_numpadInput, 150);
    lv_obj_set_height(cui_numpadInput, LV_SIZE_CONTENT);
    lv_textarea_set_max_length(cui_numpadInput, 3);
    lv_textarea_set_one_line(cui_numpadInput, true);
    lv_obj_add_flag(cui_numpadInput, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *cui_numpadKey1 = create_numpad_key(cui_temperatureComponentKeyboard, "1", false);
    lv_obj_t *cui_numpadKey2 = create_numpad_key(cui_temperatureComponentKeyboard, "2", false);
    lv_obj_t *cui_numpadKey3 = create_numpad_key(cui_temperatureComponentKeyboard, "3", false);
    lv_obj_t *cui_numpadKey4 = create_numpad_key(cui_temperatureComponentKeyboard, "4", true);
    lv_obj_t *cui_numpadKey5 = create_numpad_key(cui_temperatureComponentKeyboard, "5", false);
    lv_obj_t *cui_numpadKey6 = create_numpad_key(cui_temperatureComponentKeyboard, "6", false);
    lv_obj_t *cui_numpadKey7 = create_numpad_key(cui_temperatureComponentKeyboard, "7", true);
    lv_obj_t *cui_numpadKey8 = create_numpad_key(cui_temperatureComponentKeyboard, "8", false);
    lv_obj_t *cui_numpadKey9 = create_numpad_key(cui_temperatureComponentKeyboard, "9", false);
    lv_obj_t *cui_numpadKey0 = create_numpad_key(cui_temperatureComponentKeyboard, "0", true);

    lv_obj_t *cui_numpadKeyBack = create_numpad_key(cui_temperatureComponentKeyboard, LV_SYMBOL_CLOSE, false);
    lv_obj_set_style_bg_color(cui_numpadKeyBack, BAMBU_COLOR_BG_ELEVATED(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_numpadKeyBack, BAMBU_COLOR_RED(), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_t *cui_numpadKeyOk = create_numpad_key(cui_temperatureComponentKeyboard, LV_SYMBOL_OK, false);
    lv_obj_set_style_bg_color(cui_numpadKeyOk, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_numpadKeyOk, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_numpadKeyOk, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_numpadKeyOk, BAMBU_COLOR_GREEN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_t **children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_TEMPERATURECOMPONENT_NUM);
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENT] = cui_temperatureComponent;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS] = cui_temperatureComponentTemps;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE] = cui_temperatureComponentNozzle;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE_MAINSCREENNOZZLETEMPICON] = cui_mainScreenNozzleTempIcon;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE_MAINSCREENNOZZLETEMPVALUE] = cui_mainScreenNozzleTempValue;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTNOZZLE_MAINSCREENNOZZLETEMPINPUT] = cui_mainScreenNozzleTempInput;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED] = cui_temperatureComponentBed;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED_MAINSCREENBEDTEMPICON] = cui_mainScreenBedTempIcon;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED_MAINSCREENBEDTEMPVALUE] = cui_mainScreenBedTempValue;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTTEMPS_TEMPERATURECOMPONENTBED_MAINSCREENBEDTEMPINPUT] = cui_mainScreenBedTempInput;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS] = cui_temperatureComponentFans;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTPARTFAN] = cui_temperatureComponentPartFan;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTPARTFAN_TEMPERATURECOMPONENTPARTFANICON] = cui_temperatureComponentPartFanIcon;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTPARTFAN_TEMPERATURECOMPONENTPARTFANLABEL] = cui_temperatureComponentPartFanLabel;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTPARTFAN_TEMPERATURECOMPONENTPARTFANVALUE] = cui_temperatureComponentPartFanValue;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTPARTFAN_TEMPERATURECOMPONENTPARTFANINPUT] = cui_temperatureComponentPartFanInput;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTAUXFAN] = cui_temperatureComponentAuxFan;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTAUXFAN_TEMPERATURECOMPONENTAUXFANICON] = cui_temperatureComponentAuxFanIcon;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTAUXFAN_TEMPERATURECOMPONENTAUXFANLABEL] = cui_temperatureComponentAuxFanLabel;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTAUXFAN_TEMPERATURECOMPONENTAUXFANVALUE] = cui_temperatureComponentAuxFanValue;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTAUXFAN_TEMPERATURECOMPONENTAUXFANINPUT] = cui_temperatureComponentAuxFanInput;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTCHAMBERFAN] = cui_temperatureComponentChamberFan;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTCHAMBERFAN_TEMPERATURECOMPONENTCHAMBERFANICON] = cui_temperatureComponentChamberFanIcon;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTCHAMBERFAN_TEMPERATURECOMPONENTCHAMBERFANLABEL] = cui_temperatureComponentChamberFanLabel;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTCHAMBERFAN_TEMPERATURECOMPONENTCHAMBERFANVALUE] = cui_temperatureComponentChamberFanValue;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTFANS_TEMPERATURECOMPONENTCHAMBERFAN_TEMPERATURECOMPONENTCHAMBERFANINPUT] = cui_temperatureComponentChamberFanInput;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD] = cui_temperatureComponentKeyboard;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADINPUT] = cui_numpadInput;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY1] = cui_numpadKey1;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY2] = cui_numpadKey2;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY3] = cui_numpadKey3;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY4] = cui_numpadKey4;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY5] = cui_numpadKey5;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY6] = cui_numpadKey6;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY7] = cui_numpadKey7;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY8] = cui_numpadKey8;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY9] = cui_numpadKey9;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEY0] = cui_numpadKey0;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEYBACK] = cui_numpadKeyBack;
    children[UI_COMP_TEMPERATURECOMPONENT_TEMPERATURECOMPONENTKEYBOARD_NUMPADKEYOK] = cui_numpadKeyOk;
    lv_obj_add_event_cb(cui_temperatureComponent, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_temperatureComponent, del_component_child_event_cb, LV_EVENT_DELETE, children);
    lv_obj_add_event_cb(cui_temperatureComponentBed, ui_event_comp_temperatureComponent_temperatureComponentBed, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_temperatureComponentNozzle, ui_event_comp_temperatureComponent_temperatureComponentNozzle, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_temperatureComponentPartFan, ui_event_comp_temperatureComponent_temperatureComponentPartFan, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_temperatureComponentAuxFan, ui_event_comp_temperatureComponent_temperatureComponentAuxFan, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_temperatureComponentChamberFan, ui_event_comp_temperatureComponent_temperatureComponentChamberFan, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey1, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey2, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey3, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey4, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey5, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey6, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey7, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey8, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey9, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKey0, ui_event_comp_temperatureComponent_numpadKey, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKeyBack, ui_event_comp_temperatureComponent_numpadKeyBack, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_numpadKeyOk, ui_event_comp_temperatureComponent_numpadKeyOk, LV_EVENT_ALL, children);

    lv_obj_add_event_cb(cui_mainScreenBedTempValue, ui_temperatureComponent_onBambuDeckTemp, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_BED_TEMP, cui_mainScreenBedTempValue, NULL);

    lv_obj_add_event_cb(cui_mainScreenBedTempIcon, ui_temperatureComponent_onBambuDeckTempTarget, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_BED_TARGET_TEMP, cui_mainScreenBedTempIcon, NULL);

    lv_obj_add_event_cb(cui_mainScreenNozzleTempValue, ui_temperatureComponent_onBambuDeckTemp, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_NOZZLE_TEMP, cui_mainScreenNozzleTempValue, NULL);

    lv_obj_add_event_cb(cui_mainScreenNozzleTempIcon, ui_temperatureComponent_onBambuDeckTempTarget, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_NOZZLE_TARGET_TEMP, cui_mainScreenNozzleTempIcon, NULL);

    lv_obj_add_event_cb(cui_temperatureComponentPartFanInput, ui_temperatureComponent_onBambuDeckPartFanSpeed, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_PART_FAN_SPEED, cui_temperatureComponentPartFanInput, NULL);

    lv_obj_add_event_cb(cui_temperatureComponentAuxFanInput, ui_temperatureComponent_onBambuDeckAuxFanSpeed, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_PART_AUX_SPEED, cui_temperatureComponentAuxFanInput, NULL);

    lv_obj_add_event_cb(cui_temperatureComponentChamberFanInput, ui_temperatureComponent_onBambuDeckChamberFanSpeed, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(BAMBUDECK_ON_PART_CHAMBER_SPEED, cui_temperatureComponentChamberFanInput, NULL);

    ui_comp_temperatureComponent_create_hook(cui_temperatureComponent);

    return cui_temperatureComponent;
}
