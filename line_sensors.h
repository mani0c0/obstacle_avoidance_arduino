#ifndef LINE_SENSORS_H
#define LINE_SENSORS_H

# define EMIT_PIN    11    // Documentation says 11.
# define LS_LEFT_PIN 12   // Complete for DN1 pin
# define LS_MIDLEFT_PIN 18   // Complete for DN2 pin
# define LS_MIDDLE_PIN 20   // Complete for DN3 pin
# define LS_MIDRIGHT_PIN 21   // Complete for DN4 pin
# define LS_RIGHT_PIN 22   // Complete for DN5 pin

int ls_pins[5] = {LS_LEFT_PIN,
                  LS_MIDLEFT_PIN,
                  LS_MIDDLE_PIN,
                  LS_MIDRIGHT_PIN,
                  LS_RIGHT_PIN };

class Line_Sensors_c{
  public:
    Line_Sensors_c(){
      
    }
  
    void initialise()
    {
      pinMode(EMIT_PIN, INPUT);
      pinMode(LS_LEFT_PIN, INPUT);
      pinMode(LS_MIDLEFT_PIN, INPUT);
      pinMode(LS_MIDDLE_PIN, INPUT);
      pinMode(LS_MIDRIGHT_PIN, INPUT);
      pinMode(LS_RIGHT_PIN, INPUT);
    }
  
    float readLineSensor(int i){
      if(i < 0 || i > 4){
        Serial.println("ERROR: INVALID PIN NUMBER");
        return -1;
      }
      int the_pin = ls_pins[i];
      
      pinMode( EMIT_PIN, OUTPUT );
      digitalWrite( EMIT_PIN, HIGH );
    
    
      pinMode( the_pin, OUTPUT );
      digitalWrite( the_pin, HIGH );
      delayMicroseconds( 10 );
    
    
      unsigned long start_time = micros();
      
      pinMode(the_pin, INPUT);
      
      while( digitalRead( the_pin ) == HIGH ) {
          // Do nothing here (waiting).
          if( micros() - start_time > 10000000){
            Serial.println("ERROR: THE CAPACITOR DIDN'T DISCHARGE");
            break;
          }
      }
    
      unsigned long end_time = micros();
    
      pinMode( EMIT_PIN, INPUT );
    
      unsigned long elapsed_time = end_time - start_time;
  
      return float(elapsed_time);
    }
};

#endif
