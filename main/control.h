#include "types.h"
#include "time.h"
#include "utils.h"

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
      uint16_t totalMilliliter = data->Out_as[currentOutlet][SETTING_OUT_AMOUNT].Value_s16 + data->Out_as[currentOutlet][SETTING_OUT_AMOUNT].Value_s16;
      /* wait if enough water runs out */
      if (   (data->Outlets_s.FlowAmount_s.Value_s16 * runTime >= totalMilliliter)
          || (   (testRun == true) 
              && (data->Out_as[currentOutlet][SETTING_OUT_TESTRUN].Value_s16 == 0))) {
        /* disable pump, so now its free for other outlets */
        mutexPump = false;
        testRun = false;
        _lastRun[currentOutlet] = millis()/1000;
        char out[8];
        sprintf(out, "ID%1d", currentOutlet);
        DEBUG(out);
        sprintf(out, "V%6lu", _lastRun[currentOutlet]);
        DEBUG(out);
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
        DEBUG("P1");
    } else {
        digitalWrite(PIN_PUMP, false);
        DEBUG("P0");
    }
  
    if (currentOutlet == -1) {
        /* disable all outlets */
        for (uint8_t idx_u8 = 0; idx_u8 < WATER_OUTLET_MAX; idx_u8++) {
            digitalWrite(PIN_OUTLET_1 + idx_u8, false);  
        }      
    } else {
        digitalWrite(PIN_OUTLET_1 + currentOutlet, true);  
        char out[3];
        sprintf(out, "V%1d", currentOutlet);
        DEBUG(out);
    }
}

uint16_t _getHours() {
  return millis();
}

void _checkOutlets(WateringBoy_DataType* data, int8_t& lastIdx, bool& mutexPump, bool& testRun) {
    for (uint8_t idx_u8 = 0; idx_u8 < WATER_OUTLET_MAX; idx_u8++) {
    /* first run ever */
        if (_lastRun[idx_u8] == 0) {
        /* check for right time and if pump is not bussy */
            if (   (_t->hour == data->Out_as[idx_u8][SETTING_OUT_DAYTIME].Value_s16)
                && (mutexPump == false)) {
                lastIdx = idx_u8;
                DEBUG("V1");
            }
        } else { /* normal run */
            if (   ((_lastRun[idx_u8] + data->Out_as[idx_u8][SETTING_OUT_CYCLE].Value_s16*60*60) == (millis()/1000))
                && (mutexPump == false)) {
                lastIdx = idx_u8;
                DEBUG("V2");
            }
        }
        /*test run is enabled*/
        if (   (data->Out_as[idx_u8][SETTING_OUT_TESTRUN].Value_s16 == 1)
            && (mutexPump == false)) {
            lastIdx = idx_u8;
            testRun = true;
            DEBUG("V3");
        }
    }
}

void Control_Init(TimeType *time) {
  _t = time;
  pinMode(PIN_PUMP, OUTPUT);
  for (uint8_t idx = 0; idx < WATER_OUTLET_MAX; idx++) 
    pinMode(PIN_OUTLET_1 + idx, OUTPUT);  
}

void Control_Main(WateringBoy_DataType* data) {
  static int8_t currentOutlet = -1;
  static boolean mutexPump = false;
  static boolean testRun = false;
  
  /* check all outlets if they want to watering the plants */
  _checkOutlets(data, currentOutlet, mutexPump, testRun);

  /* an outlet wants to watering */
  _watering(data, currentOutlet, mutexPump, testRun);
}

