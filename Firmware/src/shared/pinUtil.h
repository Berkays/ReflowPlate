#ifndef __PINUTIL_H
#define __PINUTIL_H

#include "hal_driver.h"
#include "pins.h"

#include "Types.h"

#define TOGGLE_STATUS_LED(STATE)                                        \
    {                                                                   \
        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, STATE); \
    }

#define TOGGLE_FAN(STATE)                                 \
    {                                                     \
        HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, STATE); \
    }

#endif