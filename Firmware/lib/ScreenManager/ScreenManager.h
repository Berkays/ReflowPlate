#ifndef __SCREENMANAGER_H
#define __SCREENMANAGER_H

#define MAX_SCREEN_COUNT 10

#include <stdint.h>

#include "Types.h"
#include "ScreenId.h"
#include "Screen.h"

class ScreenManager
{
private:
    ScreenManager();
    inline static Screen *screens[MAX_SCREEN_COUNT];
    inline static byte screenCount = 0;
    inline static byte currentScreen = 0;
    inline static ScreenId queuedScreenId = ScreenId::None;

public:
    static void Start();
    static void Add(Screen *);
    static void Switch(ScreenId);
};

#endif
