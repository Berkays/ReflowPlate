#ifndef __MODESCREEN_H
#define __MODESCREEN_H

#include <ScreenManager.h>
#include <ButtonManager.h>

#include "global.h"
#include "display.h"

class ModeSelectionScreen : public Screen
{
public:
    ModeSelectionScreen() : Screen{ScreenId::ModeSelect}
    {
    }

    void Setup() override
    {
        lastEncoderTick = 0;
        selectedMode = 0;

        TOGGLE_STATUS_LED(LOW);

        display->Clear();
        display->ModeSelectScreen(0);
        display->Update();

        ButtonManager::Init();
        btn1.Reset();
        btn1.onClick = ModeSelectionScreen::onClick;

        __HAL_TIM_SET_AUTORELOAD(encoder_timer, 3); // ARR
        __HAL_TIM_SET_COUNTER(encoder_timer, 0);    // CNT
        HAL_TIM_Encoder_Start(encoder_timer, encoder_channels);
    }

    ScreenSignal Update() override
    {
        ButtonManager::Check();
        checkEncoder();

        return ScreenSignal::None;
    }

private:
    uint32_t lastEncoderTick = 0;
    static inline byte selectedMode = 0;

    inline void checkEncoder()
    {
        uint32_t tick = HAL_GetTick();
        if (tick - lastEncoderTick > 25)
        {
            uint32_t CNT = __HAL_TIM_GET_COUNTER(encoder_timer) / 2;

            if (selectedMode == CNT)
            {
                lastEncoderTick = tick + 50;
                return;
            }

            selectedMode = CNT;
            display->ClearBuffer();
            display->ModeSelectScreen(selectedMode);
            display->Update();

            lastEncoderTick = tick + 50;
        }
    }

    static inline void onClick()
    {
        // Omit ErrorScreen return events
        if (ErrorScreen::GetError() != Errors::None)
        {
            ErrorScreen::SetError(Errors::None);
            return;
        }

        HAL_TIM_Encoder_Stop(encoder_timer, encoder_channels);

        btn1.Reset();

#if USING(WAIT_SAFE_TEMP)
        if (selectedMode == 0 && Temperature > MAX_SAFE_TEMP)
        {
            ScreenManager::Switch(ScreenId::Cooldown);
            return;
        }
#endif

        if (selectedMode == 0)
            ScreenManager::Switch(ScreenId::ProfileSelect);
        else
            ScreenManager::Switch(ScreenId::ManualMode);
    }
};

#endif
