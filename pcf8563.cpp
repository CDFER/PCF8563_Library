/////////////////////////////////////////////////////////////////
/*
MIT License

Copyright (c) 2019 lewis he

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

pcf8563.cpp - Arduino library for NXP PCF8563 RTC chip.
Created by Lewis he on April 1, 2019.
github:https://github.com/lewisxhe/PCF8563_Library
*/
/////////////////////////////////////////////////////////////////
#include "pcf8563.h"

#include <sys/time.h>
#include <time.h>

uint8_t PCF8563_Class::begin(TwoWire &port, int addr) {
    _i2cPort = &port;                       // Set the I2C port to be used for communication
    _address = addr;                        // Set the I2C address of the PCF8563 chip
    _i2cPort->beginTransmission(_address);  // Begin the I2C transmission to the PCF8563 chip
    return _i2cPort->endTransmission();     // Check if the transmission was successful and return the result
}

const char *PCF8563_Class::getErrorText(uint8_t errorCode) {
    switch (errorCode) {
        case 0:
            return "Success";
        case 1:
            return "I2C data too long to fit in transmit buffer";
        case 2:
            return "I2C received NACK on transmit of address";
        case 3:
            return "I2C received NACK on transmit of data";
        case 4:
            return "I2C other error";
        case 5:
            return "I2C timeout";
        case 6:
            return "bytesReceived(%i) != bytesRequested(%i)";
        case 7:
            return "Measurement out of range";
        default:
            return "Unknown error";
    }
}

void PCF8563_Class::setDateTime(RTC_Date date) {
    setDateTime(date.year, date.month, date.day, date.hour, date.minute, date.second);
    // Call the setDateTime function with individual date and time components
}

uint32_t PCF8563_Class::getDayOfWeek(uint32_t day, uint32_t month, uint32_t year) {
    uint32_t val;
    if (month < 3) {          // Check if the month is January or February
        month = 12u + month;  // Adjust the month value accordingly
        year--;               // Decrease the year value by 1
    }
    val = (day + (((month + 1u) * 26u) / 10u) + year + (year / 4u) + (6u * (year / 100u)) + (year / 400u)) % 7u;
    // Calculate the day of the week using Zeller's Congruence formula
    if (0u == val) {
        val = 7;  // Adjust Sunday's value to 7 instead of 0
    }
    return (val - 1);  // Subtract 1 to match the range 0-6 for days of the week
}

void PCF8563_Class::setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
    _data[0] = _dec_to_bcd(second) & (~PCF8563_VOL_LOW_MASK);  // Convert seconds to BCD and store in data array
    _data[1] = _dec_to_bcd(minute);                            // Convert minutes to BCD and store in data array
    _data[2] = _dec_to_bcd(hour);                              // Convert hours to BCD and store in data array
    _data[3] = _dec_to_bcd(day);                               // Convert day to BCD and store in data array
    _data[4] = getDayOfWeek(day, month, year);                 // Get the day of the week and store in data array
    _data[5] = _dec_to_bcd(month);                             // Convert month to BCD and store in data array
    _data[6] = _dec_to_bcd(year % 100);                        // Convert last two digits of the year to BCD and store in data array

    if ((2000 % year) == 2000) {
        _data[5] &= (~PCF8563_CENTURY_MASK);  // Clear the century bit if the year is 2000
    } else {
        _data[5] |= PCF8563_CENTURY_MASK;  // Set the century bit if the year is not 2000
    }
    _writeByte(PCF8563_SEC_REG, 7, _data);  // Write the date and time values to the PCF8563 chip
}

