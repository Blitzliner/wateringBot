#ifndef TIME_H  
#define TIME_H

#include "DS3231.h"
#include "utils.h"
#include "types.h"

/*************************************************/
/*         CONSTANT DEFINES                      */
/*************************************************/
#define TEMPERATURE_FILTER 0.98

/*************************************************/
/*         LOCAL VARIABLE DECLARATION            */
/*************************************************/
DS3231 Clock;

/*************************************************/
/*              TYPE DECLARATION                 */
/*************************************************/
typedef struct TimeType_Tag {
  byte second;
  byte minute;
  byte hour;
  byte day;
  //byte dow;
  byte month;
  byte year;
  float temperature; 
} TimeType;

/*************************************************/
/*         LOCAL FUNCTION DECLARATION            */
/*************************************************/
static void PrintTime(TimeType* t);

#ifdef FIRST_RUN
static void parseCompileTime(TimeType* t);
#endif
/*************************************************/
/*         GLOBAL FUNCTION DECLARATION           */
/*************************************************/
void Time_Init(TimeType& time);
void SetTime(TimeType* t);
void GetTime(TimeType *t);

/*************************************************/
/*         LOCAL FUNCTION DEFINITIONS            */
/*************************************************/
void PrintTime(TimeType* t) {
    char tempBuff[6];
    char totalBuff[30];
    dtostrf(t->temperature, 4, 1, tempBuff);
    sprintf(totalBuff, "%02d:%02d:%02d, %02d.%02d.%04d; %s C\n", t->hour, t->minute, t->second, t->day, t->month, t->year + 2000, tempBuff);
    DEBUG(totalBuff);
}

#ifdef FIRST_RUN
void parseCompileTime(TimeType* t) {
    char s_month[5];
    const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    uint16_t year;
    sscanf(__DATE__, "%s %c %d", s_month, &t->day, &year);
    t->year = (char)(year - 2000);
    t->month = (strstr(month_names, s_month) - month_names) / 3 + 1;
    DEBUG((strstr(month_names, s_month) - month_names));
    sscanf(__TIME__, "%2hhd %*c %2hhd %*c %2hhd", &t->hour, &t->minute, &t->second);
}
#endif
/*************************************************/
/*         GLOBAL FUNCTION DEFINITIONS            */
/*************************************************/
void Time_Init(TimeType& time) {
    Wire.begin();
    time.temperature = Clock.getTemperature();
    #if defined(FIRST_RUN)
        parseCompileTime(&time);
        SetTime(&time);
    #endif
    PrintTime(&time);
}

void SetTime(TimeType* t) {
  Clock.setSecond(t->second);//Set the second 
  Clock.setMinute(t->minute);//Set the minute 
  Clock.setHour(t->hour);  //Set the hour 
  //Clock.setDoW(5);    //Set the day of the week
  Clock.setDate(t->day);  //Set the day of the month
  Clock.setMonth(t->month);  //Set the month of the year
  Clock.setYear(t->year);// - 2000);  //Set the year (Last two digits of the year)
}

void GetTime(TimeType *t) {
  bool h12, PM, Century;
  t->hour = Clock.getHour(h12, PM); 
  t->minute = Clock.getMinute(); 
  t->second = Clock.getSecond();
  t->year = Clock.getYear();
  t->month = Clock.getMonth(Century); 
  t->day = Clock.getDate(); 
  EXPONENTIAL_FILTER(t->temperature, Clock.getTemperature(), TEMPERATURE_FILTER);
}

#endif
