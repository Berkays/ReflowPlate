#include "peripherals.h"
#include "global.h"
#include "display.h"

#include "SSD1306_128x64.h"

TIM_HandleTypeDef *pwm_timer = &htim1;
unsigned int pwm_channel = TIM_CHANNEL_3;
TIM_HandleTypeDef *encoder_timer = &htim3;
unsigned int encoder_channel_1 = TIM_CHANNEL_1;
unsigned int encoder_channel_2 = TIM_CHANNEL_2;
unsigned int encoder_channels = encoder_channel_1 | encoder_channel_2;
TIM_HandleTypeDef *tick_timer = &htim6;
TIM_HandleTypeDef *i2c_timer = &htim15;
I2C_HandleTypeDef *i2c = &hi2c2;
UART_HandleTypeDef *uart = &huart1;
unsigned int vdiv_channel = ADC_CHANNEL_1;
unsigned int ntc_1_channel = ADC_CHANNEL_3;
unsigned int ntc_2_channel = ADC_CHANNEL_2;
GPIO_TypeDef *Btn1_Port = SW_GPIO_Port;
unsigned short Btn1_Pin = SW_Pin;
GPIO_TypeDef *Fan_Port = FAN_GPIO_Port;
unsigned short Fan_Pin = FAN_Pin;

Display *display = new SSD1306_128x64();