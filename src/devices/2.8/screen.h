#ifndef _BAMBUDECK_SCREEN
#define _BAMBUDECK_SCREEN

#include <TFT_eSPI.h>

#define LCD_BACK_LIGHT_PIN 21

// LEDC Backlight PWM channel, resolution, and frequency
#define LEDC_CHANNEL_0 0
#define LEDC_TIMER_12_BIT 12
#define LEDC_BASE_FREQ 5000

static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[4096];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

#include "ui/ui.h"
#include "touch.h"
#include "bambudeck/globals.h"

bool bambudeck_screen_touchFromPowerOff = false;

inline void bambudeck_screen_ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255)
{
    uint32_t duty = (4095 / valueMax) * min(value, valueMax);
    ledcWrite(channel, duty);
}

inline void bambudeck_screen_setBrightness(uint8_t brightness)
{
    bambudeck_screen_ledcAnalogWrite(LEDC_CHANNEL_0, brightness);
}

inline void bambudeck_screen_setBackLedOff()
{
    pinMode(4, OUTPUT);
    pinMode(16, OUTPUT);
    pinMode(17, OUTPUT);
    digitalWrite(4, HIGH);
    digitalWrite(16, HIGH);
    digitalWrite(17, HIGH); // RGB LEDs on CYD are active low
}

inline void bambudeck_screen_wakeUp()
{
    if (bambudeck_screen_onScreenOffTimer != NULL)
    {
        lv_timer_reset(bambudeck_screen_onScreenOffTimer);
    }
    bambudeck_screen_touchFromPowerOff = false;
    loadScreen(0);
    bambudeck_screen_setBrightness(bambuDeckConfig.backlightLevel);
}

inline void bambudeck_screen_onScreenOff(lv_timer_t *timer)
{
    (void)timer;
    if (bambuDeckConfig.tftOffValue < BAMBUDECK_LCD_MIN_SLEEP_TIME)
    {
        return;
    }

    ConsoleInfo.println(F("[BambuDeck][SCREEN] Screen Sleep"));
    bambudeck_screen_setBrightness(0);
    bambudeck_screen_touchFromPowerOff = true;
}

inline void bambudeck_screen_setupScreenTimer()
{
    bambudeck_screen_onScreenOffTimer = lv_timer_create(bambudeck_screen_onScreenOff, bambuDeckConfig.tftOffValue * 1000 * 60, NULL);
    lv_timer_pause(bambudeck_screen_onScreenOffTimer);
}

inline void bambudeck_screen_startScreenTimer()
{
    if (bambudeck_screen_onScreenOffTimer != NULL)
    {
        lv_timer_resume(bambudeck_screen_onScreenOffTimer);
    }
}

inline void bambudeck_screen_setScreenTimer(uint32_t period)
{
    if (bambudeck_screen_onScreenOffTimer != NULL)
    {
        lv_timer_set_period(bambudeck_screen_onScreenOffTimer, period);
    }
}

inline void bambudeck_screen_invertColors()
{
    tft.invertDisplay(bambuDeckConfig.tftInvert);
}

inline uint8_t bambudeck_screen_getTFTFlip()
{
    uint8_t val = bambudeck_eeprom_read(BAMBUDECK_EEPROM_POS_TFTFLIP);
    bambuDeckConfig.tftFlip = val;
    return val;
}

inline void bambudeck_screen_setTFTFlip(uint8_t mode)
{
    bambuDeckConfig.tftFlip = mode;
    bambudeck_eeprom_write(BAMBUDECK_EEPROM_POS_TFTFLIP, mode);
}

inline void bambudeck_screen_toggleTFTFlip()
{
    bambudeck_screen_setTFTFlip(!bambudeck_screen_getTFTFlip());
    bambudeck_resetTouchConfig();
}

inline void bambudeck_screen_setupTFTFlip()
{
    uint8_t eepromTFTFlip = bambudeck_screen_getTFTFlip();
    tft.setRotation(eepromTFTFlip == 1 ? 3 : 1);
    bambudeck_touchScreen.setRotation(eepromTFTFlip == 1 ? 3 : 1);
}

inline void bambudeck_screen_dispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

inline void bambudeck_screen_touchRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    (void)indev_driver;
    if (bambudeck_touchScreen.tirqTouched() && bambudeck_touchScreen.touched())
    {
        if (bambudeck_screen_onScreenOffTimer != NULL)
        {
            lv_timer_reset(bambudeck_screen_onScreenOffTimer);
        }
        if (bambudeck_screen_touchFromPowerOff)
        {
            bambudeck_screen_wakeUp();
            while (bambudeck_touchScreen.touched())
                ;
            return;
        }

        TS_Point p = bambudeck_touchScreen.getPoint();
        ScreenPoint sp = getScreenCoords(p.x, p.y);
        data->state = LV_INDEV_STATE_PR;
        data->point.x = sp.x;
        data->point.y = sp.y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

inline void bambudeck_screen_setup()
{
    ConsoleInfo.println(F("[BambuDeck][SCREEN] Initializing Display"));
    pinMode(XPT2046_CS, OUTPUT);
    pinMode(TFT_CS, OUTPUT);

    digitalWrite(XPT2046_CS, HIGH);
    digitalWrite(TFT_CS, HIGH);

    bambudeck_screen_setBackLedOff();

    lv_init();
    tft.init();

    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
    ledcAttachPin(LCD_BACK_LIGHT_PIN, LEDC_CHANNEL_0);

    bambudeck_touch_spi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    bambudeck_touchScreen.begin(bambudeck_touch_spi);

    bambudeck_screen_setupTFTFlip();
    bambudeck_screen_setBrightness(255);

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, 4096);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenHeight;
    disp_drv.ver_res = screenWidth;
    disp_drv.flush_cb = bambudeck_screen_dispFlush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = bambudeck_screen_touchRead;
    lv_indev_drv_register(&indev_drv);

    LV_EVENT_GET_COMP_CHILD = lv_event_register_id();

    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    initTopLayer();
}

#endif
