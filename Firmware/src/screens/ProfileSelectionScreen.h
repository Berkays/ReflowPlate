#ifndef __PROFILESCREEN_H
#define __PROFILESCREEN_H

#include <ScreenManager.h>
#include <ButtonManager.h>

#include "global.h"
#include "display.h"
#include "eeprom.h"
#include "serial.h"

class ProfileSelectionScreen : public Screen
{
public:
    ProfileSelectionScreen() : Screen{ScreenId::ProfileSelect}
    {
    }

    void Setup() override
    {
        lastEncoderTick = 0;

        display->ClearBuffer();
        display->ProfileSelectScreen(ReflowProfile, REFLOW_POINT_COUNT, ReflowProfileName);
        display->Update();

        ButtonManager::Init();
        btn1.Reset();
        btn1.onClick = ProfileSelectionScreen::onClick;

        __HAL_TIM_SET_AUTORELOAD(encoder_timer, 0 + (ENCODER_THRESHOLD * 2)); // ARR
        __HAL_TIM_SET_COUNTER(encoder_timer, ENCODER_THRESHOLD);              // CNT
        HAL_TIM_Encoder_Start(encoder_timer, encoder_channels);
    }

    ScreenSignal Update() override
    {
        ButtonManager::Check();
        checkEncoder();

        if (checkSerial(CODE_HOST_ENTER_PROFILE) == 1)
        {
            HAL_TIM_Encoder_Stop(encoder_timer, encoder_channels);

            btn1.Reset();

            ScreenManager::Switch(ScreenId::ProfileLoad);
        }

        return ScreenSignal::None;
    }

private:
    uint32_t lastEncoderTick = 0;

    inline void checkEncoder()
    {
        uint32_t tick = HAL_GetTick();
        if (tick - lastEncoderTick > 100)
        {
            uint32_t CNT = __HAL_TIM_GET_COUNTER(encoder_timer);
            uint32_t ARR = __HAL_TIM_GET_AUTORELOAD(encoder_timer);
            if (CNT < ENCODER_THRESHOLD)
            {
                CNT = ENCODER_THRESHOLD;
                __HAL_TIM_SET_COUNTER(encoder_timer, CNT);
            }
            else if (CNT > ARR - ENCODER_THRESHOLD)
            {
                CNT = ARR - ENCODER_THRESHOLD;
                __HAL_TIM_SET_COUNTER(encoder_timer, CNT);
            }

            display->ClearBuffer();
            display->ProfileSelectScreen(ReflowProfile, REFLOW_POINT_COUNT, ReflowProfileName);
            display->Update();

            lastEncoderTick = tick;
        }
    }

    static inline void onClick()
    {
        HAL_TIM_Encoder_Stop(encoder_timer, encoder_channels);

        btn1.Reset();

        ScreenManager::Switch(ScreenId::AutomaticMode);
    }
};

#endif
