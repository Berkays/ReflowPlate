#ifndef __DONESCREEN_H
#define __DONESCREEN_H

#include <ScreenManager.h>
#include <TaskScheduler.h>

#include "global.h"
#include "display.h"

class DoneScreen : public Screen
{
public:
    DoneScreen() : Screen{ScreenId::Done}
    {
    }

    void Setup() override
    {
        display->ClearBuffer();
        display->DoneScreen(Time);
        display->Update();

        TaskScheduler::ScheduleTask(switchScreenTask, 10000);
    }

    ScreenSignal Update() override
    {
        TaskScheduler::Check();

        return ScreenSignal::None;
    }

private:
    static inline byte switchScreenTask()
    {
        ScreenManager::Switch(ScreenId::ModeSelect);
        return 0;
    }
};

#endif
