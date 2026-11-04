#include "../ui.h"

void ui_event_comp_sidebarComponent_sidebarHomeButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onSidebarHome(e);
    }
}
void ui_event_comp_sidebarComponent_sidebarTempButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onSidebarTemp(e);
    }
}
void ui_event_comp_sidebarComponent_sidebarControlButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onSidebarControl(e);
    }
}
void ui_event_comp_sidebarComponent_sidebarNozzleButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onSidebarNozzle(e);
    }
}
void ui_event_comp_sidebarComponent_sidebarSettingsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        onSidebarSettings(e);
    }
}

void ui_sidebarComponent_set_active(int index)
{
    lv_obj_t *target;
    uint32_t indexes[5] = {
        UI_COMP_SIDEBARCOMPONENT_SIDEBARHOMEBUTTON,
        UI_COMP_SIDEBARCOMPONENT_SIDEBARTEMPBUTTON,
        UI_COMP_SIDEBARCOMPONENT_SIDEBARCONTROLBUTTON,
        UI_COMP_SIDEBARCOMPONENT_SIDEBARNOZZLEBUTTON,
        UI_COMP_SIDEBARCOMPONENT_SIDEBARSETTINGSBUTTON};

    for (int i = 0; i < 5; i++)
    {
        target = ui_comp_get_child(ui_sidebarComponent, indexes[i]);
        lv_obj_clear_state(target, LV_STATE_CHECKED);
    }
    uint32_t targetIndex = indexes[index];
    lv_obj_add_state(ui_comp_get_child(ui_sidebarComponent, targetIndex), LV_STATE_CHECKED);
}

static void style_sidebar_btn(lv_obj_t *btn)
{
    lv_obj_set_width(btn, lv_pct(100));
    lv_obj_set_flex_grow(btn, 1);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(btn, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(btn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(btn, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(btn, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn, BAMBU_COLOR_TEXT_SECONDARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Active (Checked) state: Bambu Green pill + crisp white icon */
    lv_obj_set_style_bg_color(btn, BAMBU_COLOR_GREEN(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(btn, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(btn, 255, LV_PART_MAIN | LV_STATE_CHECKED);

    /* Pressed state */
    lv_obj_set_style_bg_color(btn, BAMBU_COLOR_GREEN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(btn, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
}

static lv_obj_t *create_sidebar_icon(lv_obj_t *parent_btn, const char *glyph)
{
    lv_obj_t *icon = lv_label_create(parent_btn);
    lv_obj_set_width(icon, LV_SIZE_CONTENT);
    lv_obj_set_height(icon, LV_SIZE_CONTENT);
    lv_label_set_text(icon, glyph);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(icon, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_font(icon, &ui_font_bambudeck, LV_PART_MAIN | LV_STATE_DEFAULT);
    return icon;
}

// COMPONENT sidebarComponent

lv_obj_t *ui_sidebarComponent_create(lv_obj_t *comp_parent)
{
    lv_obj_t *cui_sidebarComponent;
    cui_sidebarComponent = lv_obj_create(comp_parent);
    lv_obj_set_width(cui_sidebarComponent, 48);
    lv_obj_set_height(cui_sidebarComponent, lv_pct(100));
    lv_obj_set_flex_flow(cui_sidebarComponent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cui_sidebarComponent, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cui_sidebarComponent, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(cui_sidebarComponent, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(cui_sidebarComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_sidebarComponent, BAMBU_COLOR_BG_SIDEBAR(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_sidebarComponent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cui_sidebarComponent, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(cui_sidebarComponent, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_sidebarComponent, BAMBU_COLOR_BORDER(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_sidebarComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_sidebarComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_sidebarComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_sidebarComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cui_sidebarComponent, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cui_sidebarComponent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_sidebarComponent, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_sidebarComponent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_sidebarHomeButton = lv_obj_create(cui_sidebarComponent);
    style_sidebar_btn(cui_sidebarHomeButton);
    lv_obj_t *cui_sidebarHomeButtonIcon = create_sidebar_icon(cui_sidebarHomeButton, "a");

    lv_obj_t *cui_sidebarTempButton = lv_obj_create(cui_sidebarComponent);
    style_sidebar_btn(cui_sidebarTempButton);
    lv_obj_t *cui_sidebarTempButtonIcon = create_sidebar_icon(cui_sidebarTempButton, "b");

    lv_obj_t *cui_sidebarControlButton = lv_obj_create(cui_sidebarComponent);
    style_sidebar_btn(cui_sidebarControlButton);
    lv_obj_t *cui_sidebarControlButtonIcon = create_sidebar_icon(cui_sidebarControlButton, "c");

    lv_obj_t *cui_sidebarNozzleButton = lv_obj_create(cui_sidebarComponent);
    style_sidebar_btn(cui_sidebarNozzleButton);
    lv_obj_t *cui_sidebarNozzleButtonIcon = create_sidebar_icon(cui_sidebarNozzleButton, "n");

    lv_obj_t *cui_sidebarSettingsButton = lv_obj_create(cui_sidebarComponent);
    style_sidebar_btn(cui_sidebarSettingsButton);
    lv_obj_t *cui_sidebarSettingsButtonIcon = create_sidebar_icon(cui_sidebarSettingsButton, "d");

    lv_obj_t **children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_SIDEBARCOMPONENT_NUM);
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARCOMPONENT] = cui_sidebarComponent;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARHOMEBUTTON] = cui_sidebarHomeButton;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARHOMEBUTTON_SIDEBARHOMEBUTTONICON] = cui_sidebarHomeButtonIcon;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARTEMPBUTTON] = cui_sidebarTempButton;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARTEMPBUTTON_SIDEBARTEMPBUTTONICON] = cui_sidebarTempButtonIcon;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARCONTROLBUTTON] = cui_sidebarControlButton;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARCONTROLBUTTON_SIDEBARCONTROLBUTTONICON] = cui_sidebarControlButtonIcon;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARNOZZLEBUTTON] = cui_sidebarNozzleButton;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARNOZZLEBUTTON_SIDEBARNOZZLEBUTTONICON] = cui_sidebarNozzleButtonIcon;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARSETTINGSBUTTON] = cui_sidebarSettingsButton;
    children[UI_COMP_SIDEBARCOMPONENT_SIDEBARSETTINGSBUTTON_SIDEBARSETTINGSBUTTONICON] = cui_sidebarSettingsButtonIcon;
    lv_obj_add_event_cb(cui_sidebarComponent, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_sidebarComponent, del_component_child_event_cb, LV_EVENT_DELETE, children);
    lv_obj_add_event_cb(cui_sidebarHomeButton, ui_event_comp_sidebarComponent_sidebarHomeButton, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_sidebarTempButton, ui_event_comp_sidebarComponent_sidebarTempButton, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_sidebarControlButton, ui_event_comp_sidebarComponent_sidebarControlButton, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_sidebarNozzleButton, ui_event_comp_sidebarComponent_sidebarNozzleButton, LV_EVENT_ALL, children);
    lv_obj_add_event_cb(cui_sidebarSettingsButton, ui_event_comp_sidebarComponent_sidebarSettingsButton, LV_EVENT_ALL, children);
    ui_comp_sidebarComponent_create_hook(cui_sidebarComponent);
    return cui_sidebarComponent;
}
