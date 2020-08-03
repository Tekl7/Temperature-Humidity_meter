# Temperature-Humidity_meter
Miniature temperature and humidity meter measures and displays temperature, humidity (as expected), two-state battery status and blinking proper operation indication dot.
Measuring period is set to 8 seconds in Watchdog timer. After each measurement and printing data, MCU goes to sleep mode.

Meter consists only of four main components:
- ATtiny85V MCU
- HTU21D I2C temperature and humidity sensor
- 0.96" I2C OLED display
- CR2032 battery (3V)

## MCU settings
Brown-out Detector has to be enabled to ensure proper operation of MCU.
Setting it to 1.8V (ATtiny85 is not able to do this, ATtiny85V is needed) should ensure proper operation of HTU21D sensor as well.
As BOD is enabled, clock source is set to: Int. RC Osc. 8 MHz; Start-up time PWRDWN/RESET: 6 CK/14 CK + 0 ms; [CKSEL=0010 SUT=00].

To save battery energy, Power-down sleep mode is used in conjunction with Watchdog timeout interrupt which wakes up the MCU each 8 seconds.
Then whole cycle of measuring and displaying is repeated.

## Used libraries
- I2C/TWI library for MCUs with USI interface: https://github.com/puuu/USIWire
- HTU21D library: https://github.com/Tekl7/HTU21D
- SSD1306 library (for display): https://github.com/Tekl7/AVR-SSD1306-Library + new font Tahoma15x16 containing only the necessary character to save memory

## Display description
- Title - 1st row
- Temperature in degrees Celsius - 2nd row
- Relative humidity - 3rd row
- Two-state battery status determined by HTU21D sensor - end of 1st row (next to the title)
    - VDD > 2.25V ±0.1V: full
    - VDD < 2.25V ±0.1V: empty
-  Blinking proper operation indication dot - lower right corner
    - MCU working: turned on
    - MCU in sleep mode: turned off
    - When battery discharges, the Brown-out Reset is activated and display freezes, so if indication dot is blinking, the device is operating properly.

## Build
- `F_CPU=1000000` in defined symbols (-D)
- `-std=c++11` flag
- `-Os` (optimize for size) optimization level
