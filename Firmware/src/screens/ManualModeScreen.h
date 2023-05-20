#ifndef __MANUALMODESCREEN_H
#define __MANUALMODESCREEN_H

#include <ScreenManager.h>
#include <TaskScheduler.h>
#include <ButtonManager.h>

#include "global.h"
#include "display.h"
#include "analog.h"
#include "heater.h"

using namespace mdl::temp;

class ManualModeScreen : public Screen
{
public:
    ManualModeScreen() : Screen{ScreenId::ManualMode}
    {
    }

    void Setup() override
    {
        sourceVoltage = 12.0;
        lastEncoderTick = 0;
        Output = 0;
        Target = 150;

        // Read vdiv
        uint32_t adcValue = Read_ADC(vdiv_channel, ADC_SAMPLETIME_239CYCLES_5);
        sourceVoltage = ADC_TO_VOLTAGE_DIVIDER(adcValue, VDIV_R1, VDIV_R2);

        // Read ntc
        double ntcTemperature = ntc::Read(ntc_1_channel);

        adjustCurrentClamp(sourceVoltage, ntcTemperature, &minOutputClamp, &maxOutputClamp);

        display->Clear();
        display->ManualRunScreen(ntcTemperature, Target);
        display->Update();

        ButtonManager::Init();
        btn1.Reset();
        btn1.onClick = ManualModeScreen::onClick;

        __HAL_TIM_SET_AUTORELOAD(encoder_timer, MAX_TEMP - MIN_TEMP + (ENCODER_THRESHOLD * 2)); // ARR
        __HAL_TIM_SET_COUNTER(encoder_timer, Target - ENCODER_THRESHOLD * 2);
        HAL_TIM_Encoder_Start(encoder_timer, encoder_channels);

        TaskScheduler::SchedulePeriodicTask(ManualModeScreen::ntcTask, TEMP_UPDATE_INTERVAL, 500);
        TaskScheduler::SchedulePeriodicTask(ManualModeScreen::heaterTask, HEAT_UPDATE_INTERVAL, 1500);
    }

    ScreenSignal Update() override
    {
        TaskScheduler::Check();
        ButtonManager::Check();
        checkEncoder();

        return ScreenSignal::None;
    }

private:
    uint32_t lastEncoderTick = 0;
    static inline double sourceVoltage = 12.0;
    static inline uint16_t minOutputClamp = 0xffff;
    static inline uint16_t maxOutputClamp = 0xffff;

    inline void checkEncoder()
    {
        uint32_t tick = HAL_GetTick();
        if (tick - lastEncoderTick > 75)
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

            int prevTarget = (int)Target;
            Target = CNT - ENCODER_THRESHOLD + MIN_TEMP;
            if (Target > MAX_TEMP)
                Target = MAX_TEMP;
            if (Target < MIN_TEMP)
                Target = MIN_TEMP;

            if (prevTarget == (int)Target)
            {
                lastEncoderTick = tick;
                return;
            }

            display->ClearBuffer();
            display->ManualRunScreen(Temperature, Target);
            display->Update();
            lastEncoderTick = tick;
        }
    }

    static inline void onClick()
    {
        TaskScheduler::RemoveAll();

        HAL_TIM_Encoder_Stop(encoder_timer, encoder_channels);

        btn1.Reset();

        stopHeater();
        ScreenManager::Switch(ScreenId::ModeSelect);
    }

    static inline byte ntcTask()
    {
        Temperature = ntc::Read(ntc_1_channel);
        adjustCurrentClamp(sourceVoltage, Temperature, &minOutputClamp, &maxOutputClamp);

        pid.Compute();

        display->ClearBuffer();
        display->ManualRunScreen(Temperature, Target);
        display->Update();

        return 0;
    }

    static inline byte heaterTask()
    {

        stopHeater();
        if (Output <= 0.1)
            return 0;

        uint16_t DutyCycle = maxOutputClamp * Output;

        if (DutyCycle < minOutputClamp)
            return 0;

        __HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, (uint16_t)(DutyCycle));
        HAL_TIM_PWM_Start(pwm_timer, pwm_channel);

        return 0;
    }
};

#endif
