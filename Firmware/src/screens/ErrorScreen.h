#ifndef __ERRORSCREEN_H
#define __ERRORSCREEN_H

#include <ScreenManager.h>
#include <TaskScheduler.h>

#include "global.h"
#include "display.h"

class ErrorScreen : public Screen
{
public:
    ErrorScreen() : Screen{ScreenId::Error}
    {
    }

    void Setup() override
    {
        display->ClearBuffer();

        switch (error)
        {
        case Errors::NTC_FAULT:
            display->ErrorScreen(ntc_error);
            break;
        case Errors::VOLTAGE_FAULT:
            display->ErrorScreen(voltage_error);
            break;
        case Errors::THERMALRUNAWAY_FAULT:
            display->ErrorScreen(runaway_error);
            break;

        default:
            break;
        }

        display->Update();

        blink_task2();

        ButtonManager::Init();
        btn1.Reset();
        btn1.holdDuration = 2000;
        btn1.onHold = onHold;
    }

    ScreenSignal Update() override
    {
        TaskScheduler::Check();
        ButtonManager::Check();

        return ScreenSignal::None;
    }

    static inline void SetError(Errors _error)
    {
        error = _error;
    }

    static inline Errors GetError()
    {
        return error;
    }

private:
    static inline Errors error = Errors::None;

    const char *ntc_error = "NTC Fault";
    const char *voltage_error = "Invalid Voltage";
    const char *runaway_error = "Thermal Runaway";

    static inline byte blink_task1()
    {
        display->Off();
        TaskScheduler::ScheduleTask(blink_task2, 300);
        return 0;
    }
    static inline byte blink_task2()
    {
        display->On();
        TaskScheduler::ScheduleTask(blink_task1, 1000);
        return 0;
    }

    static inline void onHold()
    {
        TaskScheduler::RemoveAll();

        display->On();
        ScreenManager::Switch(ScreenId::ModeSelect);
    }
};

#endif
