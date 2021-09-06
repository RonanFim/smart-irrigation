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
#include "definitions.h"


// Start and End times to turn the valve on (hours, minutes, seconds)
uint32_t start_times[NUMBER_OF_TIME_POINTS];
uint32_t end_times[NUMBER_OF_TIME_POINTS];
uint8_t current_time_point_idx = 0;

RTC_DS1307 rtc;
bool valve = false;

uint32_t last_timestamp;
uint8_t batt_read_counter = 0;

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
    start_points[0].setTime(19,10,0);
    end_points[0].setTime(19,12,0);
    // 2nd
    start_points[1].setTime(19,15,0);
    end_points[1].setTime(19,16,0);


    while(!Serial)
        ;

    Serial.begin(57600);

    Serial.println();
    Serial.println("-------------------------");
    Serial.println("--------- Reset ---------");
    Serial.println("-------------------------");
    delay(1000);

    Serial.println("Setting things up...");

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

    // Configure and disable Battery reading
    pinMode(RTC_BATTERY_ENABLE_READ, OUTPUT);
    digitalWrite(RTC_BATTERY_ENABLE_READ, LOW);

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

    Serial.println("Setup completed!");
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
        Serial.print("Time Error! - Diff: ");
        Serial.println(diff(now_time, last_timestamp));
    }
    else
    {
        digitalWrite(RTC_FAIL_LED_PIN, LOW);
    }

    if(!valve)
    {
        // Serial.println("Checking to turn valve on");
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
        // Serial.println("Checking to turn valve off");
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

    batt_read_counter++;
    if (batt_read_counter >= CYCLES_TO_READ_BATTERY)
    {
        // Check RTC Battery Voltage
        digitalWrite(RTC_BATTERY_ENABLE_READ, HIGH);
        delay(100);
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
            Serial.print("Battery OK! - ");
            Serial.print(batVolt);
            Serial.println("V");
        }
        digitalWrite(RTC_BATTERY_ENABLE_READ, LOW);
        batt_read_counter = 0;
    }
    
    // Serial.println(valve);

    last_timestamp += LOOP_DELAY;
    delay(LOOP_DELAY * 1000);
}
