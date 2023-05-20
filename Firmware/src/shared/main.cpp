#include "main.h"

#include <ScreenManager.h>
#include <ButtonManager.h>
#include <TaskScheduler.h>

#include "global.h"
#include "display_interface.h"

// Screens
#include "BootScreen.h"
#include "ModeSelectionScreen.h"
#include "CooldownScreen.h"
#include "AutomaticModeScreen.h"
#include "DoneScreen.h"
#include "ErrorScreen.h"
#include "ManualModeScreen.h"
#include "ProfileSelectionScreen.h"

int main(void)
{
    InitPeripherals();
    HAL_ADCEx_Calibration_Start(&hadc);

#if USING(INVERT_ENCODER)
    __HAL_TIM_SET_CAPTUREPOLARITY(encoder_timer, encoder_channel_1, TIM_ICPOLARITY_FALLING);
#endif

    __HAL_TIM_ENABLE(i2c_timer);

    // Initialize display
    display->Init(u8x8_byte_hw_i2c, u8x8_gpio_and_delay_template);

    // Initialize PID
    pid.SetMode(QuickPID::Control::timer);
    pid.SetOutputLimits(0, 1);
    pid.SetSampleTimeUs(1);

    // Register button
    ButtonManager::Add(&btn1);

    BootScreen bootScreen;
    ModeSelectionScreen modeScreen;
    CooldownScreen coolDownScreen;
    ProfileSelectionScreen profileScreen;
    AutomaticModeScreen autoModeScreen;
    ManualModeScreen manualModeScreen;
    ErrorScreen errorScreen;
    DoneScreen doneScreen;

    ScreenManager::Add(&bootScreen);
    ScreenManager::Add(&modeScreen);
    ScreenManager::Add(&coolDownScreen);
    ScreenManager::Add(&profileScreen);
    ScreenManager::Add(&autoModeScreen);
    ScreenManager::Add(&manualModeScreen);
    ScreenManager::Add(&errorScreen);
    ScreenManager::Add(&doneScreen);

    ScreenManager::Start();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == tick_timer && clockInterruptHandler != nullptr)
        clockInterruptHandler();
}