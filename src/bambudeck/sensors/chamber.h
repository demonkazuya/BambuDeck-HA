#ifndef _BAMBUDECK_SENSORS_CHAMBER_TEMP
#define _BAMBUDECK_SENSORS_CHAMBER_TEMP

#include <OneWire.h>
#include <DallasTemperature.h>

#define BAMBUDECK_CHAMBER_TEMP_PIN 22

// Setup a temperatureSensorsOneWire instance to communicate with any temperatureSensorsOneWire devices
OneWire temperatureSensorsOneWire(BAMBUDECK_CHAMBER_TEMP_PIN);

// Pass our temperatureSensorsOneWire reference to Dallas Temperature sensor
DallasTemperature bambudeck_chamber_sensors(&temperatureSensorsOneWire);

lv_timer_t *bambudeck_chambertemp_requestTemperaturesTimer = NULL;
bool bambudeck_has_ds18b20 = false;

void bambudeck_chamber_requestTemperatures(lv_timer_t *timer);

void bambudeck_chamber_timer_create()
{
    bambudeck_chambertemp_requestTemperaturesTimer = lv_timer_create(bambudeck_chamber_requestTemperatures, 5000, NULL);
    lv_timer_set_repeat_count(bambudeck_chambertemp_requestTemperaturesTimer, 1);
}

extern void bambudeck_send_msg(BAMBUDECK_MESSAGE message, unsigned long long data);
extern void bambudeck_ha_publish_chamber_temp(int temp);

void bambudeck_chamber_requestTemperatures(lv_timer_t *timer)
{
    if (!bambudeck_has_ds18b20)
        return;

    float rawTemp = bambudeck_chamber_sensors.getTempCByIndex(0);
    if (rawTemp > -50.0f && rawTemp < 125.0f && rawTemp != 85.0f)
    {
        int temperatureC = round(rawTemp) + bambuDeckConfig.chamberSensorReadingDiff;
        bambuStatus.chamber_temper = temperatureC;
        bambudeck_send_msg(BAMBUDECK_ON_CHAMBER_TEMP, temperatureC);
        bambudeck_ha_publish_chamber_temp(temperatureC);
    }
    bambudeck_chamber_sensors.requestTemperatures();
    bambudeck_chamber_timer_create();
}

bool bambudeck_chamber_started = false;
void bambudeck_chamber_timer_start()
{
    if (!bambudeck_chamber_started)
    {
        bambudeck_chamber_sensors.begin();
        bambudeck_chamber_sensors.setWaitForConversion(false);
        bambudeck_has_ds18b20 = (bambudeck_chamber_sensors.getDeviceCount() > 0);
        bambudeck_chamber_started = true;
        ConsoleInfo.printf("[BambuDeck][Chamber] Local DS18B20 sensors detected: %d\n", bambudeck_chamber_sensors.getDeviceCount());
    }
    if (bambudeck_has_ds18b20)
    {
        bambudeck_chamber_sensors.requestTemperatures();
        bambudeck_chamber_timer_create();
    }
}

void bambudeck_chamber_timer_stop()
{
    if (bambudeck_chambertemp_requestTemperaturesTimer != NULL)
    {
        lv_timer_pause(bambudeck_chambertemp_requestTemperaturesTimer);
    }
}

void bambudeck_chamber_timer_init()
{
    if (bambuDeckConfig.chamberSensorEnabled)
    {
        bambudeck_chamber_timer_start();
    }
    else
    {
        if (bambudeck_chamber_started)
        {
            bambudeck_chamber_timer_stop();
        }
    }
}

#endif
