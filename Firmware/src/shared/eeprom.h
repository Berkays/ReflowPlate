#ifndef __EEPROM_H
#define __EEPROM_H

#include <stdio.h>
#include <string.h>

#include "global.h"

#include "flash.h"

void ReadProfile(byte *, char *, byte);
byte ReadProfileCount();
void WriteProfile(byte *, unsigned long);

#endif