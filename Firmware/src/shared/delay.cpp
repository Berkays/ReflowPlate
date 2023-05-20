#include "delay.h"

void delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(i2c_timer, 0);
    while ((__HAL_TIM_GET_COUNTER(i2c_timer)) < us)
        ;
}