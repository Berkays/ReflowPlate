#ifndef __AUTOMODESCREEN_H
#define __AUTOMODESCREEN_H

#include <ScreenManager.h>

#include "global.h"
#include "display.h"
#include "heater.h"
#include "serial.h"
#include "analog.h"

using namespace mdl::temp;

class AutomaticModeScreen : public Screen
{
public:
    AutomaticModeScreen() : Screen{ScreenId::AutomaticMode}
    {
    }

    void Setup() override
    {
        sourceVoltage = 12.0;
        Output = 0;
        Time = 0;
        Target = 0;
        currentSegment = 0;
        isPreheating = true;
        preheatCounter = 0;
        m = 0;
        n = 0;
        initialTemperature = 0;
        timerTickCounter = 0;

        for (byte i = 0; i < REFLOW_DURATION / AUTOMATIC_MODE_HISTORY_RESOLUTION; i++)
            history[i] = 0;
        historyIndex = 0;

        // Read vdiv
        uint32_t adcValue = Read_ADC(vdiv_channel, ADC_SAMPLETIME_239CYCLES_5);
        sourceVoltage = ADC_TO_VOLTAGE_DIVIDER(adcValue, VDIV_R1, VDIV_R2);

        // Read ntc
        double ntcTemperature = ntc::Read(ntc_1_channel);
        initialTemperature = (int)ntcTemperature;

        adjustCurrentClamp(sourceVoltage, ntcTemperature, &minOutputClamp, &maxOutputClamp);

        history[0] = ntcTemperature;

        ButtonManager::Init();
        btn1.Reset();
        btn1.onClick = AutomaticModeScreen::onClick;

        Target = ReflowProfile[0];

#if !USING(PREHEAT)
        display->ClearBuffer();
        display->AutoRunScreen(Target, history, 0, REFLOW_DURATION / AUTOMATIC_MODE_HISTORY_RESOLUTION, 0);
        display->Update();
#endif

        TaskScheduler::SchedulePeriodicTask(ntcTask, TEMP_UPDATE_INTERVAL, 500);
        TaskScheduler::SchedulePeriodicTask(heaterTask, HEAT_UPDATE_INTERVAL, 1500);
        TaskScheduler::SchedulePeriodicTask(preheatTask, 2500, 1000);
        TaskScheduler::ScheduleTask(runawayTask, THERMAL_RUNAWAY_TIMESPAN);
    }

    ScreenSignal Update() override
    {
        TaskScheduler::Check();
        ButtonManager::Check();

        return ScreenSignal::None;
    }

private:
    static inline double sourceVoltage = 12.0;
    static inline uint16_t minOutputClamp = 0xffff;
    static inline uint16_t maxOutputClamp = 0xffff;

    static inline uint16_t history[REFLOW_DURATION / AUTOMATIC_MODE_HISTORY_RESOLUTION] = {0};
    static inline byte historyIndex = 0;

    static inline bool isPreheating = true;
    static inline byte preheatCounter = 0;

    static inline int timerTickCounter = 0;
    static inline uint16_t currentSegment = 0;
    static inline double m = 0; // Slope C°/t
    static inline double n = 0;

    // Thermal Runaway
    static inline int initialTemperature = 25;

