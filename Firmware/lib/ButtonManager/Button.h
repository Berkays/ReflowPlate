#ifndef __BUTTON_H
#define __BUTTON_H

#include <stdint.h>
#include "hal_driver.h"

typedef void (*ButtonCallback)(void);
typedef void (*ButtonIntCallback)(int);

class Button
{
public:
    Button(GPIO_TypeDef *port, uint16_t pin);
    Button(GPIO_TypeDef *port, uint16_t pin, ButtonCallback onClick);
    Button(GPIO_TypeDef *port, uint16_t pin, uint16_t holdDuration, ButtonCallback onClick, ButtonCallback onHold);
    Button(GPIO_TypeDef *port, uint16_t pin, uint16_t holdDuration, ButtonCallback onClick, ButtonCallback onHold, ButtonIntCallback onHoldProgress);
    void Reset();

    GPIO_TypeDef *port;
    uint16_t pin;
    ButtonCallback onClick;
    ButtonCallback onHold;
    ButtonIntCallback onHoldProgress;
    uint16_t holdDuration;
    uint8_t isHeldDown;
    uint8_t continousHold;
    uint8_t skipRelease;

private:
    void setPin(GPIO_TypeDef *port, uint16_t pin);
};

#endif
