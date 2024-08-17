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

pcf8563.h - Arduino library for NXP PCF8563 RTC chip.
Created by Lewis he on April 1, 2019.
github:https://github.com/lewisxhe/PCF8563_Library
*/
/////////////////////////////////////////////////////////////////
#pragma once

#include <Arduino.h>
#include <Wire.h>

#define PCF8563_ADDRESS (0x51)  // 7-bit I2C Address

//! REG MAP
#define PCF8563_STAT1_REG (0x00)
#define PCF8563_STAT2_REG (0x01)
#define PCF8563_SEC_REG (0x02)
#define PCF8563_MIN_REG (0x03)
#define PCF8563_HR_REG (0x04)
#define PCF8563_DAY_REG (0x05)
#define PCF8563_WEEKDAY_REG (0x06)
#define PCF8563_MONTH_REG (0x07)
#define PCF8563_YEAR_REG (0x08)
#define PCF8563_ALRM_MIN_REG (0x09)
#define PCF8563_SQW_REG (0x0D)
#define PCF8563_TIMER1_REG (0x0E)
#define PCF8563_TIMER2_REG (0x0F)

#define PCF8563_VOL_LOW_MASK (0x80)
#define PCF8563_minuteS_MASK (0x7F)
#define PCF8563_HOUR_MASK (0x3F)
#define PCF8563_WEEKDAY_MASK (0x07)
#define PCF8563_CENTURY_MASK (0x80)
#define PCF8563_DAY_MASK (0x3F)
#define PCF8563_MONTH_MASK (0x1F)
#define PCF8563_TIMER_CTL_MASK (0x03)

#define PCF8563_ALARM_AF (0x08)
#define PCF8563_TIMER_TF (0x04)
#define PCF8563_ALARM_AIE (0x02)
#define PCF8563_TIMER_TIE (0x01)
#define PCF8563_TIMER_TE (0x80)
#define PCF8563_TIMER_TD10 (0x03)

#define PCF8563_NO_ALARM (0xFF)
#define PCF8563_ALARM_ENABLE (0x80)
#define PCF8563_CLK_ENABLE (0x80)

enum {
    PCF8563_CLK_32_768KHZ,
    PCF8563_CLK_1024KHZ,
    PCF8563_CLK_32HZ,
    PCF8563_CLK_1HZ,
    PCF8563_CLK_MAX
};

enum {
    PCF_TIMEFORMAT_HM,
    PCF_TIMEFORMAT_HMS,
    PCF_TIMEFORMAT_YYYY_MM_DD,
    PCF_TIMEFORMAT_MM_DD_YYYY,
    PCF_TIMEFORMAT_DD_MM_YYYY,
    PCF_TIMEFORMAT_YYYY_MM_DD_H_M_S,
};

/**
 * @class RTC_Date
 * @brief Represents a date and time
 */
class RTC_Date {
  public:
    /**
	 * @brief Default constructor.
	 */
    RTC_Date();

    /**
	 * @brief Constructor that takes date and time as strings.
	 * @param date The date string in the format "YYYY-MM-DD".
	 * @param time The time string in the format "HH:MM:SS".
	 */
    RTC_Date(const char *date, const char *time);

    /**
	 * @brief Constructor that takes individual date and time components.
	 * @param year The year.
	 * @param month The month.
	 * @param day The day.
	 * @param hour The hour.
	 * @param minute The minute.
	 * @param second The second.
	 */
    RTC_Date(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

    uint16_t year;  /**< The year. */
    uint8_t month;  /**< The month. */
    uint8_t day;    /**< The day. */
    uint8_t hour;   /**< The hour. */
    uint8_t minute; /**< The minute. */
    uint8_t second; /**< The second. */

    /**
	 * @brief Equality comparison operator.
	 * @param d The RTC_Date object to compare with.
	 * @return true if the objects are equal, false otherwise.
	 */
    bool operator==(RTC_Date d);

  private:
    /**
	 * @brief Converts a string to an unsigned 8-bit integer.
	 * @param pString The string to convert.
	 * @return The converted unsigned 8-bit integer.
	 */
    uint8_t StringToUint8(const char *pString);
};

/**
 * @class RTC_Alarm
 * @brief Represents an alarm configuration
 */
class RTC_Alarm {
  public:
    /**
	 * @brief Default constructor.
	 */
    RTC_Alarm(void);

    /**
	 * @brief Constructor that takes individual alarm components.
	 * @param minute The minute of the alarm.
	 * @param hour The hour of the alarm.
	 * @param day The day of the alarm.
	 * @param weekday The weekday of the alarm.
	 */
    RTC_Alarm(uint8_t minute, uint8_t hour, uint8_t day, uint8_t weekday);

