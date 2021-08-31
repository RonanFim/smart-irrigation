// ----------------------------------------------------------------
//
//            AUTOMATIC IRRIGATION PROJECT
//
// Name: Ronan
// Date: 04/01/21
// Description: Create an automatic irrigation system using RTC
//              and a solenoid valve in order to water the
//              plants in a specific period of time.
//
// ----------------------------------------------------------------

#include <RTClib.h>
#include "TimePoint.h"

#define ONE_DAY_IN_SECS 86400
#define VALVE_PIN 4
#define RTC_FAIL_LED_PIN 6
// #define RTC_FAIL_LED_PIN LED_BUILTIN
// SDA: A4
// SCL: A5

#define LOOP_DELAY 30               // 30s
#define TIME_ERROR_THRESHOLD 120    // 2min

RTC_DS1307 rtc;
bool valve = false;

// Start and End times to turn the valve on (hours, minutes, seconds)
const TimePoint start_point(17, 00, 0);
const TimePoint end_point(17, 01, 0);
// const TimePoint start_point(17, 30, 0);
// const TimePoint end_point(17, 45, 0);
uint32_t start_time;
uint32_t end_time;

uint32_t last_timestamp;

template <typename T>
T diff(T foo, T bar) {
  return (foo > bar) ? (foo - bar) : (bar - foo);
}

void setup()
{
    while(!Serial)
        ;

    Serial.begin(57600);

    delay(3000);
    Serial.println("Inicio");

    // Configure and turn off the valve
    // Low level trigger: LOW = ON, HIGH = OFF
    pinMode(VALVE_PIN, OUTPUT);
    digitalWrite(VALVE_PIN, HIGH);
    
    pinMode(RTC_FAIL_LED_PIN, OUTPUT);
    digitalWrite(RTC_FAIL_LED_PIN, LOW);

    Serial.println("LED apagado");
    delay(3000);

    while(!rtc.begin())
    {
        digitalWrite(RTC_FAIL_LED_PIN, HIGH);
        Serial.println("Couldn't find RTC!");
        delay(10000);   // 10s
    }

    if(!rtc.isrunning())
    {
        digitalWrite(RTC_FAIL_LED_PIN, HIGH);
        // Set RTC in the first time
        Serial.println("RTC is not running");
        Serial.println("setting clock...");
        // (year, month, day, hour, minute, second)
        rtc.adjust(DateTime(2021, 8, 29, 17, 55, 0));
        Serial.println("RTC was set!");
    }

    digitalWrite(RTC_FAIL_LED_PIN, LOW);

    DateTime now = rtc.now();
    DateTime start(now.year(), now.month(), now.day(), start_point.hour, start_point.minute, start_point.second);
    DateTime end(now.year(), now.month(), now.day(), end_point.hour, end_point.minute, end_point.second);

    start_time = start.unixtime();
    end_time = end.unixtime();

    if(end_time <= start_time)
        end_time += ONE_DAY_IN_SECS;
    
    // Serial.println(start_time);
    // Serial.println(end_time);
    // Serial.println("start e end");

    last_timestamp = rtc.now().unixtime();
}

void loop() {
    DateTime now = rtc.now();

    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);

    uint32_t now_time = now.unixtime();

    if(diff(now_time, last_timestamp) > TIME_ERROR_THRESHOLD)
    {
        digitalWrite(RTC_FAIL_LED_PIN, HIGH);
        Serial.println("Time Error!");
        Serial.print("now time :");
        Serial.println(now_time);
        Serial.print("last time:");
        Serial.println(last_timestamp);
        Serial.print("Threshold:");
        Serial.println(TIME_ERROR_THRESHOLD);
        Serial.print("abs da dif:");
        Serial.println(abs(now_time - last_timestamp));

    }
    else
    {
        digitalWrite(RTC_FAIL_LED_PIN, LOW);
    }

    if(!valve)
    {
        // Serial.println("Checking to turn on");
        if((now_time >= start_time) && (now_time <= end_time))
        {
            valve = true;
            digitalWrite(VALVE_PIN, LOW);
            Serial.println("Valve turned on!");
        }
    }
    else
    {
        // Serial.println("Checking to turn off");
        if(now_time >= end_time)
        {
            valve = false;
            digitalWrite(VALVE_PIN, HIGH);
            Serial.println("Valve turned off!");
            // Change the start and end times to the next day
            start_time += ONE_DAY_IN_SECS;
            end_time += ONE_DAY_IN_SECS;
        }
    }
    
    // Serial.println(valve);

    last_timestamp += LOOP_DELAY;

    // Test in periods of 30 seconds
    delay(LOOP_DELAY * 1000);
}
