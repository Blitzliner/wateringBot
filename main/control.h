#include "types.h"
#include "time.h"
#include "utils.h"

#define MILLILITERS_PER_SECOND 50
#define PIN_PUMP 5
#define PIN_OUTLET_1 6 /* following outlets has to be in ascending pin order 6, 7, 8, 9, 10, 11, 12(max)*/

static uint32_t _lastRun[WATER_OUTLET_MAX] = {0}; /* in hours */
static TimeType *_t;
uint16_t _getHours();
void _watering(WateringBoy_DataType* data, int8_t &currentOutlet, boolean &mutexPump, bool& testRun);
void _controlPins(const int8_t currentOutlet, const boolean mutexPump);
void _checkOutlets(WateringBoy_DataType* data, int8_t& lastIdx, bool& mutexPump, bool& testRun);

void _watering(WateringBoy_DataType* data, int8_t &currentOutlet, boolean &mutexPump, bool& testRun) {
  static uint16_t pumpStartTime = 0; /* in seconds */
  
  if (currentOutlet != -1) {
    /* check if pump is running */
    if (mutexPump == false) {
      /* enable pump, start time measurement*/
      mutexPump = true;
      pumpStartTime = millis()/1000;
    } else { 
      /* pump already running, update run time */
      uint16_t runTime = millis()/1000 - pumpStartTime;
      uint16_t totalMilliliter = data->out[currentOutlet][SETTING_OUT_AMOUNT].value + data->out[currentOutlet][SETTING_OUT_AMOUNT].value;
      /* wait if enough water runs out */
      if (   (MILLILITERS_PER_SECOND * runTime >= totalMilliliter)
          || (   (testRun == true) 
              && (data->out[currentOutlet][SETTING_OUT_TESTRUN].value == 0))) {
        /* disable pump, so now its free for other outlets */
        mutexPump = false;
        testRun = false;
        _lastRun[currentOutlet] = millis()/1000;
        DEBUG("id", currentOutlet);
        DEBUG("v", _lastRun[currentOutlet]);
        currentOutlet = -1;
      } 
    }

    /* de-/activate the electric */
    _controlPins(currentOutlet, mutexPump);
  }
}

void _controlPins(const int8_t currentOutlet, const boolean mutexPump) {
  if (mutexPump == true) {
    digitalWrite(PIN_PUMP, true);
    DEBUG("P:", 1);
  } else {
    digitalWrite(PIN_PUMP, false);
    DEBUG("P:", 0);
  }
  
  if (currentOutlet == -1) {
    for (uint8_t idx = 0; idx < WATER_OUTLET_MAX; idx++) {
      /* disable all outlets */
      digitalWrite(PIN_OUTLET_1 + idx, false);  
    }      
  } else {
    digitalWrite(PIN_OUTLET_1 + currentOutlet, true);  
    DEBUG("V:", currentOutlet);
  }
}

uint16_t _getHours() {
  return millis();
}

void _checkOutlets(WateringBoy_DataType* data, int8_t& lastIdx, bool& mutexPump, bool& testRun) {
  for (uint8_t idx = 0; idx < WATER_OUTLET_MAX; idx++) {
    /* first run ever */
    if (_lastRun[idx] == 0) {
        /* check for right time and if pump is not bussy */
        if (   (_t->hour == data->out[idx][SETTING_OUT_DAYTIME].value)
            && (mutexPump == false)) {
           lastIdx = idx;
           DEBUG("", 1000);
        }
    } else { /* normal run */
      if (   ((_lastRun[idx] + data->out[idx][SETTING_OUT_CYCLE].value*60*60) == (millis()/1000))
          && (mutexPump == false)) {
        lastIdx = idx;
         DEBUG("", 2000);
      }
    }
    /*test run is enabled*/
    if (   (data->out[idx][SETTING_OUT_TESTRUN].value == 1)
        && (mutexPump == false)) {
      lastIdx = idx;
      testRun = true;
      DEBUG("", 3000);
    }
  }
}

void init_control(TimeType *time) {
  _t = time;
  pinMode(PIN_PUMP, OUTPUT);
  for (uint8_t idx = 0; idx < WATER_OUTLET_MAX; idx++) 
    pinMode(PIN_OUTLET_1 + idx, OUTPUT);  
}

void run_control(WateringBoy_DataType* data) {
  static int8_t currentOutlet = -1;
  static boolean mutexPump = false;
  static boolean testRun = false;
  
  /* check all outlets if they want to watering the plants */
  _checkOutlets(data, currentOutlet, mutexPump, testRun);

  /* an outlet wants to watering */
  _watering(data, currentOutlet, mutexPump, testRun);
}

