#include <Arduino.h>
#include <ArduinoJson.h>
#include "bambudeck/debug.h"
#include "bambudeck/paths.h"
#include "bambudeck/eeprom.h"
#include "bambudeck/types.h"
#include "bambudeck/bblp.h"
#include "bambudeck/globals.h"
#include "bambudeck/filesystem.h"
#include "ui/ui.h"
#include "bambudeck/hms.h"

#if defined(__BAMBUDECK_SCREEN_28__)
#include "devices/2.8/screen.h"
#endif

#include "bambudeck/settings.h"
#include "bambudeck/firmware.h"
#include "bambudeck/ha_api.h"
#include "bambudeck/sensors/chamber.h"
#include "bambudeck/thumbnail.h"
#include "bambudeck/events.h"
#include "bambudeck/connection.h"
#include "bambudeck/coldboot.h"

void bambudeck_intro_show(void)
{
    ui_introScreen_screen_init();
    lv_disp_load_scr(introScreen);
    lv_timer_handler();
}

void setup()
{
    Serial.begin(115200);
    delay(100); // Power rail and flash stabilization on cold boot

    bambudeck_eeprom_setup();
    bambudeck_globals_init();
    bambudeck_screen_setup();
    bambudeck_thumbnail_init();
    bambudeck_intro_show();

    bambudeck_coldboot_check();

    bambudeck_settings_loadSettings();
    bambudeck_screen_setupScreenTimer();

    bambudeck_firmware_checkFirmwareUpdate();
    bambudeck_touch_setup();

    while (!bambudeck_wifi_setup())
        ;

    bambudeck_ota_setup();
    bambudeck_firmware_checkOnlineFirmwareUpdate();

    bambudeck_setupGlobalEvents();
    bambudeck_ha_setup();

    bambudeck_chamber_timer_init();
    bambudeck_thumbnail_update_ui();
}

void loop()
{
    lv_timer_handler();
    lv_task_handler();
    bambudeck_ha_process_ui();
    bambudeck_ota_loop();
    delay(5);
}
