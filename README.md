Sure! Here's an improved version of the README file with a markdown table showcasing the functions in the library:

---

# pcf8563-RTC

A library to interface ESP chips with the NXP PCF8563 Real-Time Clock (RTC) in the Arduino (C++) Framework.

## Features

- Supports multiple I2C buses -> `rtc.begin(Wire1);`
- Works with timezones (RTC is set to UTC)
  - [List of timezones](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)
- Sets System (ESP32) time from RTC (assumes RTC is set to UTC/GMT)
- Sets RTC time from System (epoch)
- Set RTC time using WiFi example

## Warnings

- Not all functions are implemented
- Not compatible with Lewis' library
- Only tested with the ESP32
- Under development

## Setup

```c++
#include "pcf8563.h"
#include "time.h"

PCF8563_Class rtc;
const char *time_zone = "NZST-12NZDT,M9.5.0,M4.1.0/3";	// [List of timezones](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)

Wire.begin();
rtc.begin(Wire);
setenv("TZ", time_zone, 1);
tzset();
rtc.syncToSystem();
```

## Loop

```c++
struct tm timeinfo;
getLocalTime(&timeinfo);
Serial.print(&timeinfo, "%d/%m/%y %H:%M:%S");
```

## Schematic

![Schematic](/images/schematic.png)

- Uses a CR1220 coincell (though almost any 3V lithium coincell should work)
- To not discharge the battery too fast, disable output clock and alarms
- Low backup current: typical 0.25uA at 3.0V (theoretical, not tested)
- Currently testing with 4.7kohm pull-ups on SDA and SCL
- Currently using a Seiko Epson Q13FC1350000400 crystal (+-20ppm)
- In theory, this setup will drift by a maximum of ~11 minutes per year ([Real-Time Clock Calculator](https://www.analog.com/en/design-center/interactive-design-tools/real-time-clock-calculator.html))
- In theory, a 37mAh CR1220 battery will last ~14 years without any charging (including battery self-discharge)
- 3.3V -> diode -> resistor charging of a coincell from 3.3V is hotly debated as to whether it is a good, okay, or horrifically bad idea. I have not had any issues, but your mileage may vary
- Be careful with PCB placement (have the crystal as close to the RTC chip as possible and surround it with ground planes connected with vias)

## Based on the awesome work of Lewis He

Originally created by Lewis He in 2019

[GitHub Repository](https://github.com/lewisxhe/PCF8563_Library)

## License

This library is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

---

And here's the markdown table with the functions:

| Function                                                                                                    | Description                                                                               |
| ----------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------- |
| `int begin(TwoWire &port = Wire, uint8_t addr = PCF8563_ADDRESS)`                                           | Initializes the PCF8563 RTC.                                                              |
| `void setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)` | Sets the date and time on the RTC.                                                        |
| `void setDateTime(RTC_Date date)`                                                                           | Sets the date and time on the RTC using an RTC_Date object.                               |
| `RTC_Date getDateTime()`                                                                                    | Retrieves the current date and time from the RTC.                                         |
| `RTC_Alarm getAlarm()`                                                                                      | Retrieves the alarm settings from the RTC.                                                |
| `void enableAlarm()`                                                                                        | Enables the alarm on the RTC.                                                             |
| `void disableAlarm()`                                                                                       | Disables the alarm on the RTC.                                                            |
| `bool alarmActive()`                                                                                        | Checks if the alarm is active on the RTC.                                                 |
| `void resetAlarm()`                                                                                         | Resets the alarm on the RTC.                                                              |
| `void setAlarm(RTC_Alarm alarm)`                                                                            | Sets the alarm on the RTC using an RTC_Alarm object.                                      |
| `void setAlarm(uint8_t hour, uint8_t minute, uint8_t day, uint8_t weekday)`                                 | Sets the alarm on the RTC using individual values for hour, minute, day, and weekday.     |
| `bool isValid()`                                                                                            | Checks if the clock data on the RTC can be trusted.                                       |
| `void setAlarmByWeekDay(uint8_t weekday)`                                                                   | Sets the alarm on the RTC based on the weekday.                                           |
| `void setAlarmByHours(uint8_t hour)`                                                                        | Sets the alarm on the RTC based on the hour.                                              |
| `void setAlarmByDays(uint8_t day)`                                                                          | Sets the alarm on the RTC based on the day.                                               |
| `void setAlarmByMinutes(uint8_t minute)`                                                                    | Sets the alarm on the RTC based on the minute.                                            |
| `bool isTimerEnable()`                                                                                      | Checks if the timer is enabled on the RTC.                                                |
| `bool isTimerActive()`                                                                                      | Checks if the timer is active on the RTC.                                                 |
| `void enableTimer()`                                                                                        | Enables the timer on the RTC.                                                             |
| `void disableTimer()`                                                                                       | Disables the timer on the RTC.                                                            |
| `void setTimer(uint8_t val, uint8_t freq, bool enInterrupt)`                                                | Sets the timer on the RTC with the specified values.                                      |
| `void clearTimer()`                                                                                         | Clears the timer on the RTC.                                                              |
| `bool enableCLK(uint8_t freq)`                                                                              | Enables the clock output on the RTC with the specified frequency.                         |
| `void disableCLK()`                                                                                         | Disables the clock output on the RTC.                                                     |
| `bool syncToSystem()`                                                                                       | Sets the system time from the RTC if the RTC data is valid.                               |
| `bool syncToRtc()`                                                                                          | Sets the RTC time from the system time (ESP) if the system time is between 1970 and 2100. |

Please note that not all functions are implemented, and the library is under active development. Make sure to refer to the documentation and examples for more information on how to use the library.