    uint8_t minute;  /**< The minute of the alarm. */
    uint8_t hour;    /**< The hour of the alarm. */
    uint8_t day;     /**< The day of the alarm. */
    uint8_t weekday; /**< The weekday of the alarm. */
};

/**
 * @class PCF8563_Class
 * @brief Represents the PCF8563 RTC module
 */
class PCF8563_Class {
  public:
    /**
	 * @brief Initializes the PCF8563 RTC module.
	 * @param port The I2C port to use.
	 * @param addr The I2C address of the PCF8563 module.
	 * @return 0 if successful, error code otherwise.
	 */
    uint8_t begin(TwoWire &port = Wire, int addr = PCF8563_ADDRESS);

    /**
	 * @brief Converts an error code into descriptive text.
	 *
	 * This function takes an error code and returns a corresponding descriptive text. It can be used to convert
	 * error codes returned by the RTC into human-readable error messages.
	 *
	 * @param errorCode The error code to convert.
	 * @return A pointer to a constant character array containing the descriptive text of the error.
	 *         If the error code is not recognized, "Unknown error" is returned.
	 *
	 * @note The error codes and their meanings are as follows:
	 *   - 0: Success
	 *   - 1: I2C data too long to fit in transmit buffer
	 *   - 2: I2C received NACK on transmit of address
	 *   - 3: I2C received NACK on transmit of data
	 *   - 4: I2C other error
	 *   - 5: I2C timeout
	 *   - 6: bytesReceived(%i) != bytesRequested(%i)
	 *   - 7: Measurement out of range
	 */
    const char *getErrorText(uint8_t errorCode);

    /**
	 * @brief Sets the date and time of the RTC.
	 * @param year The year.
	 * @param month The month.
	 * @param day The day.
	 * @param hour The hour.
	 * @param minute The minute.
	 * @param second The second.
	 */
    void setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

    /**
	 * @brief Sets the date and time of the RTC using an RTC_Date object.
	 * @param date The RTC_Date object containing the date and time.
	 */
    void setDateTime(RTC_Date date);

    /**
	 * @brief Gets the current date and time from the RTC.
	 * @return The RTC_Date object representing the current date and time.
	 */
    RTC_Date getDateTime();

    /**
	 * @brief Gets the current alarm configuration from the RTC.
	 * @return The RTC_Alarm object representing the current alarm configuration.
	 */
    RTC_Alarm getAlarm();

    /**
	 * @brief Enables the alarm.
	 */
    void enableAlarm();

    /**
	 * @brief Disables the alarm.
	 */
    void disableAlarm();

    /**
	 * @brief Checks if the alarm is active.
	 * @return true if the alarm is active, false otherwise.
	 */
    bool alarmActive();

    /**
	 * @brief Resets the alarm.
	 */
    void resetAlarm();

    /**
	 * @brief Sets the alarm configuration.
	 * @param alarm The RTC_Alarm object representing the alarm configuration.
	 */
    void setAlarm(RTC_Alarm alarm);

    /**
	 * @brief Sets the alarm configuration using individual alarm components.
	 * @param hour The hour of the alarm.
	 * @param minute The minute of the alarm.
	 * @param day The day of the alarm.
	 * @param weekday The weekday of the alarm.
	 */
    void setAlarm(uint8_t hour, uint8_t minute, uint8_t day, uint8_t weekday);

    /**
	 * @brief Checks if the clock data can be trusted.
	 *
	 * The PCF8563 has an on-chip voltage-low detector. When Vdd drops below ~0.9V, the valid flag (bit) in the
	 * VL_seconds register is set to indicate that the integrity of the clock information is no longer guaranteed.
	 * The isValid() flag can be cleared by using the setDateTime() or syncToRTC() functions.
	 *
	 * @return true if the time is valid, false if the integrity is not guaranteed.
	 */
    bool isValid();

    /**
	 * @brief Sets the alarm to trigger on a specific weekday.
	 * @param weekday The weekday on which the alarm should trigger.
	 */
    void setAlarmByWeekDay(uint8_t weekday);

    /**
	 * @brief Sets the alarm to trigger at a specific hour.
	 * @param hour The hour at which the alarm should trigger.
	 */
    void setAlarmByHours(uint8_t hour);

    /**
	 * @brief Sets the alarm to trigger on a specific day of the month.
	 * @param day The day of the month on which the alarm should trigger.
	 */
    void setAlarmByDays(uint8_t day);

    /**
	 * @brief Sets the alarm to trigger at a specific minute.
	 * @param minute The minute at which the alarm should trigger.
	 */
    void setAlarmByMinutes(uint8_t minute);

    /**
	 * @brief Checks if the timer is enabled.
	 * @return true if the timer is enabled, false otherwise.
	 */
    bool isTimerEnable();

    /**
	 * @brief Checks if the timer is active.
	 * @return true if the timer is active, false otherwise.
	 */
    bool isTimerActive();

