## About

This project contains required firmware for the reflow plate. Although it is mainly developed for the custom hardware design found [here](../Hardware/), firmware can be easily extended and modified to fit other custom STM32 boards since the project utilizes STM32 Hal Drivers generated by CubeMX. Personally I prefer using JLink emulated STLink for flashing and debugging purposes but you can use the same SWD ports with standard STLink.

### Features

- NTC Support with either RT Table curve or steinhart approximation.
- Rotary encoder as main input.
- Wide OLED display support thanks to U8G2.
- Automatic and Manual modes.
- Thermal runaway check in auto mode.
- Profile selection and graphing.
- Dynamic pwm current adjustment for current limiting.
- Automated CubeMx import script.
- Easy configuration and extension.
- Preheating and fan cooling.

## Programming

You need an STM32 programmer capable of flashing through SWD pins. Simplest way would be to download the precompiled binary and flash it using an STLink. For debugging purposes JLink works much better with PlatformIO and needs minimal configuration. JLink devices can also use SWD ports for fast bidirectional communication. Refer to [SEGGER STLinkReflash utility tool](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/) for converting STLink to JLink which is a reversible process.

## Configuration

Board configurations are mainly located in [Config.h](./include/Config.h) while some of the modular components has individual configuration files in [Features](./include/Features/) folder.
For only custom board ports, you will most likely only need to change the [Config.h](./include/Config.h) file after importing your generated CubeMx project.
Modification of screen layouts and temperature modules should be done under [modules](./src/modules/) folder.

## Porting to other STM32 MCUs

1. Generate CubeMx project with Makefile project type. Check copy necessary libraries to output.
2. Run [import.sh](/scripts/import.sh) script with generated CubeMx project directory as argument. <br>e.g `./scripts/import.sh ../CubeMx/MyProject`
3. Create a new PlatformIO enviroment in [platformio.ini](platformio.ini) and inherit from stm32 base environment.
4. Create mcu definition in [boards](/boards/) folder if it doesn't exist in platformio database.
5. Define peripheral mappings in [mappings.cpp](/src/STM32F030C8/mappings.cpp)
6. (Optional) You might need to modify analog read functions in [analog.cpp](/src/shared/analog.cpp) if the chip ADC has different register set.

### Required CubeMx peripherals

| Component       | Function             | Pin Type | Notes                                                        |
| --------------- | -------------------- | -------- | ------------------------------------------------------------ |
| PWM Timer       | PWM Output           | Output   | Adjust according to your application. Default is about 1khz. |
| Interrupt Timer | 1s Interrupt         | None     | Set Prescaler & ARR to have 1s period.                       |
| I2C             | OLED Display         | Output   | Enable internal pull-up on SDA & SCL.                        |
| Encoder Timer   | Rotary Encoder input | Input x2 | Enable combined channels. Set encoder mode to TI1 & TI2.     |
| I2C Timer       | Display Delay        | None     | Set frequency to 1Mhz. E.g `Clock/Prescaler+1 = 1Mhz`        |
| ADC-CH-A        | NTC1                 | Input    |                                                              |
| ADC-CH-B        | NTC2                 | Input    | Optional. Can be used for various applications.              |
| ADC-CH-C        | Source voltage       | Input    | Connected to a voltage divider.                              |
| GPIO Input      | Navigation           | Input    | Connected to an encoder SW pin.                              |
| GPIO Output     | Fan Output           | Output   | Optional. External fan control.                              |

## EEPROM Profile Layout

Each profile takes <b>32 Bytes</b> in eeprom. This value can be configured in [serial.h](src/shared/serial.h) depending on your needs and MCUs capabilities. <br>

```
---------- Profile - 1 ------------
20B Data
2B Offset
10B Name
---------- Profile - 2 ------------
20B Data
2B Offset
10B Name
---------- Profile - 3 ------------
...etc
```

## Attribution

Dependecies below are used to realize this project. Be aware that these dependecies may have different licenses and may not be suitable for your use case.

- [U8G2](https://github.com/olikraus/u8g2)
- [QuickPID](https://github.com/Dlloydev/QuickPID)
- [SEGGER_RTT](https://www.segger.com/products/debug-probes/j-link/technology/about-real-time-transfer/)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
