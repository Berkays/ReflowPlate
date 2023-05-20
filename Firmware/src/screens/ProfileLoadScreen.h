#ifndef __PROFILELOADSCREEN_H
#define __PROFILELOADSCREEN_H

#include <ScreenManager.h>
#include <TaskScheduler.h>

#include "global.h"
#include "display.h"
#include "serial.h"

class ProfileLoadScreen : public Screen
{
public:
    ProfileLoadScreen() : Screen{ScreenId::ProfileLoad}
    {
    }

    void Setup() override
    {
        display->Clear();
        display->Update();

        TaskScheduler::ScheduleTask(switchScreenTask, 60000 * 5); // Timeout after 5 minutes
    }

    ScreenSignal Update() override
    {
        TaskScheduler::Check();

        if (checkSerial(CODE_HOST_EXIT_PROFILE) == 2)
        {
            TaskScheduler::RemoveAll();
            ScreenManager::Switch(ScreenId::ProfileSelect);
        }

        return ScreenSignal::None;
    }

private:
    static inline byte switchScreenTask()
    {
        ScreenManager::Switch(ScreenId::ProfileSelect);
        return 0;
    }
};

#endif
