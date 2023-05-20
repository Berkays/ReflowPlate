#ifndef __COOLDOWNSCREEN_H
#define __COOLDOWNSCREEN_H

#include <ScreenManager.h>
#include <TaskScheduler.h>

#include "global.h"
#include "display.h"

using namespace mdl::temp;

class CooldownScreen : public Screen
{
public:
    CooldownScreen() : Screen{ScreenId::Cooldown}
    {
    }

    void Setup() override
    {
#if USING(FAN_COOLDOWN)
        display->ClearBuffer();
        display->CoolDownScreen(Temperature, 1);
        display->Update();

        HAL_GPIO_WritePin(Fan_Port, Fan_Pin, GPIO_PIN_SET);

        TaskScheduler::SchedulePeriodicTask(cooldownTask, 2000, 1000);
#else
        display->ClearBuffer();
        display->CoolDownScreen(Temperature, 0);
        display->Update();

        HAL_Delay(2500);

        ScreenManager::Switch(ScreenId::ModeSelect);
#endif
    }

    ScreenSignal Update() override
    {
        TaskScheduler::Check();

        return ScreenSignal::None;
    }

private:
    static inline byte cooldownTask()
    {
        double ntcTemperature = ntc::Read(ntc_1_channel);

        display->ClearBuffer();
        display->CoolDownScreen(Temperature, 1);
        display->Update();

        if (ntcTemperature < MAX_SAFE_TEMP)
        {
            TaskScheduler::RemoveAll();

            HAL_GPIO_WritePin(Fan_Port, Fan_Pin, GPIO_PIN_RESET);
            ScreenManager::Switch(ScreenId::ModeSelect);
        }

        return 0;
    }
};

#endif
