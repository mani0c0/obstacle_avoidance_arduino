#ifndef _ENCODERS_H
#define _ENCODERS_H

#define ENCODER_0_A_PIN  7
#define ENCODER_0_B_PIN  23
#define ENCODER_1_A_PIN  26
//#define ENCODER_1_B_PIN Non-standard pin!


// Volatile Global variables used by Encoder ISR.
volatile long count_eR; // used by encoder to count the rotation
volatile byte state_eR;
volatile long count_eL;
volatile byte state_eL;
volatile long count_eR_prev;
volatile long count_eL_prev;
  
class odom{
  public:
    odom(){
      
    }
    float X_prev=0.0, Y_prev = 0.0, Theta_prev =0.0;
    
    void update_location(){
      float phi_R = float(count_eR - count_eR_prev) / 358.3 * 2 * PI ;
      float phi_L = float(count_eL - count_eL_prev) / 358.3 * 2 * PI ;

      


      float x = 0.016 * (phi_R + phi_L) / 2;
      float theta = 0.016 * (phi_R - phi_L) / (0.09);                 //CHANGED THE SIGN
    
      X_prev = X_prev + x * cos (Theta_prev);                               //CHANGED SIN AND COS
      Y_prev = Y_prev +  x * sin(Theta_prev);
      Theta_prev = Theta_prev + theta;
    
      count_eR_prev = count_eR;
      count_eL_prev = count_eL;
  }
};

// This ISR handles just Encoder 0
// ISR to read the Encoder0 Channel A and B pins
// and then look up based on  transition what kind of
// rotation must have occured.
ISR( INT6_vect ) {
  // We know that the ISR is only called when a pin changes.
  // We also know only 1 pin can change at a time.
  // The XOR(AB) signal change from "Channel A" triggers ISR.
 
  // First, Read in the new state of the encoder pins.
  // Standard pins, so standard read functions.
  boolean eR_B = digitalRead( ENCODER_0_B_PIN ); // normal B state
  boolean eR_A = digitalRead( ENCODER_0_A_PIN ); // XOR(AB)
 
  // Software XOR (^) logically infers
  // the true value of A given the state of B
  eR_A = eR_A ^ eR_B;

  // Shift our (new) current readings into bit positions
  // 2 and 3 in the state variable (current state)
  // State: (bit3)  (bit2)  (bit1)   (bit0)
  // State:  new B   new A   old B   old A
    state_eR = state_eR | ( eR_B  << 3 );
    state_eR = state_eR | ( eR_A  << 2 );

    // Handle which transition we have registered.
    // Complete this if statement as necessary.
    // Refer to the labsheet. 
    if ( state_eR == 1 |  state_eR == 7 | state_eR == 8 | state_eR == 14 ) {
      count_eR -= 1;
    }
    else if (state_eR == 2 | state_eR == 4 | state_eR == 11 | state_eR == 13){
      count_eR += 1;
    } // Continue this if statement as necessary.
    else
      Serial.println("Encoder state change error");



    // Shift the current readings (bits 3 and 2) down
    // into position 1 and 0 (to become prior readings)
    // This bumps bits 1 and 0 off to the right, "deleting"
    // them for the next ISR call. 
    state_eR = state_eR >> 2;

}


// This ISR handles just Encoder 1
// ISR to read the Encoder0 Channel A and B pins
// and then look up based on  transition what kind of
// rotation must have occured.
ISR( PCINT0_vect ) {
 
    // First, Read in the new state of the encoder pins.

    // Mask for a specific pin from the port.
    // Non-standard pin, so we access the register
    // directly.  
    // Reading just PINE would give us a number
    // composed of all 8 bits.  We want only bit 2.
    // B00000100 masks out all but bit 2
    // It is more portable to use the PINE2 keyword.
    boolean eL_B = PINE & (1<<PINE2);
    //boolean eL_B = PINE & B00000100;  // Does same as above.

    // Standard read from the other pin.
    boolean eL_A = digitalRead( ENCODER_1_A_PIN ); // 26 the same as A8


    eL_A = eL_A ^ eL_B;

    // Create a bitwise representation of our states
    // We do this by shifting the boolean value up by
    // the appropriate number of bits, as per our table
    // header:
    //
    // State :  (bit3)  (bit2)  (bit1)  (bit0)
    // State :  New A,  New B,  Old A,  Old B.
    state_eL = state_eL | ( eL_B  << 3 );
    state_eL = state_eL | ( eL_A  << 2 );


    // Handle which transition we have registered.
    // Complete this if statement as necessary.
    // Refer to the labsheet. 
   if ( state_eL == 1 |  state_eL == 7 | state_eL == 8 | state_eL == 14 ) {
      count_eL -= 1;
    }
    else if (state_eL == 2 | state_eL == 4 | state_eL == 11 | state_eL == 13){
      count_eL += 1;
    } // Continue this if statement as necessary.
    else
      Serial.println("Encoder state change error");
    

    // Shift the current readings (bits 3 and 2) down
    // into position 1 and 0 (to become prior readings)
    // This bumps bits 1 and 0 off to the right, "deleting"
    // them for the next ISR call. 
    state_eL = state_eL >> 2;
}


