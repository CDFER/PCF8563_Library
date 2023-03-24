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



## Based on the awesome work of Lewis he
Origin created by Lewis he on April 1, 2019.
https://github.com/lewisxhe/PCF8563_Library

MIT license, all text above must be included in any redistribution