bool PCF8563_Class::isValid() {
    _readByte(PCF8563_SEC_REG, 1, &_isValid);  // Read the seconds register to check validity
    return !(_isValid & (1 << 7));             // Check the validity flag and return the result
}
RTC_Date PCF8563_Class::getDateTime() {
    uint16_t year;                                               // Variable to store the year
    uint8_t century = 0;                                         // Variable to store the century flag
    _readByte(PCF8563_SEC_REG, 7, _data);                        // Read the date and time registers
    _voltageLow = (_data[0] & PCF8563_VOL_LOW_MASK);             // Check if voltage low flag is set
    _data[0] = _bcd_to_dec(_data[0] & (~PCF8563_VOL_LOW_MASK));  // Convert seconds from BCD to decimal
    _data[1] = _bcd_to_dec(_data[1] & PCF8563_minuteS_MASK);     // Convert minutes from BCD to decimal
    _data[2] = _bcd_to_dec(_data[2] & PCF8563_HOUR_MASK);        // Convert hours from BCD to decimal
    _data[3] = _bcd_to_dec(_data[3] & PCF8563_DAY_MASK);         // Convert day from BCD to decimal
    _data[4] = _bcd_to_dec(_data[4] & PCF8563_WEEKDAY_MASK);     // Convert weekday from BCD to decimal
    century = _data[5] & PCF8563_CENTURY_MASK;                   // Check the century flag
    _data[5] = _bcd_to_dec(_data[5] & PCF8563_MONTH_MASK);       // Convert month from BCD to decimal
    year = _bcd_to_dec(_data[6]);                                // Convert year from BCD to decimal
    // century :  0 = 1900 , 1 = 2000
    year = century ? 1900 + year : 2000 + year;  // Determine the actual year based on the century flag
    return RTC_Date(
        year,
        _data[5],
        _data[3],
        _data[2],
        _data[1],
        _data[0]);  // Create and return RTC_Date object with the obtained date and time values
}

RTC_Alarm PCF8563_Class::getAlarm() {
    _readByte(PCF8563_ALRM_MIN_REG, 4, _data);                   // Read the alarm registers
    _data[0] = _bcd_to_dec(_data[0] & (~PCF8563_minuteS_MASK));  // Convert minutes from BCD to decimal
    _data[1] = _bcd_to_dec(_data[1] & (~PCF8563_HOUR_MASK));     // Convert hours from BCD to decimal
    _data[2] = _bcd_to_dec(_data[2] & (~PCF8563_DAY_MASK));      // Convert day from BCD to decimal
    _data[3] = _bcd_to_dec(_data[3] & (~PCF8563_WEEKDAY_MASK));  // Convert weekday from BCD to decimal
    return RTC_Alarm(_data[0], _data[1], _data[2], _data[3]);    // Create and return RTC_Alarm object with the obtained alarm values
}

void PCF8563_Class::enableAlarm() {
    _readByte(PCF8563_STAT2_REG, 1, _data);              // Read the status register 2
    _data[0] &= ~PCF8563_ALARM_AF;                       // Clear the alarm flag
    _data[0] |= (PCF8563_TIMER_TF | PCF8563_ALARM_AIE);  // Set the alarm enable and timer flag
    _writeByte(PCF8563_STAT2_REG, 1, _data);             // Write back the updated status register 2
}

void PCF8563_Class::disableAlarm() {
    _readByte(PCF8563_STAT2_REG, 1, _data);               // Read the status register 2
    _data[0] &= ~(PCF8563_ALARM_AF | PCF8563_ALARM_AIE);  // Clear the alarm flag and alarm enable
    _data[0] |= PCF8563_TIMER_TF;                         // Set the timer flag
    _writeByte(PCF8563_STAT2_REG, 1, _data);              // Write back the updated status register 2
}

void PCF8563_Class::resetAlarm() {
    _readByte(PCF8563_STAT2_REG, 1, _data);   // Read the status register 2
    _data[0] &= ~(PCF8563_ALARM_AF);          // Clear the alarm flag
    _data[0] |= PCF8563_TIMER_TF;             // Set the timer flag
    _writeByte(PCF8563_STAT2_REG, 1, _data);  // Write back the updated status register 2
}
bool PCF8563_Class::alarmActive() {
    _readByte(PCF8563_STAT2_REG, 1, _data);      // Read the status register 2
    return (bool)(_data[0] & PCF8563_ALARM_AF);  // Check if the alarm flag is set
}

