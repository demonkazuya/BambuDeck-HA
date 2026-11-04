#ifndef _BAMBUDECK_TOUCH
#define _BAMBUDECK_TOUCH

#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <ArduinoJson.h>
#include "bambudeck/types.h"
#include "bambudeck/filesystem.h"
#include "bambudeck/paths.h"
#include "bambudeck/debug.h"

#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25
#define XPT2046_CS   33

SPIClass bambudeck_touch_spi = SPIClass(HSPI);
XPT2046_Touchscreen bambudeck_touchScreen(XPT2046_CS, XPT2046_IRQ);
BambuDeckPanelConfig bambudeck_touchConfig;

class ScreenPoint
{
public:
    int16_t x;
    int16_t y;

    ScreenPoint() : x(0), y(0) {}
    ScreenPoint(int16_t xIn, int16_t yIn) : x(xIn), y(yIn) {}
};

inline ScreenPoint getScreenCoords(int16_t x, int16_t y)
{
    int16_t xCoord = round((x * bambudeck_touchConfig.xCalM) + bambudeck_touchConfig.xCalC);
    int16_t yCoord = round((y * bambudeck_touchConfig.yCalM) + bambudeck_touchConfig.yCalC);
    if (xCoord < 0) xCoord = 0;
    if (xCoord >= 320) xCoord = 319;
    if (yCoord < 0) yCoord = 0;
    if (yCoord >= 240) yCoord = 239;
    return ScreenPoint(xCoord, yCoord);
}

inline bool bambudeck_loadTouchConfig(BambuDeckPanelConfig &config)
{
    if (!bambudeck_filesystem_exist(bambudeck_paths_touch))
    {
        return false;
    }

    File file = bambudeck_filesystem_open(bambudeck_paths_touch);
    if (!file)
        return false;

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error || doc["xCalM"].isNull() || doc["yCalM"].isNull())
    {
        ConsoleError.println(F("[BambuDeck][Touch] Invalid touch calibration"));
        return false;
    }

    config.xCalM = doc["xCalM"].as<float>();
    config.yCalM = doc["yCalM"].as<float>();
    config.xCalC = doc["xCalC"].as<float>();
    config.yCalC = doc["yCalC"].as<float>();

    if (config.xCalM == 0.0f || config.yCalM == 0.0f || isnan(config.xCalM) || isnan(config.yCalM))
    {
        ConsoleError.println(F("[BambuDeck][Touch] Invalid calibration coefficients"));
        return false;
    }

    return true;
}

inline void bambudeck_saveTouchConfig(BambuDeckPanelConfig &config)
{
    DynamicJsonDocument doc(512);
    doc["xCalM"] = config.xCalM;
    doc["yCalM"] = config.yCalM;
    doc["xCalC"] = config.xCalC;
    doc["yCalC"] = config.yCalC;

    bambudeck_filesystem_writeJson(bambudeck_paths_touch, doc);
}

inline void bambudeck_resetTouchConfig()
{
    ConsoleInfo.println(F("[BambuDeck][Touch] Resetting touch calibration"));
    bambudeck_filesystem_deleteFile(bambudeck_paths_touch);
    delay(500);
    ESP.restart();
}

inline bool hasTouchConfig()
{
    return bambudeck_filesystem_exist(bambudeck_paths_touch);
}

inline void bambudeck_touch_setup()
{
    if (hasTouchConfig() && bambudeck_loadTouchConfig(bambudeck_touchConfig))
    {
        ConsoleInfo.println(F("[BambuDeck][TOUCH] Loaded calibration from flash"));
        return;
    }

    ConsoleInfo.println(F("[BambuDeck][TOUCH] Starting 2-Point Touch Calibration"));
    TS_Point p;
    int16_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    while (true)
    {
        lv_label_set_text(introScreenCaption, "Step 1/2: Touch TOP-LEFT  " LV_SYMBOL_PLUS);
        lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0x00ae42), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_timer_handler();

        while (bambudeck_touchScreen.touched())
            ;
        while (!bambudeck_touchScreen.touched())
            ;

        delay(150);
        p = bambudeck_touchScreen.getPoint();
        x1 = p.x;
        y1 = p.y;

        lv_label_set_text(introScreenCaption, "Step 2/2: Touch BOTTOM-RIGHT  " LV_SYMBOL_PLUS);
        lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0x00ae42), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_timer_handler();

        while (bambudeck_touchScreen.touched())
            ;
        while (!bambudeck_touchScreen.touched())
            ;

        delay(150);
        p = bambudeck_touchScreen.getPoint();
        x2 = p.x;
        y2 = p.y;

        if (abs(x2 - x1) > 500 && abs(y2 - y1) > 500)
        {
            break;
        }
    }

    int16_t screenX1 = 30;
    int16_t screenY1 = 30;
    int16_t screenX2 = 290;
    int16_t screenY2 = 210;

    bambudeck_touchConfig.xCalM = (float)(screenX2 - screenX1) / (float)(x2 - x1);
    bambudeck_touchConfig.xCalC = screenX1 - (bambudeck_touchConfig.xCalM * x1);

    bambudeck_touchConfig.yCalM = (float)(screenY2 - screenY1) / (float)(y2 - y1);
    bambudeck_touchConfig.yCalC = screenY1 - (bambudeck_touchConfig.yCalM * y1);

    bambudeck_saveTouchConfig(bambudeck_touchConfig);
    ConsoleInfo.println(F("[BambuDeck][TOUCH] Calibration completed and saved"));

    while (bambudeck_touchScreen.touched())
        ;
}

#endif
