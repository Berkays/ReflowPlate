#ifndef __FLASH_H
#define __FLASH_H

#include "Types.h"
#include "Config.h"

constexpr uint32_t PROFILE_FLASH_DATA_SIZE = REFLOW_POINT_COUNT; // Bytes
constexpr uint32_t PROFILE_FLASH_DATA_OFFSET = 2;                // Bytes
constexpr uint32_t PROFILE_FLASH_NAME_SIZE = 10;                 // Bytes
constexpr uint32_t PROFILE_SIZE = 32;                            // Bytes

constexpr uint32_t PROFILE_FLASH_PADDING = (PROFILE_SIZE - (PROFILE_FLASH_DATA_SIZE + PROFILE_FLASH_DATA_OFFSET + PROFILE_FLASH_NAME_SIZE));

// define in flash.cpp
extern uint32_t GetProfileAddress(byte);
extern uint32_t GetPage();
extern uint32_t GetPageAddress();
extern byte GetMaxProfileCount();

#endif