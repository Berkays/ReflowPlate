#include "Button.h"

void Button::setPin(GPIO_TypeDef *port, uint16_t pin)
{
    this->port = port;
    this->pin = pin;
}

void Button::Reset()
{
    this->continousHold = 0;
    this->holdDuration = 0;
    this->isHeldDown = 0;
    this->skipRelease = 0;
    this->onClick = nullptr;
    this->onHold = nullptr;
    this->onHoldProgress = nullptr;
}

Button::Button(GPIO_TypeDef *port, uint16_t pin)
{
    setPin(port, pin);
    Reset();
}

Button::Button(GPIO_TypeDef *port, uint16_t pin, ButtonCallback onClick)
{
    setPin(port, pin);
    this->continousHold = 0;
    this->holdDuration = 0;
    this->isHeldDown = 0;
    this->skipRelease = 0;
    this->onClick = onClick;
    this->onHold = nullptr;
    this->onHoldProgress = nullptr;
}

Button::Button(GPIO_TypeDef *port, uint16_t pin, uint16_t holdDuration, ButtonCallback onClick, ButtonCallback onHold)
{
    setPin(port, pin);
    this->continousHold = 0;
    this->holdDuration = holdDuration;
    this->isHeldDown = 0;
    this->skipRelease = 0;
    this->onClick = onClick;
    this->onHold = onHold;
    this->onHoldProgress = nullptr;
}

Button::Button(GPIO_TypeDef *port, uint16_t pin, uint16_t holdDuration, ButtonCallback onClick, ButtonCallback onHold, ButtonIntCallback onHoldProgress)
{
    setPin(port, pin);
    this->continousHold = 1;
    this->holdDuration = holdDuration;
    this->isHeldDown = 0;
    this->skipRelease = 0;
    this->onClick = onClick;
    this->onHold = onHold;
    this->onHoldProgress = onHoldProgress;
}