#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdint.h>
#include "hal_driver.h"
#include "Config.h"

#include <Button.h>
#include <QuickPID.h>

#include "display.h"

// External Mappings.
// Define in mappings.cpp
extern TIM_HandleTypeDef *pwm_timer;
extern unsigned int pwm_channel;
extern TIM_HandleTypeDef *encoder_timer;
extern unsigned int encoder_channels;
extern unsigned int encoder_channel_1;
extern unsigned int encoder_channel_2;
extern TIM_HandleTypeDef *tick_timer;
extern TIM_HandleTypeDef *i2c_timer;
extern I2C_HandleTypeDef *i2c;
extern UART_HandleTypeDef *uart;
extern unsigned int ntc_1_channel;
extern unsigned int ntc_2_channel;
extern unsigned int vdiv_channel;
extern GPIO_TypeDef *Btn1_Port;
extern unsigned short Btn1_Pin;
extern GPIO_TypeDef *Fan_Port;
extern unsigned short Fan_Pin;

extern Display *display;
// End of external mappings

// Heating
inline double Temperature = 25.0;
inline double Target = 50.0;
inline double Output = 0.0;
inline uint64_t Time = 0;

inline byte ReflowProfile[REFLOW_POINT_COUNT] = {60, 75, 100, 100, 120, 120, 130, 135, 150, 150, 160, 140, 0, 0, 0, 0, 0, 0, 0, 0};
inline char ReflowProfileName[11] = "Generic";
inline QuickPID pid(&Temperature, &Output, &Target, KP, KI, KD, QuickPID::Action::direct);

// IO
inline Button btn1(Btn1_Port, Btn1_Pin);

inline void (*clockInterruptHandler)(void) = nullptr;

#endif