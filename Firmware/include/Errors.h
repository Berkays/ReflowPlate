#ifndef __ERRORS_H
#define __ERRORS_H

#include "Types.h"

enum class Errors : byte
{
    None = 0,
    VOLTAGE_FAULT = 1,
    NTC_FAULT = 2,
    THERMALRUNAWAY_FAULT = 3
};

#endif