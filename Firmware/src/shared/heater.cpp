#include "heater.h"

void stopHeater()
{
    HAL_TIM_PWM_Stop(pwm_timer, pwm_channel);
}

void adjustCurrentClamp(double sourceVoltage, double temperature, uint16_t *minOutputClamp, uint16_t *maxOutputClamp)
{
    volatile uint32_t ARR = __HAL_TIM_GET_AUTORELOAD(pwm_timer);

    double nominalCurrent = (sourceVoltage / (HEATING_ELEMENT_NOMINAL_RESISTANCE + (HEATING_ELEMENT_RESISTANCE_PER_DEGREE * (temperature - 20.0))));
    *minOutputClamp = ARR * (MIN_CURRENT / nominalCurrent);
    *maxOutputClamp = ARR * (MAX_CURRENT / nominalCurrent);
}