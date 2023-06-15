# pcf8563-RTC

The `pcf8563-RTC` library is designed to interface ESP chips with the NXP PCF8563 Real-Time Clock (RTC) within the Arduino (C++) Framework.

## Features

- Supports multiple I2C buses: `rtc.begin(Wire1);`
- Timezone support (RTC is set to UTC)
  - [List of timezones](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)
- Sets the system (ESP32) time from the RTC (assumes RTC is set to UTC/GMT)
- Sets the RTC time from the system (epoch)
- Example of setting RTC time using WiFi

## Warnings

- Not all functions are implemented
- Not compatible with Lewis' library
- Only tested with the ESP32 and S2 variants
- Currently under development

## Example ESP32 Code

```cpp
#include "pcf8563.h"
#include "time.h"

PCF8563_Class rtc;
const char *time_zone = "NZST-12NZDT,M9.5.0,M4.1.0/3"; // [List of timezones](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)

Wire.begin();
rtc.begin(Wire);
setenv("TZ", time_zone, 1);
tzset();
rtc.syncToSystem();
```

```cpp
struct tm timeinfo;
getLocalTime(&timeinfo);
Serial.print(&timeinfo, "%d/%m/%y %H:%M:%S");
```

## Schematic

![Schematic](/images/schematic.png)

- Uses a CR1220 coincell (though almost any 3V lithium coincell should work)
- To prevent battery discharge, disable output clock and alarms
- Low backup current: typically 0.25uA at 3.0V (theoretical, not tested)
- Currently testing with 4.7kohm pull-ups on SDA and SCL
- Currently using a Seiko Epson Q13FC1350000400 crystal (+-20ppm)
- In theory, this setup will drift by a maximum of ~11 minutes per year ([Real-Time Clock Calculator](https://www.analog.com/en/design-center/interactive-design-tools/real-time-clock-calculator.html))
- In theory, a 37mAh CR1220 battery will last ~14 years without any charging (including battery self-discharge)
- Charging a coincell from 3.3V through a diode and resistor is a topic of debate. It is important to exercise caution and consider individual circumstances. Using a 200-ohm resistor should limit the current to a maximum of 12mA, while a 1k-ohm resistor should limit it to a maximum of 2.6mA.
- Be careful with PCB placement; ensure the crystal is placed as close to the RTC chip as possible and surrounded by ground planes connected with vias

## Acknowledgments

This library is based on the original work of Lewis He, created in 2019.

[GitHub Repository](https://github.com/lewisxhe/PCF8563_Library)

## License

This library is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

Please note that not all functions are implemented, and the library is under active development. For more information on how to use the library, refer to the documentation and examples provided.