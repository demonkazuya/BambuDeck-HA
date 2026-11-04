#ifndef _BAMBUDECK_GLOBALS
#define _BAMBUDECK_GLOBALS

#include "types.h"

BambuDeckBambuStatus bambuStatus;
BambuDeckControlMode controlMode;
BambuDeckConfig bambuDeckConfig;

inline void bambudeck_globals_init()
{
    controlMode.inc = 1;
    controlMode.axis = ControlAxisXY;
    controlMode.target_bed_temper = 0;
    controlMode.target_nozzle_temper = 0;
    bambuDeckConfig.currentScreenIndex = -1;
}

#endif