    /**
	 * @brief Enables the timer.
	 */
    void enableTimer();

    /**
	 * @brief Disables the timer.
	 */
    void disableTimer();

    /**
	 * @brief Sets the timer configuration.
	 * @param val The timer value.
	 * @param freq The timer frequency.
	 * @param enInterrupt Whether to enable the timer interrupt.
	 */
    void setTimer(uint8_t val, uint8_t freq, bool enInterrupt);

    /**
	 * @brief Clears the timer.
	 */
    void clearTimer();

    /**
	 * @brief Enables the CLK output with the specified frequency.
	 * @param freq The CLK output frequency.
	 * @return true if CLK output was enabled successfully, false otherwise.
	 */
    bool enableCLK(uint8_t freq);

    /**
	 * @brief Disables the CLK output, reducing power consumption from 550nA to 250nA.
	 */
    void disableCLK();

#ifdef ESP32
    /**
	 * @brief Sets the System (ESP32) time from the RTC if the RTC data is valid.
	 *
	 * @note This function creates a time struct from rtc.getDateTime() and then makes the epoch from this,
	 * assuming that the RTC outputs UTC/GMT. This function works with the system timezone already set.
	 *
	 * @return true if the time is valid, false if the integrity is not guaranteed.
	 */
    bool syncToSystem();

    /**
	 * @brief Sets the RTC time from the System (ESP32) time if the ESP32 time is between 1970 and 2100.
	 *
	 * @note This function uses the system epoch (seconds since 1970) to create a UTC Time struct
	 * (year, month, day, hours, etc.) and then sets the RTC to this.
	 *
	 * @return true if the time was set successfully, false otherwise.
	 */
    bool syncToRtc();
#endif

    /**
	 * @brief Formats the date and time as a string.
	 * @param style The style of the formatted string (default: PCF_TIMEFORMAT_HMS).
	 * @return The formatted date and time string.
	 */
    const char *formatDateTime(uint8_t style = PCF_TIMEFORMAT_HMS);

    /**
	 * @brief Gets the day of the week for a given date.
	 * @param day The day.
	 * @param month The month.
	 * @param year The year.
	 * @return The day of the week (0 = Sunday, 1 = Monday, ..., 6 = Saturday).
	 */
    uint32_t getDayOfWeek(uint32_t day, uint32_t month, uint32_t year);

    /**
	 * @brief Gets the status2 value of the PCF8563.
	 * @return The status2 value.
	 */
    uint8_t status2();

  private:
    /**
	 * @brief Converts a binary-coded decimal (BCD) value to decimal.
	 * @param val The BCD value to convert.
	 * @return The decimal value.
	 */
    uint8_t _bcd_to_dec(uint8_t val) {
        return ((val / 16 * 10) + (val % 16));
    }

    /**
	 * @brief Converts a decimal value to binary-coded decimal (BCD).
	 * @param val The decimal value to convert.
	 * @return The BCD value.
	 */
    uint8_t _dec_to_bcd(uint8_t val) {
        return ((val / 10 * 16) + (val % 10));
    }

    /**
	 * @brief Reads bytes from the specified register.
	 * @param reg The register address to read from.
	 * @param nbytes The number of bytes to read.
	 * @param data Pointer to the buffer to store the read data.
	 * @return 0 if successful, non-zero if there was an error.
	 */
    uint8_t _readByte(uint8_t reg, int nbytes, uint8_t *data) {
        _i2cPort->beginTransmission(_address);
        _i2cPort->write(reg);

        // Adapt to HYM8563, no stop bit is sent after reading the sending register address
        _i2cPort->endTransmission(false);
        _i2cPort->requestFrom(_address, nbytes, 1);  // HYM8563 send stop bit

        uint8_t index = 0;
        while (_i2cPort->available())
            data[index++] = _i2cPort->read();
        return 0;
    }

    /**
	 * @brief Writes bytes to the specified register.
	 * @param reg The register address to write to.
	 * @param nbytes The number of bytes to write.
	 * @param data Pointer to the buffer containing the data to write.
	 * @return 0 if successful, non-zero if there was an error.
	 */
    uint8_t _writeByte(uint8_t reg, uint8_t nbytes, uint8_t *data) {
        _i2cPort->beginTransmission(_address);
        _i2cPort->write(reg);
        for (uint8_t i = 0; i < nbytes; i++) {
            _i2cPort->write(data[i]);
        }
        return _i2cPort->endTransmission();
    }

    uint8_t _isValid = false;  // Flag indicating if the clock data is valid
    int _address;              // I2C address of the RTC module
    TwoWire *_i2cPort;         // Pointer to the I2C port
    uint8_t _data[16];         // Data buffer for reading from the RTC module
    bool _voltageLow;          // Flag indicating if the voltage is low
    char format[128];          // Format string for date and time formatting
};