void PCF8563_Class::setAlarm(RTC_Alarm alarm) {
    setAlarm(alarm.minute, alarm.hour, alarm.day, alarm.weekday);  // Call the setAlarm function with individual alarm parameters
}

void PCF8563_Class::setAlarm(uint8_t hour, uint8_t minute, uint8_t day, uint8_t weekday) {
    if (minute != PCF8563_NO_ALARM) {
        _data[0] = _dec_to_bcd(constrain(minute, 0, 59));  // Convert minutes to BCD and store in data[0]
        _data[0] &= ~PCF8563_ALARM_ENABLE;                 // Clear the alarm enable bit
    } else {
        _data[0] = PCF8563_ALARM_ENABLE;  // Set the alarm enable bit
    }

    if (hour != PCF8563_NO_ALARM) {
        _data[1] = _dec_to_bcd(constrain(hour, 0, 23));  // Convert hours to BCD and store in data[1]
        _data[1] &= ~PCF8563_ALARM_ENABLE;               // Clear the alarm enable bit
    } else {
        _data[1] = PCF8563_ALARM_ENABLE;  // Set the alarm enable bit
    }

    if (day != PCF8563_NO_ALARM) {
        _data[2] = _dec_to_bcd(constrain(day, 1, 31));  // Convert day to BCD and store in data[2]
        _data[2] &= ~PCF8563_ALARM_ENABLE;              // Clear the alarm enable bit
    } else {
        _data[2] = PCF8563_ALARM_ENABLE;  // Set the alarm enable bit
    }

    if (weekday != PCF8563_NO_ALARM) {
        _data[3] = _dec_to_bcd(constrain(weekday, 0, 6));  // Convert weekday to BCD and store in data[3]
        _data[3] &= ~PCF8563_ALARM_ENABLE;                 // Clear the alarm enable bit
    } else {
        _data[3] = PCF8563_ALARM_ENABLE;  // Set the alarm enable bit
    }

    _writeByte(PCF8563_ALRM_MIN_REG, 4, _data);  // Write the alarm data to the alarm registers
}

void PCF8563_Class::setAlarmByMinutes(uint8_t minute) {
    setAlarm(PCF8563_NO_ALARM, minute, PCF8563_NO_ALARM, PCF8563_NO_ALARM);  // Set the alarm only for minutes
}

void PCF8563_Class::setAlarmByDays(uint8_t day) {
    setAlarm(PCF8563_NO_ALARM, PCF8563_NO_ALARM, day, PCF8563_NO_ALARM);  // Set the alarm only for days
}

void PCF8563_Class::setAlarmByHours(uint8_t hour) {
    setAlarm(hour, PCF8563_NO_ALARM, PCF8563_NO_ALARM, PCF8563_NO_ALARM);  // Set the alarm only for hours
}

void PCF8563_Class::setAlarmByWeekDay(uint8_t weekday) {
    setAlarm(PCF8563_NO_ALARM, PCF8563_NO_ALARM, PCF8563_NO_ALARM, weekday);  // Set the alarm only for weekdays
}

bool PCF8563_Class::isTimerEnable() {
    _readByte(PCF8563_STAT2_REG, 1, &_data[0]);             // Read the status register 2
    _readByte(PCF8563_TIMER1_REG, 1, &_data[1]);            // Read the timer register
    if (_data[0] & PCF8563_TIMER_TIE) {                     // Check if timer interrupt is enabled
        return _data[1] & PCF8563_TIMER_TE ? true : false;  // Check if timer is enabled
    }
    return false;  // Timer is not enabled
}
bool PCF8563_Class::isTimerActive() {
    _readByte(PCF8563_STAT2_REG, 1, _data);    // Read the status register 2
    return (bool)_data[0] & PCF8563_TIMER_TF;  // Check if the timer flag is set
}

