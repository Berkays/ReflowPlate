#include "temperature.h"

#include "Config.h"

#include <math.h>

#include "analog.h"

#if TEMPERATURE_METHOD == NTC_STEINHART
double mdl::temp::ntc::Read(unsigned int channel, unsigned int sampleCount)
{
    uint16_t adcValue = 0;

    for (byte i = 0; i < sampleCount; i++)
        adcValue += Read_ADC(channel);

    adcValue /= sampleCount;

    double readVoltage = ADC_TO_VOLTAGE(adcValue);

    double Rt = THERMISTOR_SERIES_RESISTOR * ((OPERATING_VOLTAGE / readVoltage) - 1);
    double temp = Rt / THERMISTOR_NOMINAL_RESISTOR;          // (R/Ro)
    temp = log(temp);                                        // ln(R/Ro)
    temp /= THERMISTOR_BETA_COEFFICIENT;                     // 1/B * ln(R/Ro)
    temp += 1.0 / (THERMISTOR_TEMPERATURE_NOMINAL + 273.15); // + (1/To)
    temp = 1.0 / temp;
    temp -= 273.15;

    return temp;
}
#endif