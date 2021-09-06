#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__


#define NUMBER_OF_TIME_POINTS 2                 // Número de vezes que a válvula será acionada por dia
#define LOOP_DELAY 30                           // [s] Delay em segundos entre cada loop de varredura

#define VALVE_PIN 4                             // Saída Digital que aciona a válvula
#define RTC_FAIL_LED_PIN 6                      // Saída Digital que liga o LED indicador de falha no RTC
#define RTC_LOW_BATTERY_LED_PIN 7               // Saída Digital que liga o LED indicador de bateria baixa do RTC
// #define RTC_LOW_BATTERY_LED_PIN LED_BUILTIN
#define RTC_BATTERY_AI_PIN A1                   // Entrada Analógica para ler tensão da bateria do RTC
#define RTC_BATTERY_ENABLE_READ 8               // Saída Digital que habilita a leitura da tensão da bateria do RTC
// SDA: A4
// SCL: A5

#define RTC_LOW_BATT_VOLT_THR (2.2)             // [V] Limiar de tensão em Volts para declarar bateria baixa do RTC
#define TIME_ERROR_THRESHOLD 1800               // [s] Limiar de tempo em segundos para determinar falha no RTC

#define ONE_DAY_IN_SECS 86400                   // [s] 1 dia em segundos (para somar ao timestamp de entrada e saída da válvula)
#define CYCLES_TO_READ_BATTERY 10               // Número de ciclos para realizar uma leitura da tensão da bateria do RTC


// Os tempos de início e fim de acionamento das válvulas são configurados no início
// da função Setup()

#endif
