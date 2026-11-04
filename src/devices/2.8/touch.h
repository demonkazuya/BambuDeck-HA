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
    delay(300);
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
        lv_label_set_text(introScreenCaption, "Touch the " LV_SYMBOL_PLUS " crosshair with stylus (1/2)");
        lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0x00ae42), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_timer_handler();

        // Wait for release while yielding to watchdog
        while (bambudeck_touchScreen.touched())
        {
            delay(10);
            lv_timer_handler();
        }

        // Draw crosshair at Top-Left (25, 25)
        tft.drawFastHLine(15, 25, 21, TFT_WHITE);
        tft.drawFastVLine(25, 15, 21, TFT_WHITE);

        // Wait for press while yielding to watchdog
        while (!bambudeck_touchScreen.touched())
        {
            delay(10);
            lv_timer_handler();
        }

        delay(60);
        p = bambudeck_touchScreen.getPoint();
        x1 = p.x;
        y1 = p.y;

        // Erase crosshair
        tft.drawFastHLine(15, 25, 21, TFT_BLACK);
        tft.drawFastVLine(25, 15, 21, TFT_BLACK);
        delay(300);

        lv_label_set_text(introScreenCaption, "Touch the " LV_SYMBOL_PLUS " crosshair with stylus (2/2)");
        lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0x00ae42), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_timer_handler();

        // Wait for release
        while (bambudeck_touchScreen.touched())
        {
            delay(10);
            lv_timer_handler();
        }

        // Draw crosshair at Bottom-Right (295, 215)
        tft.drawFastHLine(285, 215, 21, TFT_WHITE);
        tft.drawFastVLine(295, 205, 21, TFT_WHITE);

        // Wait for press
        while (!bambudeck_touchScreen.touched())
        {
            delay(10);
            lv_timer_handler();
        }

        delay(60);
        p = bambudeck_touchScreen.getPoint();
        x2 = p.x;
        y2 = p.y;

        // Erase crosshair
        tft.drawFastHLine(285, 215, 21, TFT_BLACK);
        tft.drawFastVLine(295, 205, 21, TFT_BLACK);

        if (abs(x2 - x1) > 300 && abs(y2 - y1) > 300)
        {
            break;
        }
        delay(300);
    }

    // Coordinates of calibration targets: P1(25, 25), P2(295, 215)
    int16_t xDist = 295 - 25; // 270
    int16_t yDist = 215 - 25; // 190

    bambudeck_touchConfig.xCalM = (float)xDist / (float)(x2 - x1);
    bambudeck_touchConfig.xCalC = 25.0f - ((float)x1 * bambudeck_touchConfig.xCalM);

    bambudeck_touchConfig.yCalM = (float)yDist / (float)(y2 - y1);
    bambudeck_touchConfig.yCalC = 25.0f - ((float)y1 * bambudeck_touchConfig.yCalM);

    bambudeck_saveTouchConfig(bambudeck_touchConfig);
    ConsoleInfo.printf("[BambuDeck][TOUCH] Calibrated: xM=%.4f xC=%.2f yM=%.4f yC=%.2f\n",
                       bambudeck_touchConfig.xCalM, bambudeck_touchConfig.xCalC,
                       bambudeck_touchConfig.yCalM, bambudeck_touchConfig.yCalC);

    while (bambudeck_touchScreen.touched())
    {
        delay(10);
        lv_timer_handler();
    }
}

#endif