void PCF8563_Class::enableTimer() {
    _readByte(PCF8563_STAT2_REG, 1, &_data[0]);          // Read the status register 2
    _readByte(PCF8563_TIMER1_REG, 1, &_data[1]);         // Read the timer register
    _data[0] &= ~PCF8563_TIMER_TF;                       // Clear the timer flag
    _data[0] |= (PCF8563_ALARM_AF | PCF8563_TIMER_TIE);  // Enable alarm interrupt and timer interrupt
    _data[1] |= PCF8563_TIMER_TE;                        // Enable the timer
    _writeByte(PCF8563_STAT2_REG, 1, &_data[0]);         // Write the updated status register 2
    _writeByte(PCF8563_TIMER1_REG, 1, &_data[1]);        // Write the updated timer register
}

void PCF8563_Class::disableTimer() {
    _readByte(PCF8563_STAT2_REG, 1, _data);   // Read the status register 2
    _data[0] &= ~PCF8563_TIMER_TF;            // Clear the timer flag
    _data[1] |= PCF8563_ALARM_AF;             // Set the alarm flag
    _writeByte(PCF8563_STAT2_REG, 1, _data);  // Write the updated status register 2
}

void PCF8563_Class::setTimer(uint8_t val, uint8_t freq, bool enInterrupt) {
    _readByte(PCF8563_STAT2_REG, 1, &_data[0]);   // Read the status register 2
    _readByte(PCF8563_TIMER1_REG, 1, &_data[1]);  // Read the timer register
    if (enInterrupt) {
        _data[0] |= 1 << 4;  // Set the interrupt enable bit
    } else {
        _data[0] &= ~(1 << 4);  // Clear the interrupt enable bit
    }
    _data[1] |= (freq & PCF8563_TIMER_TD10);       // Set the timer frequency
    _data[2] = val;                                // Set the timer value
    _writeByte(PCF8563_STAT2_REG, 1, &_data[0]);   // Write the updated status register 2
    _writeByte(PCF8563_TIMER1_REG, 1, &_data[1]);  // Write the updated timer register
    _writeByte(PCF8563_TIMER2_REG, 1, &_data[2]);  // Write the timer value
}

void PCF8563_Class::clearTimer() {
    _readByte(PCF8563_STAT2_REG, 1, _data);               // Read the status register 2
    _data[0] &= ~(PCF8563_TIMER_TF | PCF8563_TIMER_TIE);  // Clear the timer flag and timer interrupt enable bits
    _data[0] |= PCF8563_ALARM_AF;                         // Set the alarm flag
    _data[1] = 0x00;                                      // Clear the timer value
    _writeByte(PCF8563_STAT2_REG, 1, &_data[0]);          // Write the updated status register 2
    _writeByte(PCF8563_TIMER1_REG, 1, &_data[1]);         // Write the updated timer register
}

bool PCF8563_Class::enableCLK(uint8_t freq) {
    if (freq >= PCF8563_CLK_MAX) return false;  // Check if the frequency is valid
    _data[0] = freq | PCF8563_CLK_ENABLE;       // Set the frequency and enable the clock
    _writeByte(PCF8563_SQW_REG, 1, _data);      // Write the updated square wave register
    return true;                                // Clock enabled successfully
}

void PCF8563_Class::disableCLK() {
    _data[0] = 0x00;                        // Disable the clock
    _writeByte(PCF8563_SQW_REG, 1, _data);  // Write the updated square wave register
}

