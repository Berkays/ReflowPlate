#ifndef __MACROS_H
#define __MACROS_H

#include "Config.h"

#define VOLTAGE_TO_ADC(V) ((ADC_MAX * V / OPERATING_VOLTAGE))
#define ADC_TO_VOLTAGE(ADC) ((ADC * OPERATING_VOLTAGE / ADC_MAX))
#define ADC_TO_VOLTAGE_DIVIDER(adc, r1, r2) ((ADC_TO_VOLTAGE(adc) * (r1 + r2)) / r2)

#endif