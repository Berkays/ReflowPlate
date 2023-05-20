#ifndef __SCREENID_H
#define __SCREENID_H

enum class ScreenId
{
    None = 0,
    Boot = 1,
    ModeSelect = 2,
    ProfileSelect = 3,
    AutomaticMode = 4,
    ManualMode = 5,
    Cooldown = 6,
    ProfileLoad = 7,
    Options = 8,
    Done = 254,
    Error = 255
};

#endif