#include <Wire.h>
#include "pcf8563.h"

#include <WiFi.h>
#include "sntp.h"
#include "time.h"

PCF8563_Class rtc;

const char *ssid = "wifi";
const char *password = "password";

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const char *ntpServer3 = "time.google.com";

void setup() {
    Serial.begin(115200);
    Wire.begin();
    rtc.begin();

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char *)ntpServer1);
    sntp_setservername(1, (char *)ntpServer2);
    sntp_setservername(2, (char *)ntpServer3);
    sntp_init();
}

void loop() {
    // connect to WiFi
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    uint8_t retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" CONNECTED");

        struct tm timeinfo;
        time_t now;
        while (getLocalTime(&timeinfo) == false) {
            Serial.println("Waiting for NTP");
            vTaskDelay(950 / portTICK_PERIOD_MS);
        }
        rtc.syncToRtc();
    } else {
        Serial.println("Wifi Connect Failed");
    }

    delay(1000);
}
