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
#ifdef DEBUG_ENABLED
  #define DEBUG(x, val) Serial.print(F(x)); Serial.println(val);
#else
  #define DEBUG(x, val) 
#endif

#endif /* UTILS_H */
