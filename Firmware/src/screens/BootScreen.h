#ifndef __BOOTSCREEN_H
#define __BOOTSCREEN_H

#include "Errors.h"

#include <ScreenManager.h>
#include "ErrorScreen.h"

#include "global.h"
#include "analog.h"
#include "display.h"
#include "temperature.h"

using namespace mdl::temp;

class BootScreen : public Screen
{
public:
    BootScreen() : Screen{ScreenId::Boot}
    {
    }

    void Setup() override
    {
        display->Clear();
        display->BootScreen();

        // Read vdiv
        uint32_t adcValue = Read_ADC(vdiv_channel, ADC_SAMPLETIME_239CYCLES_5);

        double sourceVoltage = ADC_TO_VOLTAGE_DIVIDER(adcValue, VDIV_R1, VDIV_R2);

        if (sourceVoltage < MIN_ALLOWED_VOLTAGE || sourceVoltage > MAX_ALLOWED_VOLTAGE)
        {
            ErrorScreen::SetError(Errors::VOLTAGE_FAULT);
            ScreenManager::Switch(ScreenId::Error);
            return;
        }

        // Read ntc
        Temperature = ntc::Read(ntc_1_channel);

        if (Temperature < 0 || Temperature > 250)
        {
            ErrorScreen::SetError(Errors::NTC_FAULT);
            ScreenManager::Switch(ScreenId::Error);
            return;
        }

        ScreenManager::Switch(ScreenId::ModeSelect);
    }

    ScreenSignal Update() override
    {
        return ScreenSignal::Exit;
    }
};

#endif
