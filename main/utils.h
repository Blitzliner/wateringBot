#ifndef UTILS_H  
#define UTILS_H 

#define SERIAL_BAUD_9600 9600
#define SERIAL_BAUD_115200 115200

/*static Stream *_stream = NULL;

void utils_init(Stream &port, uint16_t baudrate = SERIAL_BAUD_115200) {
  _stream = &port;
   _stream->begin(baudrate);
   //Serial.println("okay");
}
*/

#define CLAMP(val, min, max) val = ((val)<(min)?(min):((val)>(max)?(max):(val)))

#define EXPONENTIAL_FILTER(last, new, alpha) last=(((last * alpha) + new * (1.0 - alpha)))

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
