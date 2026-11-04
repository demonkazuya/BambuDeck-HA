#ifndef _BAMBUDECK_EEPROM
#define _BAMBUDECK_EEPROM

#include <Arduino.h>
#include <LittleFS.h>
#include "paths.h"
#include "debug.h"
#include "filesystem.h"

#define BAMBUDECK_EEPROM_SIZE 16
#define BAMBUDECK_EEPROM_POS_TFTFLIP 0

inline void bambudeck_eeprom_setup()
{
    if (!bambudeck_filesystem_init())
    {
        ConsoleError.println(F("[BambuDeck][EEPROM] Filesystem init failed"));
        return;
    }

    if (!LittleFS.exists(bambudeck_paths_root))
    {
        LittleFS.mkdir(bambudeck_paths_root);
    }

    if (!LittleFS.exists(bambudeck_paths_eeprom))
    {
        uint8_t buffer[BAMBUDECK_EEPROM_SIZE] = {0};
        File f = LittleFS.open(bambudeck_paths_eeprom, FILE_WRITE);
        if (f)
        {
            f.write(buffer, BAMBUDECK_EEPROM_SIZE);
            f.close();
        }
    }
}

inline void bambudeck_eeprom_write(int address, uint8_t value)
{
    if (address < 0 || address >= BAMBUDECK_EEPROM_SIZE)
        return;

    uint8_t buffer[BAMBUDECK_EEPROM_SIZE] = {0};
    if (LittleFS.exists(bambudeck_paths_eeprom))
    {
        File rf = LittleFS.open(bambudeck_paths_eeprom, FILE_READ);
        if (rf)
        {
            rf.read(buffer, BAMBUDECK_EEPROM_SIZE);
            rf.close();
        }
    }

    buffer[address] = value;

    File wf = LittleFS.open(bambudeck_paths_eeprom, FILE_WRITE);
    if (wf)
    {
        wf.write(buffer, BAMBUDECK_EEPROM_SIZE);
        wf.close();
    }
}

inline uint8_t bambudeck_eeprom_read(int address)
{
    if (address < 0 || address >= BAMBUDECK_EEPROM_SIZE)
        return 0;

    uint8_t buffer[BAMBUDECK_EEPROM_SIZE] = {0};
    if (LittleFS.exists(bambudeck_paths_eeprom))
    {
        File f = LittleFS.open(bambudeck_paths_eeprom, FILE_READ);
        if (f)
        {
            f.read(buffer, BAMBUDECK_EEPROM_SIZE);
            f.close();
        }
    }
    return buffer[address];
}

#endif
