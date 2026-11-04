#include "bblp.h"
#include <string.h>
#include "types.h"

extern "C" {

bool bambudeck_bblp_is_p1p()
{
    return strcmp(bambuDeckConfig.printerModel, "C11") == 0;
}

bool bambudeck_bblp_is_p1s()
{
    return strcmp(bambuDeckConfig.printerModel, "C12") == 0;
}

bool bambudeck_bblp_is_p1Series()
{
    return bambudeck_bblp_is_p1p() || bambudeck_bblp_is_p1s();
}

bool bambudeck_bblp_is_x1()
{
    return strcmp(bambuDeckConfig.printerModel, "3DPrinter-X1") == 0;
}

bool bambudeck_bblp_is_x1c()
{
    return strcmp(bambuDeckConfig.printerModel, "3DPrinter-X1-Carbon") == 0;
}

bool bambudeck_bblp_is_x1Series()
{
    return bambudeck_bblp_is_x1() || bambudeck_bblp_is_x1c();
}

}
