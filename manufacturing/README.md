# Revision 0

| Components Side | Display Side |
|-|-|
|![](/documentation/images/REV0_PCB_1.png)|![](/documentation/images/REV0_PCB_2.png)|

Features:

- SD Card with SPI Mode
- IMU on I2C3 SMBA
- TFT on SPI1

# Revision 1

| Components Side | Display Side |
|-|-|
|![](/documentation/images/REV1_PCB_1.png)|![](/documentation/images/REV1_PCB_2.png)|

Changes:

- Fullspeed SDIO Interface
- IMU on I2C1 SMBA
- Optimized Buck Converter
- Slope Control for CAN Bus
- 10pin Header for Debugging
- Larger Silkscreen
- JLC Tooling Holes

# Revision 2

| Components Side | Display Side |
|-|-|
|![](/documentation/images/REV2_PCB_1.png)|![](/documentation/images/REV2_PCB_2.png)|

Changes:

- Simple Switcher Buckconverter Update
- Optimized Traces for
  - IMU
  - SD Card
  - SWD Interface
  - TFT Display
- Reduced Overall costs

# Revision 3

> In Development

Changes:

- rm Bootmode Switch (SWD only flashing, space constriants)
- MCU: STM32F412RGT6 (chip shortage)
- SD Card
  - Supply Fitering (Additional Pi Filter)
  - ESD Protection (ESD9B3.3ST5G)
  - Dataline Pullups
- change Stack-Up -> Sig/Pwr, GND, GND, Sig/Pwr
- Fiducals/Tooling Holes larger silkscreen expansion
- Buck converter Thinner Feedback Trace
