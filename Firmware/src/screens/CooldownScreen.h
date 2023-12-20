#ifndef __COOLDOWNSCREEN_H
#define __COOLDOWNSCREEN_H

#include <ScreenManager.h>
#include <TaskScheduler.h>

#include "pinUtil.h"
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

        TOGGLE_FAN(HIGH);

        TaskScheduler::SchedulePeriodicTask(cooldownTask, 2000, 1000);
#else
        display->ClearBuffer();
        display->CoolDownScreen(Temperature, 0);
        display->Update();

        HAL_Delay(2500);

        TOGGLE_STATUS_LED(HIGH);

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

            TOGGLE_FAN(LOW);
            TOGGLE_STATUS_LED(LOW);
            ScreenManager::Switch(ScreenId::ModeSelect);
        }

        return 0;
    }
};

#endif