const char *PCF8563_Class::formatDateTime(uint8_t style) {
    RTC_Date t = getDateTime();  // Get the current date and time
    switch (style) {
        case PCF_TIMEFORMAT_HM:
            snprintf(format, sizeof(format), "%d:%d", t.hour, t.minute);
            break;
        case PCF_TIMEFORMAT_HMS:
            snprintf(format, sizeof(format), "%d:%d:%d", t.hour, t.minute, t.second);
            break;
        case PCF_TIMEFORMAT_YYYY_MM_DD:
            snprintf(format, sizeof(format), "%d-%d-%d", t.year, t.month, t.day);
            break;
        case PCF_TIMEFORMAT_MM_DD_YYYY:
            snprintf(format, sizeof(format), "%d-%d-%d", t.month, t.day, t.year);
            break;
        case PCF_TIMEFORMAT_DD_MM_YYYY:
            snprintf(format, sizeof(format), "%d-%d-%d", t.day, t.month, t.year);
            break;
        case PCF_TIMEFORMAT_YYYY_MM_DD_H_M_S:
            snprintf(format, sizeof(format), "%d-%d-%d/%d:%d:%d", t.year, t.month, t.day, t.hour, t.minute, t.second);
            break;
        default:
            snprintf(format, sizeof(format), "%d:%d", t.hour, t.minute);
            break;
    }
    return format;
}

#ifdef ESP32

bool PCF8563_Class::syncToSystem() {
    if (PCF8563_Class::isValid()) {
        struct tm t_tm;
        struct timeval val;
        RTC_Date dt = getDateTime();
        t_tm.tm_hour = dt.hour;
        t_tm.tm_min = dt.minute;
        t_tm.tm_sec = dt.second;
        t_tm.tm_year = dt.year - 1900;  // Year, whose value starts from 1900
        t_tm.tm_mon = dt.month - 1;     // Month (starting from January, 0 for January) - Value range is [0,11]
        t_tm.tm_mday = dt.day;

        val.tv_sec = mktime(&t_tm) - _timezone;  // make epoch from UTC/GMT even when system timezone already set
        val.tv_usec = 0;

        settimeofday(&val, NULL);  // set system epoch
        return true;
    } else {
        ESP_LOGE("RTC Time is not Valid", "System Epoch Not Set");
        return false;
    }
}

bool PCF8563_Class::syncToRtc() {
    time_t epoch;
    struct tm gmt;
    time(&epoch);
    if (epoch > 0 && epoch < 4102444800) {  // sanity check if epoch is between 1970 and 2100
        gmtime_r(&epoch, &gmt);
        setDateTime(gmt.tm_year + 1900, gmt.tm_mon + 1, gmt.tm_mday, gmt.tm_hour, gmt.tm_min, gmt.tm_sec);
        return true;
    } else {
        ESP_LOGE("ESP32 Time is not Valid", "RTC Time Not Set");
        return false;
    }
}

#endif

RTC_Date::RTC_Date()
    : year(0), month(0), day(0), hour(0), minute(0), second(0) {
}

RTC_Date::RTC_Date(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mm, uint8_t s)
    : year(y), month(m), day(d), hour(h), minute(mm), second(s) {
}

uint8_t RTC_Date::StringToUint8(const char *pString) {
    uint8_t value = 0;

    // skip leading 0 and spaces
    while ('0' == *pString || *pString == ' ') {
        pString++;
    }

    // calculate number until we hit non-numeral char
    while ('0' <= *pString && *pString <= '9') {
        value *= 10;
        value += *pString - '0';
        pString++;
    }
    return value;
}

RTC_Alarm::RTC_Alarm(uint8_t m, uint8_t h, uint8_t d, uint8_t w)
    : minute(m), hour(h), day(d), weekday(w) {
}

uint8_t PCF8563_Class::status2() {
    _readByte(PCF8563_STAT2_REG, 1, _data);
    return _data[0];
}

bool RTC_Date::operator==(RTC_Date d) {
    return ((d.year == year) && (d.month == month) && (d.day == day) && (d.hour == hour) && (d.minute == minute));
}