#ifndef __SCREEN_H
#define __SCREEN_H

#include "ScreenId.h"

enum class ScreenSignal
{
    None = 0,
    Exit = 1
};

class Screen
{
public:
    Screen(ScreenId _screenId)
    {
        screenId = _screenId;
    }

    virtual void Setup() = 0;
    virtual ScreenSignal Update() = 0;

    inline ScreenId getScreenId()
    {
        return screenId;
    }

private:
    ScreenId screenId = ScreenId::None;
};

#endif
