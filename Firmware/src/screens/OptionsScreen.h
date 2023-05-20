#ifndef __OPTIONSSCREEN_H
#define __OPTIONSSCREEN_H

#include <ScreenManager.h>
#include <Display.h>

class OptionsScreen : public Screen
{
public:
    OptionsScreen() : Screen{ScreenId::Options}
    {
    }

    void Setup() override
    {
    }

    ScreenSignal Update() override
    {
        return ScreenSignal::Exit;
    }
};

#endif
