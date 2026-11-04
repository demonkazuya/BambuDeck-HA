#ifndef _BAMBUDECK_HMS_ERRORS
#define _BAMBUDECK_HMS_ERRORS
#ifdef __cplusplus
extern "C"
{
#endif

#include "bambudeck/bbl/bbl-errors.h"
#include <pgmspace.h>
    // Function to retrieve a value by key
    bool bambudeck_errors_isKeyPresent(const char *key, const char * const keys[], int length);
    bool bambudeck_errors_deviceErrorHasDone(const char *key);
    bool bambudeck_errors_deviceErrorHasRetry(const char *key);
    const char *bambudeck_errors_getValueByKey(const char *key, const char * const keys[], const char * const values[], int numEntries);
    const char *bambudeck_errors_getHMSError(const char *key);
    const char *bambudeck_errors_getDeviceError(const char *key);
#ifdef __cplusplus
}
#endif

#endif