/*
   This setup routine enables interrupts for
   encoder1.  The interrupt is automatically
   triggered when one of the encoder pin changes.
   This is really convenient!  It means we don't
   have to check the encoder manually.
*/
void setupEncoder0() 
{
    count_eR = 0;

    // Setup pins for right encoder 
    pinMode( ENCODER_0_A_PIN, INPUT );
    pinMode( ENCODER_0_B_PIN, INPUT );

    // initialise the recorded state of eR encoder.
    state_eR = 0;

    // Get initial state of encoder pins A + B
    boolean eR_A = digitalRead( ENCODER_0_A_PIN );
    boolean eR_B = digitalRead( ENCODER_0_B_PIN );
    eR_A = eR_A ^ eR_B;

    // Shift values into correct place in state.
    // Bits 1 and 0  are prior states.
    state_eR = state_eR | ( eR_B << 1 );
    state_eR = state_eR | ( eR_A << 0 );


    // Now to set up PE6 as an external interupt (INT6), which means it can
    // have its own dedicated ISR vector INT6_vector

    // Page 90, 11.1.3 External Interrupt Mask Register – EIMSK
    // Disable external interrupts for INT6 first
    // Set INT6 bit low, preserve other bits
    EIMSK = EIMSK & ~(1<<INT6);
    //EIMSK = EIMSK & B1011111; // Same as above.
  
    // Page 89, 11.1.2 External Interrupt Control Register B – EICRB
    // Used to set up INT6 interrupt
    EICRB |= ( 1 << ISC60 );  // using header file names, push 1 to bit ISC60
    //EICRB |= B00010000; // does same as above

    // Page 90, 11.1.4 External Interrupt Flag Register – EIFR
    // Setting a 1 in bit 6 (INTF6) clears the interrupt flag.
    EIFR |= ( 1 << INTF6 );
    //EIFR |= B01000000;  // same as above

    // Now that we have set INT6 interrupt up, we can enable
    // the interrupt to happen
    // Page 90, 11.1.3 External Interrupt Mask Register – EIMSK
    // Disable external interrupts for INT6 first
    // Set INT6 bit high, preserve other bits
    EIMSK |= ( 1 << INT6 );
    //EIMSK |= B01000000; // Same as above

}

void setupEncoder1() 
{

    count_eL = 0;

    // Setting up left encoder:
    // The Romi board uses the pin PE2 (port E, pin 2) which is
    // very unconventional.  It doesn't have a standard
    // arduino alias (like d6, or a5, for example).
    // We set it up here with direct register access
    // Writing a 0 to a DDR sets as input
    // DDRE = Data Direction Register (Port)E
    // We want pin PE2, which means bit 2 (counting from 0)
    // PE Register bits [ 7  6  5  4  3  2  1  0 ]
    // Binary mask      [ 1  1  1  1  1  0  1  1 ]
    //    
    // By performing an & here, the 0 sets low, all 1's preserve
    // any previous state.
    DDRE = DDRE & ~(1<<DDE6);
    //DDRE = DDRE & B11111011; // Same as above. 

    // We need to enable the pull up resistor for the pin
    // To do this, once a pin is set to input (as above)
    // You write a 1 to the bit in the output register
    PORTE = PORTE | (1 << PORTE2 );
    //PORTE = PORTE | 0B00000100;

    // Encoder0 uses conventional pin 26
    pinMode( ENCODER_1_A_PIN, INPUT );
    digitalWrite( ENCODER_1_A_PIN, HIGH ); // Encoder 1 xor

    // initialise the recorded state of eL encoder.
    state_eL = 0;
    
    // Get initial state of encoder.
    boolean eL_B = PINE & (1<<PINE2);
    //boolean eL_B = PINE & B00000100;  // Does same as above.

    // Standard read from the other pin.
    boolean eL_A = digitalRead( ENCODER_1_A_PIN ); // 26 the same as A8

    // Some clever electronics combines the
    // signals and this XOR restores the 
    // true value.
    eL_A = eL_A ^ eL_B;

    // Shift values into correct place in state.
    // Bits 1 and 0  are prior states.
    state_eL = state_eL | ( eL_B << 1 );
    state_eL = state_eL | ( eL_A << 0 );

    // Enable pin-change interrupt on A8 (PB4) for encoder0, and disable other
    // pin-change interrupts.
    // Note, this register will normally create an interrupt a change to any pins
    // on the port, but we use PCMSK0 to set it only for PCINT4 which is A8 (PB4)
    // When we set these registers, the compiler will now look for a routine called
    // ISR( PCINT0_vect ) when it detects a change on the pin.  PCINT0 seems like a
    // mismatch to PCINT4, however there is only the one vector servicing a change
    // to all PCINT0->7 pins.
    // See Manual 11.1.5 Pin Change Interrupt Control Register - PCICR
    
    // Page 91, 11.1.5, Pin Change Interrupt Control Register 
    // Disable interrupt first
    PCICR = PCICR & ~( 1 << PCIE0 );
    // PCICR &= B11111110;  // Same as above
    
    // 11.1.7 Pin Change Mask Register 0 – PCMSK0
    PCMSK0 |= (1 << PCINT4);
    
    // Page 91, 11.1.6 Pin Change Interrupt Flag Register – PCIFR
    PCIFR |= (1 << PCIF0);  // Clear its interrupt flag by writing a 1.

    // Enable
    PCICR |= (1 << PCIE0);
}

#endif
