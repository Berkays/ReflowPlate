#ifndef __TEMPERATURE_H
#define __TEMPERATURE_H

#include "Types.h"
#include "global.h"

namespace mdl::temp::ntc
{
    extern double Read(unsigned int, unsigned int sampleCount = 1);
}

#endif