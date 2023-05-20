## About

This project contains sample CubeMx project for the custom hardware design found [here](../Hardware/). You can reference this project to create your own CubeMx projects.

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
