pcf8563-RTC
=====================================
A library to interace esp chips with the NXP PCF8563 Real time clock (RTC) in the arduino (c++) Framework.

## Features
- use multiple I2C Busses -> rtc.begin(Wire1);
- works with timezones (RTC is set to UTC)
https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
- set System (ESP32) time from RTC (assumes RTC is set to UTC/GMT)
- set RTC time from System (epoch)
- Set RTC time over wifi example

## Warnings
- not all functions are implemented
- not compatible with Lewis he's Library
- only tested with the esp32
- under active development

### Setup
```c++
#include "pcf8563.h"
#include "time.h"
PCF8563_Class rtc;
const char *time_zone = "NZST-12NZDT,M9.5.0,M4.1.0/3";	// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

Wire.begin();
rtc.begin(Wire);
setenv("TZ", time_zone, 1);
tzset();
rtc.syncToSystem();
```
### Loop
```c++
struct tm timeinfo;
getLocalTime(&timeinfo);
Serial.print(&timeinfo, "%d/%m/%y %H:%M:%S");
```

## 🖼️ Schematic
![Schematic](/images/schematic.png)
- uses a cr1220 coincell (though almost any 3v lithium coincell should work)
- to not discharge the battery too fast disable output clock and alarms
- Low backup current: typical 0.25 uA at 3.0 V (theoretical not tested)
- currently I'm testing with 4.7kohm pullups on sda and scl
- currently the crystal I'm using is a Seiko Epson Q13FC1350000400 (+-20ppm)
- in theory this setup will drift by a maximum of ~11mins per year (https://www.analog.com/en/design-center/interactive-design-tools/real-time-clock-calculator.html)
- in theory a 37mAh cr1220 battery will last ~14 years without any charging (incl battery self discharge)
- 3.3V -> diode -> resistor charging of a coincell from 3.3v is hotly debated as to wether is good, ok, or horificly bad idea. I have not had any issuses but your mileage may vary
- be careful with pcb placement (have the crystal as close to the RTC chip a possible and souround it with ground planes connected with vias)

## Based on the awesome work of Lewis he
Origin created by Lewis he on April 1, 2019.
https://github.com/lewisxhe/PCF8563_Library

MIT license
