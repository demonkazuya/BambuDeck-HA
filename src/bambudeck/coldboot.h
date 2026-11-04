#ifndef _BAMBUDECK_COLDBOOT
#define _BAMBUDECK_COLDBOOT

#include <Arduino.h>
#include <rom/rtc.h>
#include "config.h"

inline void bambudeck_coldboot_check()
{
    if (rtc_get_reset_reason(0) == POWERON_RESET)
    {
        DynamicJsonDocument config = bambudeck_load_config();
        int coldbootTimeout = config.containsKey("coldboot") ? config["coldboot"].as<int>() : 500;
        lv_label_set_text(introScreenCaption, LV_SYMBOL_POWER);
        lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_timer_handler();
        delay(coldbootTimeout);
    }
}

#endif
