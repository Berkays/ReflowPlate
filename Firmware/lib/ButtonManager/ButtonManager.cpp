#include "ButtonManager.h"

void ButtonManager::Init()
{
    ButtonManager::time = 0;
    ButtonManager::counter = 0;
    ButtonManager::lastButtonPressed = 0x00;
    ButtonManager::debounceButton = 0x00;
}

void ButtonManager::Add(Button *button)
{
    if (buttonCount >= BUTTON_MAX_COUNT)
        return;
    ButtonManager::buttons[buttonCount++] = button;
}

void ButtonManager::Check()
{
    uint32_t currentTime = HAL_GetTick();
    if (currentTime - ButtonManager::time > BUTTONMANAGER_SAMPLING_INTERVAL)
    {
        ButtonManager::time = currentTime;

        for (byte i = 0; i < buttonCount; i++)
        {
            Button *btn = ButtonManager::buttons[i];

            if (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET)
            {
                // If last pressed button is different
                if (ButtonManager::lastButtonPressed != btn)
                {
                    btn->isHeldDown = 0;
                    if (ButtonManager::debounceButton != btn)
                    {
                        ButtonManager::counter = 0;
                        ButtonManager::debounceButton = btn;
                    }
                    if (++(ButtonManager::counter) >= BUTTONMANAGER_DEBOUNCE)
                    {
                        ButtonManager::lastButtonPressed = btn;
                        ButtonManager::previousMillis = currentTime;
                    }
                }
                else if (btn->onHold != nullptr) // Same button as lastPressed
                {
                    if (btn->isHeldDown == 0)
                    {
                        int holdDuration = (currentTime - ButtonManager::previousMillis);

                        if (btn->onHoldProgress != nullptr && holdDuration > BUTTONMANAGER_HOLD_DETECT && btn->continousHold == 1)
                        {
                            int holdPercent = (127 * holdDuration) / btn->holdDuration;
                            btn->skipRelease = 1;
                            btn->onHoldProgress(holdPercent);
                        }

                        if (holdDuration > btn->holdDuration)
                        {
                            ButtonManager::buttons[i]->isHeldDown = 1;
                            btn->onHold();
                            ButtonManager::previousMillis = currentTime;
                            // return;
                        }
                    }
                }
                return;
            }
        }

        // If execution reaches this point then no button has been pressed during this check
        if (ButtonManager::lastButtonPressed != 0x00 && ButtonManager::lastButtonPressed->isHeldDown == 0)
        {
            if (ButtonManager::lastButtonPressed->skipRelease == 0 && ButtonManager::lastButtonPressed->onClick != nullptr)
                ButtonManager::lastButtonPressed->onClick();
        }

        if (ButtonManager::lastButtonPressed != 0x00)
            ButtonManager::lastButtonPressed->skipRelease = 0;

        ButtonManager::debounceButton = 0x00;
        ButtonManager::lastButtonPressed = 0x00;
    }
}