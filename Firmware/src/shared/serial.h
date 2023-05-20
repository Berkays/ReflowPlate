#ifndef __SERIAL_H
#define __SERIAL_H

#include "hal_driver.h"

#include "Types.h"
#include "RTT/SEGGER_RTT.h"

#include "global.h"
#include "eeprom.h"

const byte CODE_START = 0xAF;
const byte CODE_END = 0xFA;
const byte CODE_TELEMETRY = 0xAB;
const byte CODE_PROFILE = 0xAC;

// Host Commands. Uses shifted bitfield.
const byte CODE_HOST_ENTER_PROFILE = 0x01;
const byte CODE_HOST_EXIT_PROFILE = 0x02;

const byte MARK_STATUS = 0xC0;
const byte MARK_TEMPERATURE = 0xC1;
const byte MARK_TARGET = 0xC2;
const byte MARK_OUTPUT = 0xC3;
const byte MARK_STATE = 0xC4;
const byte MARK_TIME = 0xC5;

const byte MARK_PROFILE_FLASH_ADDR = 0xD0;
const byte MARK_PROFILE_COUNT = 0xD1;
const byte MARK_PROFILE_SIZE = 0xD2;
const byte MARK_PROFILE_DATA_SIZE = 0xD3;
const byte MARK_PROFILE_DATA_OFFSET = 0xD4;
const byte MARK_PROFILE_NAME_SIZE = 0xD5;

uint16_t checkSerial(uint16_t);

void sendStatus(uint32_t, double, double, double);
void sendFlashInfo(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

void copyDoubleToBuffer(byte *, double);
void copyUIntToBuffer(byte *ptr, uint32_t value);

#endif