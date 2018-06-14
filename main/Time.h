#ifndef TIME_H  
#define TIME_H

#include "DS3231.h"

DS3231 Clock;
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

#define EXPONENTIAL_FILTER(last, new, alpha) last=(((last * alpha) + new * (1.0 - alpha)))

void initTime(TimeType& time) {
  Wire.begin();
  time.temperature = Clock.getTemperature();
}

void setTime(TimeType* t) {
  Clock.setSecond(t->second);//Set the second 
  Clock.setMinute(t->minute);//Set the minute 
  Clock.setHour(t->hour);  //Set the hour 
  //Clock.setDoW(5);    //Set the day of the week
  Clock.setDate(t->day);  //Set the day of the month
  Clock.setMonth(t->month);  //Set the month of the year
  Clock.setYear(t->year);// - 2000);  //Set the year (Last two digits of the year)
}

void readTime(TimeType *t) {
  //Clock.getTime(t->year, t->month, t->day, t->dow, t->hour, t->minute, t->second);
  bool h12, PM, Century;
  t->hour = Clock.getHour(h12, PM); 
  t->minute = Clock.getMinute(); 
  t->second = Clock.getSecond();
  t->year = Clock.getYear();
  t->month = Clock.getMonth(Century); 
  t->day = Clock.getDate(); 
  EXPONENTIAL_FILTER(t->temperature, Clock.getTemperature(), 0.98);
}

void printTime(TimeType* t) {
  char tempBuff[6];
  char totalBuff[30];
  dtostrf(t->temperature, 4, 1, tempBuff);
  sprintf(totalBuff, "%02d:%02d:%02d, %02d.%02d.%04d; %s C\n", t->hour, t->minute, t->second, t->day, t->month, t->year + 2000, tempBuff); 
  #ifdef DEBUG_ENABLED
  Serial.write(totalBuff);
  #endif
}

void parseCompileTime(TimeType* t){
    char s_month[5];
    const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    uint16_t year;
    sscanf(__DATE__, "%s %c %d", s_month, &t->day, &year);
    t->year = year - 2000;//1970;//2000;
    t->month = (strstr(month_names, s_month) - month_names)/3 + 1;
    #ifdef DEBUG_ENABLED
    Serial.print("DEBUG: ");
    Serial.println((strstr(month_names, s_month) - month_names));
    #endif
    sscanf(__TIME__, "%2hhd %*c %2hhd %*c %2hhd", &t->hour, &t->minute, &t->second);
}  


/*uint16_t getSmallTimeStamp(TimeType* t) {
  uint16_t totalTime = 0;
  totalTime = t->second;
  totalTime = t->minute * 60;
  totalTime = t->hour * 60 * 60;
  return totalTime;
}
*/

/*
  if (Century) {      // Won't need this for 89 years.
    Serial.print("1");
  } else {
    Serial.print("0");
  }
  // and the day of the week
  //Serial.print(Clock.getDoW(), DEC);
  // Add AM/PM indicator
  if (h12) {
    if (PM) {
      Serial.print(" PM ");
    } else {
      Serial.print(" AM ");
    }
  } else {
    Serial.print(" 24h ");
  }
  // Tell whether the time is (likely to be) valid
  if (Clock.oscillatorCheck()) {
    Serial.print(" O+");
  } else {
    Serial.print(" O-");
  }*/
  // Indicate whether an alarm went off
  /*if (Clock.checkIfAlarm(1)) {
    Serial.print(" A1!");
  }
  if (Clock.checkIfAlarm(2)) {
    Serial.print(" A2!");
  }*/
/*  Serial.print("Alarm 1: ");
  Clock.getA1Time(ADay, AHour, AMinute, ASecond, ABits, ADy, A12h, Apm);
  Serial.print(ADay, DEC);
  if (ADy) {
    Serial.print(" DoW");
  } else {
    Serial.print(" Date");
  }
  if (A12h) {
    if (Apm) {
      Serial.print('pm ');
    } else {
      Serial.print('am ');
    }
  }
  if (Clock.checkAlarmEnabled(1)) {
    Serial.print("enabled");
  }
  Serial.print('\n');
  // Display Alarm 2 information
  Serial.print("Alarm 2: ");
  Clock.getA2Time(ADay, AHour, AMinute, ABits, ADy, A12h, Apm);
  Serial.print(ADay, DEC);
  if (ADy) {
    Serial.print(" DoW");
  } else {
    Serial.print(" Date");
  }
  Serial.print(' ');
  Serial.print(AHour, DEC);
  Serial.print(' ');
  Serial.print(AMinute, DEC);
  Serial.print(' ');
  if (A12h) {
    if (Apm) {
      Serial.print('pm');
    } else {
      Serial.print('am');
    }
  }
  if (Clock.checkAlarmEnabled(2)) {
    Serial.print("enabled");
  }*/
  /* display alarm bits
  Serial.print('\n');
  Serial.print('Alarm bits: ');
  Serial.print(ABits, DEC);
  */

#endif