    static inline void timerAction()
    {
        history[historyIndex] = Temperature;

        uint16_t nextSegment = Time / REFLOW_RESOLUTION;

        if (historyIndex >= REFLOW_DURATION / AUTOMATIC_MODE_HISTORY_RESOLUTION || ReflowProfile[currentSegment] < MIN_TEMP)
        {
            Target = 0;
            Time += 1;

            sendStatus(Time, 0, 0, 0);
            TaskScheduler::RemoveAll();

            HAL_TIM_Base_Stop_IT(tick_timer);
            clockInterruptHandler = nullptr;

            btn1.Reset();

            stopHeater();
            ScreenManager::Switch(ScreenId::Done);
            return;
        }

        // Recalculate temperature slope for new segment
        if (nextSegment > currentSegment)
        {
            currentSegment = nextSegment;

            uint16_t t1 = currentSegment * REFLOW_RESOLUTION;
            uint16_t t2 = t1 + REFLOW_RESOLUTION;

            // y = mx + n
            m = (double)(ReflowProfile[currentSegment] - ReflowProfile[currentSegment - 1]) / (double)(t2 - t1);
            n = ReflowProfile[currentSegment - 1] - (m * (double)t1);
        }

        // // Adjust target based on slope
        Target = (m * (double)Time) + n;

        // Initial segment
        if (Target == 0)
            Target = ReflowProfile[0];

        sendStatus(Time, Temperature, Target, Output);

        Time++;
        display->ClearBuffer();
        display->AutoRunScreen(Target, history, historyIndex, REFLOW_DURATION / AUTOMATIC_MODE_HISTORY_RESOLUTION, Time);
        display->Update();

        if (timerTickCounter++ >= AUTOMATIC_MODE_HISTORY_RESOLUTION)
        {
            timerTickCounter = 0;
            historyIndex++;
        }
    }

    static inline void onClick()
    {
        TaskScheduler::RemoveAll();

        HAL_TIM_Base_Stop_IT(tick_timer);
        clockInterruptHandler = nullptr;

        btn1.Reset();

        stopHeater();
        ScreenManager::Switch(ScreenId::ModeSelect);
    }

    static inline byte ntcTask()
    {
        Temperature = ntc::Read(ntc_1_channel, THERMISTOR_NUM_SAMPLES);
        adjustCurrentClamp(sourceVoltage, Temperature, &minOutputClamp, &maxOutputClamp);

        pid.Compute();

        return 0;
    }

    static inline byte heaterTask()
    {
        if (Output <= 0.1)
        {
            stopHeater();
            return 0;
        }

        uint16_t DutyCycle = maxOutputClamp * Output;

        if (DutyCycle < minOutputClamp)
            return 0;

        __HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, (uint16_t)(DutyCycle));
        HAL_TIM_PWM_Start(pwm_timer, pwm_channel);

        return 0;
    }

    static inline byte runawayTask()
    {
        if (Temperature <= Target - 2 && Temperature < initialTemperature + THERMAL_RUNAWAY_THRESHOLD)
        {
            TaskScheduler::RemoveAll();

            HAL_TIM_Base_Stop_IT(tick_timer);
            clockInterruptHandler = nullptr;

            btn1.Reset();

            stopHeater();

            ErrorScreen::SetError(Errors::THERMALRUNAWAY_FAULT);
            ScreenManager::Switch(ScreenId::Error);

            return 0;
        }

        initialTemperature = Temperature;
        TaskScheduler::ScheduleTask(AutomaticModeScreen::runawayTask, THERMAL_RUNAWAY_TIMESPAN);

        return 0;
    }

    static inline byte preheatTask()
    {
#if USING(PREHEAT)
        display->ClearBuffer();
        display->AutoRunPreheatScreen(Target, Temperature);
        display->Update();

        int diff = Temperature - ReflowProfile[0];
        if (diff > -1 * THERMAL_RUNAWAY_THRESHOLD && diff < THERMAL_RUNAWAY_THRESHOLD)
        {
            if (preheatCounter++ >= 3)
            {
                TaskScheduler::RemoveTask(preheatTask);
                isPreheating = false;

                clockInterruptHandler = AutomaticModeScreen::timerAction;
                HAL_TIM_Base_Start_IT(tick_timer);
            }
        }
        else
        {
            preheatCounter = 0;
        }
#else
        TaskScheduler::RemoveTask(preheatTask);
        isPreheating = false;

        clockInterruptHandler = AutomaticModeScreen::timerAction;
        HAL_TIM_Base_Start_IT(tick_timer);
#endif
        return 0;
    }
};

#endif
