#include "ScreenManager.h"

void ScreenManager::Add(Screen *screen)
{
    screens[screenCount++] = screen;
}

void ScreenManager::Start()
{
    while (true)
    {
        Screen *screen = screens[currentScreen];

        screen->Setup();

        ScreenSignal returnCode = ScreenSignal::None;
        while (returnCode == ScreenSignal::None && queuedScreenId == ScreenId::None)
        {
            returnCode = screen->Update();
        }

        if (queuedScreenId == ScreenId::None)
        {
            if (currentScreen + 1 >= screenCount)
                break;

            // Sequential Switch
            currentScreen++;
            continue;
        }

        // Manual Switch
        for (byte i = 0; i < screenCount; i++)
        {
            if (screens[i]->getScreenId() == queuedScreenId)
            {
                currentScreen = i;
                queuedScreenId = ScreenId::None;
                break;
            }
        }
    }
}

void ScreenManager::Switch(ScreenId screenId)
{
    queuedScreenId = screenId;
}