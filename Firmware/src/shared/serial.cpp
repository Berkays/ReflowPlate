#include "serial.h"

uint16_t checkSerial(uint16_t commands)
{
    if (SEGGER_RTT_HasKey())
    {
        byte buffer[32] = {0};
        SEGGER_RTT_Read(0, buffer, 32);

        if (buffer[0] != CODE_START)
            return 0;

        uint8_t command = buffer[1];

        if ((commands & command) == 0)
            return 0;

        switch (command)
        {
        case CODE_HOST_ENTER_PROFILE:
            sendFlashInfo(GetPageAddress(), ReadProfileCount(), PROFILE_SIZE, PROFILE_FLASH_DATA_SIZE, PROFILE_FLASH_DATA_OFFSET, PROFILE_FLASH_NAME_SIZE);
            return 1;
            break;
        case CODE_HOST_EXIT_PROFILE:
            return 2;
            break;
        default:
            break;
        }
    }
    return 0;
}

void sendStatus(uint32_t timeInSec, double temperature, double target, double output)
{
    byte buffer[32] = {0};
    byte *ptr = buffer;

    *(ptr++) = CODE_START;
    *(ptr++) = CODE_TELEMETRY;
    *(ptr++) = MARK_TIME;
    copyUIntToBuffer(ptr, timeInSec);
    *(ptr++) = MARK_TEMPERATURE;
    copyDoubleToBuffer(ptr, temperature);
    *(ptr++) = MARK_TARGET;
    copyDoubleToBuffer(ptr, target);
    *(ptr++) = MARK_OUTPUT;
    copyDoubleToBuffer(ptr, output);

    buffer[31] = CODE_END;
    SEGGER_RTT_Write(0, buffer, 32);
}

void sendFlashInfo(uint32_t flashAddress, uint32_t profileCount, uint32_t profileSize, uint32_t profileDataSize, uint32_t profileDataOffset, uint32_t profileNameSize)
{
    byte buffer[32] = {0};
    byte *ptr = buffer;

    *(ptr++) = CODE_START;
    *(ptr++) = CODE_PROFILE;
    *(ptr++) = MARK_PROFILE_FLASH_ADDR;
    copyUIntToBuffer(ptr, flashAddress);
    *(ptr++) = MARK_PROFILE_COUNT;
    copyUIntToBuffer(ptr, profileCount);
    *(ptr++) = MARK_PROFILE_SIZE;
    copyUIntToBuffer(ptr, profileSize);
    *(ptr++) = MARK_PROFILE_DATA_SIZE;
    copyUIntToBuffer(ptr, profileDataSize);
    *(ptr++) = MARK_PROFILE_DATA_OFFSET;
    copyUIntToBuffer(ptr, profileDataOffset);
    *(ptr++) = MARK_PROFILE_NAME_SIZE;
    copyUIntToBuffer(ptr, profileNameSize);

    buffer[31] = CODE_END;
    SEGGER_RTT_Write(0, buffer, 32);
}

void copyDoubleToBuffer(byte *ptr, double value)
{
    int intVal = (int)(value * 1000); // Send as integer
    uint8_t *valPtr = (uint8_t *)&intVal;
    for (int k = (sizeof(int) - 1); k >= 0; k--)
    {
        *ptr = *(valPtr + k);
        ptr++;
    }
}

void copyUIntToBuffer(byte *ptr, uint32_t value)
{
    uint8_t *valPtr = (uint8_t *)&value;
    for (int k = (sizeof(uint32_t) - 1); k >= 0; k--)
    {
        *ptr = *(valPtr + k);
        ptr++;
    }
}