#ifndef __DISPLAY_INTERFACE_H
#define __DISPLAY_INTERFACE_H

#include "hal_driver.h"

#include "Types.h"

#include "u8g2.h"

#include "delay.h"
#include "global.h"

inline byte u8x8_gpio_and_delay_template(u8x8_t *u8x8, byte msg, byte arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT: // called once during init phase of u8g2/u8x8
        break;                         // can be used to setup pins
    case U8X8_MSG_DELAY_NANO:          // delay arg_int * 1 nano second
        delay_us(1000 * arg_int);
        break;
    case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
        delay_us(100 * arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
        delay_us(10 * arg_int);
        break;
    case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
        HAL_Delay(arg_int);
        break;
    case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        delay_us(1);
        break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
        break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
        break;                    // arg_int=1: Input dir with pullup high for I2C data pin
    }
    return 1;
}

inline byte u8x8_byte_hw_i2c(u8x8_t *u8x8, byte msg, byte arg_int, void *arg_ptr)
{
    byte *ptr;
    static byte buffer_count;
    static byte buffer[32];

    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
        ptr = (byte *)arg_ptr;
        for (int i = 1; i <= arg_int; i++)
        {
            buffer[buffer_count] = *(ptr++);
            buffer_count++;
        }
        break;
    case U8X8_MSG_BYTE_INIT:
        break;
    case U8X8_MSG_BYTE_SET_DC:
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        buffer_count = 0;
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        if (HAL_I2C_Master_Transmit(i2c, i2c->Init.OwnAddress1, (byte *)buffer, buffer_count, HAL_MAX_DELAY) != HAL_OK)
            return 0;
        break;
    default:
        return 0;
    }
    return 1;
}
#endif