#ifndef __HEATER_H
#define __HEATER_H

#include "peripherals.h"

#include "global.h"

void stopHeater();
void adjustCurrentClamp(double, double, uint16_t *, uint16_t *);

#endif