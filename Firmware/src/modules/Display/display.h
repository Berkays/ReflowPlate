#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "hal_driver.h"
#include "Types.h"

#include "u8g2.h"

class Display
{
public:
    virtual void Init(u8x8_msg_cb, u8x8_msg_cb) = 0;
    virtual void On() = 0;
    virtual void Off() = 0;
    virtual void Clear() = 0;
    virtual void ClearBuffer() = 0;
    virtual void Update() = 0;

    virtual void BootScreen() = 0;
    virtual void ErrorScreen(const char *) = 0;
    virtual void ModeSelectScreen(byte) = 0;
    virtual void CoolDownScreen(int, int) = 0;
    virtual void ProfileSelectScreen(byte *, int, char *) = 0;
    virtual void ManualRunScreen(int, int) = 0;
    virtual void AutoRunScreen(double, uint16_t *, byte, int, uint64_t) = 0;
    virtual void AutoRunPreheatScreen(int, int) = 0;
    virtual void DoneScreen(uint64_t) = 0;
};

#endif