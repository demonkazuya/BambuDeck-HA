#ifndef _BAMBU_THEME_H
#define _BAMBU_THEME_H

#include "lvgl.h"
#include "../bambudeck/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * BAMBU LAB BRAND & SEMANTIC ACCENT COLORS
 * Inspired by Bambu Studio, Bambu Handy, and Bambu X1C Touchscreen UI
 * ========================================================================= */
#define BAMBU_GREEN_HEX          0x00AE42  /* Signature Bambu Lab Green */
#define BAMBU_GREEN_PRESSED_HEX  0x008A34  /* Pressed / Darker Bambu Green */
#define BAMBU_ORANGE_HEX         0xFF6B00  /* Heating / Active Warning Amber */
#define BAMBU_ORANGE_PRESSED_HEX 0xCC5500  /* Pressed Amber */
#define BAMBU_RED_HEX            0xE53935  /* Stop / Cancel / Danger Red */
#define BAMBU_RED_PRESSED_HEX    0xB71C1C  /* Pressed Danger Red */

/* =========================================================================
 * DARK / LIGHT MODE ADAPTIVE SURFACE & TEXT PALETTE
 * Checked dynamically via bambuDeckConfig.lightMode
 * ========================================================================= */

/* Main Screen Background Canvas */
#define BAMBU_BG_SCREEN_HEX()    (bambuDeckConfig.lightMode ? 0xEEF1F5 : 0x121417)

/* Left Navigation Sidebar Surface */
#define BAMBU_BG_SIDEBAR_HEX()   (bambuDeckConfig.lightMode ? 0xFFFFFF : 0x1A1D21)

/* Card / Panel Container Surface */
#define BAMBU_BG_CARD_HEX()      (bambuDeckConfig.lightMode ? 0xFFFFFF : 0x1E2228)

/* Interactive Button / Elevated Control Surface */
#define BAMBU_BG_ELEVATED_HEX()  (bambuDeckConfig.lightMode ? 0xE2E7EE : 0x282E36)

/* Pressed Button Surface */
#define BAMBU_BG_PRESSED_HEX()   (bambuDeckConfig.lightMode ? 0xCBD2DC : 0x363E4A)

/* Subtle Active Green Tint Surface */
#define BAMBU_BG_GREEN_TINT_HEX() (bambuDeckConfig.lightMode ? 0xDCF5E6 : 0x143320)

/* Subtle Heating Orange Tint Surface */
#define BAMBU_BG_HEAT_TINT_HEX() (bambuDeckConfig.lightMode ? 0xFFF0E5 : 0x331F12)

/* Card / Control Border & Divider Line */
#define BAMBU_BORDER_HEX()       (bambuDeckConfig.lightMode ? 0xD0D7DE : 0x323942)

/* Primary High-Contrast Text & Active Icons */
#define BAMBU_TEXT_PRIMARY_HEX() (bambuDeckConfig.lightMode ? 0x1B1F24 : 0xF0F4F8)

/* Secondary Label / Inactive Icon Text */
#define BAMBU_TEXT_SECONDARY_HEX() (bambuDeckConfig.lightMode ? 0x57606A : 0x9AA5B1)

/* Muted / Disabled Hint Text */
#define BAMBU_TEXT_MUTED_HEX()   (bambuDeckConfig.lightMode ? 0x8C959F : 0x6E7781)

/* Convenience lv_color_t wrappers */
#define BAMBU_COLOR_GREEN()          lv_color_hex(BAMBU_GREEN_HEX)
#define BAMBU_COLOR_GREEN_PRESSED()  lv_color_hex(BAMBU_GREEN_PRESSED_HEX)
#define BAMBU_COLOR_ORANGE()         lv_color_hex(BAMBU_ORANGE_HEX)
#define BAMBU_COLOR_ORANGE_PRESSED() lv_color_hex(BAMBU_ORANGE_PRESSED_HEX)
#define BAMBU_COLOR_RED()            lv_color_hex(BAMBU_RED_HEX)
#define BAMBU_COLOR_RED_PRESSED()    lv_color_hex(BAMBU_RED_PRESSED_HEX)

#define BAMBU_COLOR_BG_SCREEN()      lv_color_hex(BAMBU_BG_SCREEN_HEX())
#define BAMBU_COLOR_BG_SIDEBAR()     lv_color_hex(BAMBU_BG_SIDEBAR_HEX())
#define BAMBU_COLOR_BG_CARD()        lv_color_hex(BAMBU_BG_CARD_HEX())
#define BAMBU_COLOR_BG_ELEVATED()    lv_color_hex(BAMBU_BG_ELEVATED_HEX())
#define BAMBU_COLOR_BG_PRESSED()     lv_color_hex(BAMBU_BG_PRESSED_HEX())
#define BAMBU_COLOR_BG_GREEN_TINT()  lv_color_hex(BAMBU_BG_GREEN_TINT_HEX())
#define BAMBU_COLOR_BG_HEAT_TINT()   lv_color_hex(BAMBU_BG_HEAT_TINT_HEX())
#define BAMBU_COLOR_BORDER()         lv_color_hex(BAMBU_BORDER_HEX())
#define BAMBU_COLOR_TEXT_PRIMARY()   lv_color_hex(BAMBU_TEXT_PRIMARY_HEX())
#define BAMBU_COLOR_TEXT_SECONDARY() lv_color_hex(BAMBU_TEXT_SECONDARY_HEX())
#define BAMBU_COLOR_TEXT_MUTED()     lv_color_hex(BAMBU_TEXT_MUTED_HEX())

static inline void bambu_apply_screen_theme(lv_obj_t *screen)
{
    lv_disp_t *dispp = lv_disp_get_default();
    if (dispp)
    {
        lv_theme_t *theme = lv_theme_default_init(
            dispp,
            lv_color_hex(BAMBU_GREEN_HEX),
            lv_color_hex(BAMBU_ORANGE_HEX),
            !bambuDeckConfig.lightMode,
            LV_FONT_DEFAULT);
        lv_disp_set_theme(dispp, theme);
    }
    if (screen)
    {
        lv_obj_set_style_bg_color(screen, BAMBU_COLOR_BG_SCREEN(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen, BAMBU_COLOR_TEXT_PRIMARY(), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _BAMBU_THEME_H */
