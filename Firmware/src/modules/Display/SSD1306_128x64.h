#include "display.h"

#include <stdio.h>
#include <string.h>

#include "u8g2.h"
#include "custom_fonts.h"

class SSD1306_128x64 : public Display
{
public:
    SSD1306_128x64() : Display{}
    {
    }

    void Init(u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb) override
    {
#if USING(INVERT_SCREEN)
        u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R2, byte_cb, gpio_and_delay_cb);
#else
        u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, byte_cb, gpio_and_delay_cb);
#endif
        u8g2_InitDisplay(&u8g2);
        u8g2_ClearBuffer(&u8g2);
        u8g2_SendBuffer(&u8g2);
        u8g2_SetPowerSave(&u8g2, 0);
    }
    void Off() override
    {
        u8g2_SetPowerSave(&u8g2, 1);
    }
    void On() override
    {
        u8g2_SetPowerSave(&u8g2, 0);
    }
    void Clear() override
    {
        u8g2_ClearBuffer(&u8g2);
        u8g2_SendBuffer(&u8g2);
    }
    void ClearBuffer() override
    {
        u8g2_ClearBuffer(&u8g2);
    }
    void Update() override
    {
        u8g2_SendBuffer(&u8g2);
    }

    void BootScreen() override
    {
        u8g2_SetFont(&u8g2, boot_animation);
        for (uint8_t i = 0; i < 1; i++)
        {
            for (uint8_t e = 48; e <= 75; e++)
            {
                u8g2_ClearBuffer(&u8g2);
                u8g2_DrawGlyph(&u8g2, 46, 40, e);
                u8g2_SendBuffer(&u8g2);
                HAL_Delay(5);
            }
            HAL_Delay(50);
        }
    }
    void ErrorScreen(const char *error) override
    {
        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
        u8g2_SetDrawColor(&u8g2, 1);

        u8g2_DrawStr(&u8g2, 44, 24, "ERROR!");
        u8g2_DrawStr(&u8g2, 4 + ((15 - strlen(error)) * 4), 48, error);
    }
    void ModeSelectScreen(byte mode) override
    {
        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
        u8g2_SetDrawColor(&u8g2, 1);

        u8g2_DrawGlyph(&u8g2, 2, 36, 0x25c0);
        u8g2_DrawGlyph(&u8g2, 119, 36, 0x25b6);

        if (mode == 0)
        {
            // Auto
            u8g2_DrawStr(&u8g2, 48, 36, "Auto");
        }
        else
        {
            // Manual
            u8g2_DrawStr(&u8g2, 40, 36, "Manual");
        }
    }
    void CoolDownScreen(int temperature, int fanState) override
    {
        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
        u8g2_SetDrawColor(&u8g2, 1);

        u8g2_DrawStr(&u8g2, 14, 22, "Wait Safe Temp");

        char buffer[9] = {0};

        sprintf(buffer, "%d", temperature);

        int offset = 0;

        if (temperature >= 100)
            offset += 4;
        u8g2_DrawStr(&u8g2, 50 - offset, 52, buffer);
        u8g2_DrawGlyph(&u8g2, 70 + offset, 52, 0x2103);

        if (fanState == 1)
        {
            u8g2_DrawGlyph(&u8g2, 88 + offset, 52, 0x2602);
        }
    }
    void ProfileSelectScreen(byte *profile, int profileLength, char *profileName) override
    {
        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
        u8g2_SetDrawColor(&u8g2, 1);

        u8g2_DrawGlyph(&u8g2, 2, 63, 0x25c0);
        u8g2_DrawGlyph(&u8g2, 119, 63, 0x25b6);

        // 10 Chars max
        const int maxLength = 10;
        char buffer[maxLength + 1] = {0};

        int nameLength = strlen((const char *)profileName);
        strncpy(buffer, profileName, nameLength > maxLength ? maxLength : nameLength);

        u8g2_DrawStr(&u8g2, 24 + ((maxLength - nameLength) * 4), 62, buffer);

        const double factor = 42.0 / 180.0;

        int x2 = -1;
        int y2 = -1;
        for (int i = 0; i < profileLength; i++)
        {

            int x1 = (i * 6) + 4;
            int y1 = 45 - (profile[i] * factor);
            u8g2_DrawDisc(&u8g2, x1, y1, 1, U8G2_DRAW_ALL);
            if (x2 > 0 && y2 > 0)
            {
                u8g2_DrawLine(&u8g2, x1, y1, x2, y2);
            }
            x2 = x1;
            y2 = y1;
        }

        u8g2_DrawGlyph(&u8g2, 112, 12, 0x25b7);
    }
    void ManualRunScreen(int temperature, int target) override
    {
        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
        u8g2_SetDrawColor(&u8g2, 1);
        u8g2_DrawGlyph(&u8g2, 2, 34, 0x25c0);
        u8g2_DrawGlyph(&u8g2, 119, 34, 0x25b6);

        const int maxLength = 8;
        char buffer[maxLength + 1] = {0};

        sprintf(buffer, "Set:%d", target);

        int strLength = strlen(buffer);

        // Setpoint
        u8g2_DrawStr(&u8g2, 30 + ((maxLength - 1 - strLength) * 4), 26, buffer);
        u8g2_DrawGlyph(&u8g2, 30 + ((maxLength - strLength) * 2) + 56, 26, 0x2103);

        // Current
        sprintf(buffer, "  T:%d", temperature);
        strLength = strlen(buffer);

        u8g2_DrawStr(&u8g2, 20 + ((maxLength - 1 - strLength) * 4), 52, buffer);
        u8g2_DrawGlyph(&u8g2, 20 + ((maxLength - strLength) * 2) + 56, 52, 0x2103);

        u8g2_DrawGlyph(&u8g2, 108, 56, 0x2615);

        u8g2_DrawGlyph(&u8g2, 2, 62, 0x25c1);
    }

    void AutoRunPreheatScreen(int target, int temperature) override
    {
        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
        u8g2_SetDrawColor(&u8g2, 1);

        u8g2_DrawStr(&u8g2, 14, 22, "Preheating...");

        char buffer[9] = {0};

        sprintf(buffer, "%d->%d", temperature, target);

        int offset = 0;

        if (target >= 100)
            offset += 4;
        if (temperature >= 100)
            offset += 4;
        u8g2_DrawStr(&u8g2, 32 - offset, 52, buffer);
        u8g2_DrawGlyph(&u8g2, 88 + offset, 52, 0x2103);

        u8g2_DrawGlyph(&u8g2, 2, 62, 0x25c1);
    }
    void AutoRunScreen(double target, uint16_t *history, byte historyIndex, int historyLength, uint64_t time) override
    {
        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
        u8g2_SetDrawColor(&u8g2, 1);

        int hMax = 80;

        for (int i = 0; i < historyIndex; i++)
        {
            if (history[i] > hMax)
                hMax = history[i];
        }

        double hFactor = 25.0 / (double)hMax;
        int xFactor = 200 / (historyLength - 1);
        int x2 = -1;
        int y2 = -1;
        for (int i = 0; i < historyIndex; i++)
        {
            int x1 = (i * xFactor) + 2;
            int y1 = 50 - (history[i] * hFactor);
            u8g2_DrawCircle(&u8g2, x1, y1, 1, U8G2_DRAW_ALL);
            if (x2 > 0 && y2 > 0)
            {
                u8g2_DrawLine(&u8g2, x1, y1, x2, y2);
            }
            x2 = x1;
            y2 = y1;
        }

        char buffer[12] = {0};
        sprintf(buffer, "%d/%d", history[historyIndex], (int)target);

        int offset = 0;

        if (history[historyIndex] >= 100)
            offset += 4;
        if (target >= 100)
            offset += 4;

        u8g2_DrawStr(&u8g2, 20, 62, buffer);
        u8g2_DrawGlyph(&u8g2, 0, 63, 0x2615);
        u8g2_DrawGlyph(&u8g2, 68 + offset, 63, 0x2103);

        buffer[0] = 0;
        buffer[1] = 0;
        buffer[2] = 0;
        buffer[3] = 0;
        buffer[4] = 0;
        buffer[5] = 0;
        buffer[6] = 0;
        buffer[7] = 0;
        buffer[8] = 0;
        buffer[9] = 0;
        buffer[10] = 0;
        buffer[11] = 0;

        if (time >= 1000)
            time = 999;
        sprintf(buffer, "%ds", (int)time);
        u8g2_DrawStr(&u8g2, 92, 62, buffer);

        u8g2_DrawGlyph(&u8g2, 6, 10, 0x25c1);
    }
    void DoneScreen(uint64_t time) override
    {
        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
        u8g2_SetDrawColor(&u8g2, 1);

        u8g2_DrawGlyph(&u8g2, 34, 28, 0x2714);
        u8g2_DrawStr(&u8g2, 54, 28, "Done");

        char buffer[12] = {0};

        sprintf(buffer, "Time: %ds", (int)time);

        u8g2_DrawStr(&u8g2, 32, 52, buffer);
    }

private:
    u8g2_t u8g2;
};
