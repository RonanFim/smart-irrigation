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
// #define RTC_LOW_BATTERY_LED_PIN 7
#define RTC_LOW_BATTERY_LED_PIN LED_BUILTIN
#define RTC_BATTERY_AI_PIN A1
#define RTC_LOW_BATT_VOLT_THR (2.2)  // 2.2V
// SDA: A4
// SCL: A5

#define LOOP_DELAY 30               // 30s
#define TIME_ERROR_THRESHOLD 120    // 2min

#define NUMBER_OF_TIME_POINTS 2     // Number of times the valve will activate per day
// Start and End times to turn the valve on (hours, minutes, seconds)
uint32_t start_times[NUMBER_OF_TIME_POINTS];
uint32_t end_times[NUMBER_OF_TIME_POINTS];
uint8_t current_time_point_idx = 0;

RTC_DS1307 rtc;
bool valve = false;


uint32_t last_timestamp;

template <typename T>
T diff(T foo, T bar) {
  return (foo > bar) ? (foo - bar) : (bar - foo);
}

void setup()
{
    // ------------------------------------
    // Times to activate valve
    // SET TIMES HERE!!!
    // ------------------------------------
    TimePoint start_points[NUMBER_OF_TIME_POINTS];
    TimePoint end_points[NUMBER_OF_TIME_POINTS];
    // 1st
    start_points[0].setTime(20,27,0);
    end_points[0].setTime(20,28,0);
    // 2nd
    start_points[1].setTime(20,30,0);
    end_points[1].setTime(20,32,0);


    while(!Serial)
        ;

    Serial.begin(57600);

    Serial.println("------ Reset ------");
    delay(3000);

    // Configure and turn off the valve
    // Low level trigger: LOW = ON, HIGH = OFF
    pinMode(VALVE_PIN, OUTPUT);
    digitalWrite(VALVE_PIN, HIGH);
    
    // Configure and turn off RTC Fail LED
    pinMode(RTC_FAIL_LED_PIN, OUTPUT);
    digitalWrite(RTC_FAIL_LED_PIN, LOW);

    // Configure and turn off RTC Low Battery LED
    pinMode(RTC_LOW_BATTERY_LED_PIN, OUTPUT);
    digitalWrite(RTC_LOW_BATTERY_LED_PIN, LOW);

    while(!rtc.begin())
    {
        digitalWrite(RTC_FAIL_LED_PIN, HIGH);
        Serial.println("Couldn't find RTC!");
        delay(10000);   // 10s
    }

    while(!rtc.isrunning())
    {
        digitalWrite(RTC_FAIL_LED_PIN, HIGH);
        // Set RTC in the first time
        Serial.println("RTC is not running");
        Serial.println("setting clock...");
        // (year, month, day, hour, minute, second)
        rtc.adjust(DateTime(2021, 9, 1, 21, 0, 0));
        Serial.println("RTC was set!");
        delay(10000);       // 10s
    }

    digitalWrite(RTC_FAIL_LED_PIN, LOW);

    DateTime now = rtc.now();

    for(uint8_t i = 0; i < NUMBER_OF_TIME_POINTS; i++)
    {
        DateTime start(now.year(), now.month(), now.day(), 
                    start_points[i].hour,
                    start_points[i].minute,
                    start_points[i].second);
        DateTime end(now.year(), now.month(), now.day(),
                    end_points[i].hour,
                    end_points[i].minute,
                    end_points[i].second);

        start_times[i] = start.unixtime();
        end_times[i] = end.unixtime();

        if(end_times[i] <= start_times[i])
            end_times[i] += ONE_DAY_IN_SECS;
    }

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
        for(uint8_t i = 0; i < NUMBER_OF_TIME_POINTS; i++)
        {
            if((now_time >= start_times[i]) && (now_time <= end_times[i]))
            {
                valve = true;
                digitalWrite(VALVE_PIN, LOW);
                Serial.println("Valve turned on!");
                current_time_point_idx = i;
            }
        }
    }
    else
    {
        // Serial.println("Checking to turn off");
        if(now_time >= end_times[current_time_point_idx])
        {
            valve = false;
            digitalWrite(VALVE_PIN, HIGH);
            Serial.println("Valve turned off!");
            // Change the start and end times to the next day
            start_times[current_time_point_idx] += ONE_DAY_IN_SECS;
            end_times[current_time_point_idx] += ONE_DAY_IN_SECS;
        }
    }

    // Check RTC Battery Voltage
    float batVolt = (analogRead(RTC_BATTERY_AI_PIN)*5.0)/1024.0;
    if(batVolt < RTC_LOW_BATT_VOLT_THR)
    {
        digitalWrite(RTC_LOW_BATTERY_LED_PIN, HIGH);
        Serial.print("Low Battery! - ");
        Serial.print(batVolt);
        Serial.println("V");
    }
    else
    {
        digitalWrite(RTC_LOW_BATTERY_LED_PIN, LOW);
        // Serial.print("Battery OK! - ");
        // Serial.print(batVolt);
        // Serial.println("V");
    }
    
    // Serial.println(valve);

    last_timestamp += LOOP_DELAY;

    // Test in periods of 30 seconds
    delay(LOOP_DELAY * 1000);
}
