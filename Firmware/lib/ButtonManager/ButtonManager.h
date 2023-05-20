#ifndef __BUTTONMANAGER_H
#define __BUTTONMANAGER_H

#include "Types.h"
#include "hal_driver.h"

#include "Config.h"
#include "Button.h"

#ifndef BUTTONMANAGER_SAMPLING_INTERVAL
#define BUTTONMANAGER_SAMPLING_INTERVAL 15
#endif

#ifndef BUTTONMANAGER_DEBOUNCE
#define BUTTONMANAGER_DEBOUNCE 10
#endif

#ifndef BUTTONMANAGER_HOLD_DETECT
#define BUTTONMANAGER_HOLD_DETECT 250
#endif

class ButtonManager
{
public:
    static void Init();
    static void Check();
    static void Add(Button *button);

private:
    ButtonManager(){};
    static inline Button *buttons[BUTTON_MAX_COUNT];
    static inline byte buttonCount = 0;
    static inline Button *lastButtonPressed = 0;
    static inline Button *debounceButton = 0;
    static inline uint32_t time = 0;
    static inline uint32_t previousMillis = 0;
    static inline uint8_t counter = 0;
};

#endif