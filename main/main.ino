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

RTC_DS1307 rtc;
bool valve = false;

// Start and End times to turn the valve on
const TimePoint start_point(20, 42, 0);
const TimePoint end_point(20, 43, 0);
uint32_t start_time;
uint32_t end_time;

void setup()
{
    while(!Serial)
        ;

    Serial.begin(57600);

    // Configure and turn on the valve
    pinMode(VALVE_PIN, OUTPUT);
    digitalWrite(VALVE_PIN, LOW);

    while(!rtc.begin())
    {
        Serial.println("Couldn't find RTC!");
        delay(10000);   // 10s
    }

    if(!rtc.isrunning())
    {
        // Set RTC in the first time
        Serial.println("RTC is not running");
        Serial.println("setting clock...");
        // (year, month, day, hour, minute, second)
        rtc.adjust(DateTime(2021, 1, 4, 20, 11, 0));
        Serial.println("RTC was set!");
    }

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
}

void loop() {
    DateTime now = rtc.now();

    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);

    uint32_t now_time = now.unixtime();

    if(!valve)
    {
        // Serial.println("Checking to turn on");
        if((now_time >= start_time) && (now_time <= end_time))
        {
            valve = true;
            digitalWrite(VALVE_PIN, HIGH);
            Serial.println("Valve turned on!");
        }
    }
    else
    {
        // Serial.println("Checking to turn off");
        if(now_time >= end_time)
        {
            valve = false;
            digitalWrite(VALVE_PIN, LOW);
            Serial.println("Valve turned off!");
            // Change the start and end times to the next day
            start_time += ONE_DAY_IN_SECS;
            end_time += ONE_DAY_IN_SECS;
        }
    }
    
    // Serial.println(valve);

    // Test in periods of 5 seconds
    delay(5000);
}
