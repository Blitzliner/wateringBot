#ifndef UTILS_H  
#define UTILS_H 
#include "types.h" /* is debugging enabled */

//#define DEBUG_ENABLED
#define FIRST_RUN true /* disable after first upload!! */
#define TEST_RUN false

#define SERIAL_BAUD_9600 9600
#define SERIAL_BAUD_115200 115200

#define CLAMP(val, min, max) val = ((val)<(min)?(min):((val)>(max)?(max):(val)))

#define EXPONENTIAL_FILTER(last, new, alpha) last=(((last * alpha) + new * (1.0 - alpha)))
#define IS_ENABLED(flag) (flag == true)

#ifdef DEBUG_ENABLED
    #define DEBUG_INIT(baudrate)    Serial.begin(baudrate)
    #define DEBUG_OPT(str)          Serial.println(F(str))
    #define DEBUG_VALUE(val)        Serial.println(val)
    #define DEBUG(str)              Serial.println(str)
#else
    #define DEBUG_INIT(baudrate) 
    #define DEBUG_OPT(str)  
    #define DEBUG_VALUE(val) 
    #define DEBUG(str)
#endif

#endif /* UTILS_H */
