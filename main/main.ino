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

#define START_HOUR 20
#define START_MIN  49
#define START_SEC  0
#define END_HOUR 20
#define END_MIN  50
#define END_SEC  0

RTC_DS1307 rtc;
bool valve = false;

void setup()
{
    while(!Serial)
        ;

    Serial.begin(57600);

    while(!rtc.begin())
    {
        Serial.println("Couldn't find RTC!");
        delay(10000);   // 10s
    }

    if(!rtc.isrunning())
    {
        Serial.println("RTC is not running");
        Serial.println("setting clock...");
        rtc.adjust(DateTime(2021, 1, 4, 20, 11, 0));
        Serial.println("RTC was set!");
    }
  
}

void loop() {
    DateTime now = rtc.now();

    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);

    if(!valve)
    {
        Serial.println("Checando para entrar");
        if((now.hour() >= START_HOUR) && (now.hour() <= END_HOUR))
            if((now.minute() >= START_MIN) && (now.minute() <= END_MIN))
                if((now.second() >= START_SEC) && (now.second() <= END_SEC)) {
                    valve = true;
                    Serial.println("valve ativada");
                }
    }
    else
    {
        Serial.println("Checando para sair");
        if(now.hour() >= END_HOUR)
            if(now.minute() >= END_MIN)
                if(now.second() >= END_SEC) {
                    valve = false;
                    Serial.println("valve desativada");
                }
    }
    
    Serial.println(valve);

    /*Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print(" (");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println(")");*/
    delay(5000);
}
