#include "flash.h"

#include "hal_driver.h"

// Total number of flash pages in MCU.
constexpr uint32_t FLASH_PAGE_COUNT = (((FLASH_BANK1_END - FLASH_BASE) / FLASH_PAGE_SIZE) + 1);

// Get the address of the last page.
constexpr uint32_t PROFILE_PAGE_ADDRESS = (FLASH_BASE + (FLASH_PAGE_SIZE * (FLASH_PAGE_COUNT - 1)));

// Get amount of profiles that can be stored inside single page.
constexpr uint32_t PROFILE_PER_PAGE = FLASH_PAGE_SIZE / PROFILE_SIZE;

uint32_t GetProfileAddress(byte index)
{
    return PROFILE_PAGE_ADDRESS + (PROFILE_SIZE * index);
}

uint32_t GetPage()
{
    return FLASH_PAGE_COUNT - 1;
}

uint32_t GetPageAddress()
{
    return PROFILE_PAGE_ADDRESS;
}

byte GetMaxProfileCount()
{
    return PROFILE_PER_PAGE;
}
