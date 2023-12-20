#ifndef __PERIPHERALS_H
#define __PERIPHERALS_H

#include "hal_driver.h"
#include "pins.h"

#ifdef __cplusplus
extern "C"
{
#endif
extern ADC_HandleTypeDef hadc;
extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim15;
extern UART_HandleTypeDef huart1;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
#ifdef __cplusplus
}
#endif

void InitPeripherals(void);
void Error_Handler(void);

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_ADC_Init(void);
void MX_I2C2_Init(void);
void MX_TIM1_Init(void);
void MX_TIM3_Init(void);
void MX_TIM6_Init(void);
void MX_TIM15_Init(void);
void MX_USART1_UART_Init(void);
#endif