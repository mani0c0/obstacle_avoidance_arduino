#ifndef BUMP_SENSORS_H
#define BUMP_SENSORS_H

# define EMIT_PIN 11
# define BL_PIN 4 
# define BR_PIN 5   

int bump_pins[2] = {BL_PIN, BR_PIN };

class Bump_Sensors_c{
  public:
    Bump_Sensors_c(){
      
    }
    float ambient_readings[2];
    float avg_readings[2];
    
    void initialise()
    {
      pinMode(EMIT_PIN, INPUT);
      pinMode(BL_PIN, INPUT);
      pinMode(BR_PIN, INPUT);
    }
  
    float readBumpSensor(int i){                           //measure relative IR intensity by checking capacitor discharge times
      if(i < 0 || i > 2){
        Serial.println("ERROR: INVALID PIN NUMBER");
        return -1;
      }
      int the_pin = bump_pins[i];
      
      pinMode( EMIT_PIN, OUTPUT );
      digitalWrite( EMIT_PIN, LOW );
    
    
      pinMode( the_pin, OUTPUT );                         //charge
      digitalWrite( the_pin, HIGH );
      delayMicroseconds( 10 );
    
    
      unsigned long start_time = micros();
      
      pinMode(the_pin, INPUT);                          //discharge
      
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

    void get_avg_readings(int sample_count = 100){
      float avg_reading;

      for( int i = 0; i < 2; i++ ) {
        avg_reading = 0;        
        for (int j = 0; j < sample_count; j++)
             avg_reading = avg_reading + readBumpSensor(i);
        avg_readings[i] = avg_reading / sample_count;
      }
    }
};

#endif
