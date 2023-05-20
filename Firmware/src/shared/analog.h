#ifndef __ANALOG_H
#define __ANALOG_H

#include "peripherals.h"

#include "global.h"

HAL_StatusTypeDef Add_ADC_Channel(uint32_t, uint32_t);
uint32_t Read_ADC(uint32_t, uint32_t sampleTime = ADC_SAMPLETIME_71CYCLES_5);

#endif