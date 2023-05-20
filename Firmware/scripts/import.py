import os
import sys
import re

print(sys.argv)
if len(sys.argv) < 3 or sys.argv[1] == None or sys.argv[2] == None:
    print("Invalid Arguments.")
    os._exit(1)
    
# PINS
try:
    f = open('./Core/Inc/main.h', mode='r')
except FileNotFoundError:
    print("Generate project first!")
    os._exit(1)


regex = r"/\*\s*Private\s*defines[.|\s|\S]*?(#define[.|\s|\S]*?)/\*\s*USER\s*CODE"
main_h = f.read()
f.close()

search = re.search(regex, main_h, re.M|re.I)
pins = search.group(1)

f = open('./pins.h', mode='wt')
f.write(pins)
f.close()

print("Written pins.h")


# PERIPHERALS_H
f = open('./Core/Src/main.c', mode='r')
main_c = f.read()
f.close()

content = """
#ifndef __PERIPHERALS_H
#define __PERIPHERALS_H

#include "hal_driver.h"
#include "pins.h"

#ifdef __cplusplus
extern "C"
{
#endif
""".lstrip()

# definitions
regex = r"/\*\s*Private\s*variables[.|\s|\S].*([\s|\S|.]*?)/\*\s*USER"
search = re.search(regex, main_c, re.M|re.I)
definitions = search.group(1).rstrip()

definitions = definitions.splitlines(keepends=True)
definitions = [x for x in definitions if len(x.rstrip()) > 0]

content += "".join(["extern " + x for x in definitions])
content += """\n
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
#ifdef __cplusplus
}
#endif

void InitPeripherals(void);
void Error_Handler(void);
""".rstrip()

# function prototypes
regex = r"/\*\s*Private\s*function[.|\s|\S].*([\s|\S|.]*?)/\*\s*USER"
search = re.search(regex, main_c, re.M|re.I)
prototypes = search.group(1).rstrip().replace("static ", "")

content += "\n" + prototypes
content += "\n#endif"

# peripherals.h
f = open('./peripherals.h', mode='wt')
f.write(content)
f.close()
print("Written peripherals.h")

content = """
#include "peripherals.h"
""".lstrip()

regex = r"(void\s*SystemClock_Config\(void\)\s*{\s*[\s|\S|.]*)"
search = re.search(regex, main_c, re.M|re.I)
functions = search.group(1).rstrip().replace("static ", "")

content += "\n" + "".join([x.replace(";","").rstrip() + " = {};\n" for x in definitions])

content += """
void InitPeripherals(void)
{
  HAL_Init();
"""
content += prototypes.replace("void", "")
content += "\n}"
content += "\n\n" + functions
content += "\n"

# peripherals.cpp
f = open('./peripherals.cpp', mode='wt')
f.write(content)
f.close()
print("Written peripherals.cpp")

# Modify linker script
regex = r"FLASH\s*\(rx\)\s*[\w|\W]*?\d{2,4}(K.*)\s*}"

f = open(sys.argv[2], mode='r+')
f2 = open("loader.ld", mode='w')
content = f.read()
search = re.search(regex, content, re.MULTILINE | re.IGNORECASE)
content = content[0: search.start(1) + 1] + " - LD_RESERVED_PROFILE_SPACE" + content[search.start(1) + 1:] 
f.close()
f2.write(content)
f2.close()

# Hal Driver header
content = """
#ifndef __HAL_DRIVER_H
#define __HAL_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "%s_hal.h"
#ifdef __cplusplus
}
#endif
#endif
""" % sys.argv[1].strip().lower()

# content = content.format(sys.argv[1].strip().lower())

f = open('hal_driver.h', mode='w')
f.write(content)
f.close()

print("Written hal_driver.h")

os._exit(0)