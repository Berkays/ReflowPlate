#include "temperature.h"

#include "Config.h"

#include <math.h>

#include "analog.h"

#if TEMPERATURE_METHOD == NTC_CURVE
double mdl::temp::ntc::Read(unsigned int channel, unsigned int sampleCount)
{
    uint32_t adcValue = 0;

    for (byte i = 0; i < sampleCount; i++)
        adcValue += Read_ADC(channel);

    adcValue /= sampleCount;

    double readVoltage = ADC_TO_VOLTAGE(adcValue);

    if (readVoltage < 0.5)
        return -100;

    double R = THERMISTOR_SERIES_RESISTOR * ((OPERATING_VOLTAGE / readVoltage) - 1);

    return THERMISTOR_CURVE(R);
}
#endif