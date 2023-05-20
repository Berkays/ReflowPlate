#include "eeprom.h"

void ReadProfile(byte *profile, char *profileName, byte index)
{
    uint32_t origin = GetProfileAddress(index);

    for (uint32_t i = 0; i < PROFILE_FLASH_DATA_SIZE; i += sizeof(uint32_t))
    {
        uint32_t val = *(__IO uint32_t *)origin;

        profile[i] = val >> 24 & 0xFF;
        profile[i + 1] = val >> 16 & 0xFF;
        profile[i + 2] = val >> 8 & 0xFF;
        profile[i + 3] = val & 0xFF;

        origin += sizeof(uint32_t);
    }

    origin += PROFILE_FLASH_DATA_OFFSET;

    for (uint32_t i = 0; i < PROFILE_FLASH_DATA_SIZE; i += sizeof(uint8_t))
    {
        uint16_t val = *(__IO uint16_t *)origin;

        profileName[i] = val >> 8 & 0xFF;
        profileName[i + 1] = val & 0xFF;

        origin += sizeof(uint8_t);
    }
}

byte ReadProfileCount()
{
    byte profileCount = 0;

    // Read all profile names, stop incrementing if name is FF FF .. .. etc
    for (byte i = 0; i < GetMaxProfileCount(); i++)
    {
        uint32_t origin = GetProfileAddress(i);
        uint32_t *nameAddress = (uint32_t *)(origin + PROFILE_FLASH_DATA_SIZE + PROFILE_FLASH_DATA_OFFSET);

        if (*nameAddress == 0xFF)
            break;

        profileCount++;
    }

    return profileCount;
}