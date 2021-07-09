/*--------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------Pulse Extender------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------------*/
/*  
 * Reads short input pulses <= 10ms and outputs longer output pulses of ~ 30ms.
 * Input pulses are counted and divided to account for lower maximum frequency of output pulses.
 * 
 * Used to convert 7ms long IR LED pulses from a EMH eHZ via an TCRT5000 read head to 30ms S0 pulses.
 * EMH eHZ delivers 10.000 imp/kWh, after conversion we get 1000 imp/kwh 
 */

#warning TODO: fully move away from arduino framework, only use bare C, more speed, more better

/*-------------------------------------------------------------------*/
/*--------------------------Includes---------------------------------*/
/*-------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/interrupt.h>

/*-------------------------------------------------------------------*/
/*--------------------------Settings---------------------------------*/
/*-------------------------------------------------------------------*/

#define OUTPUT_PULSE_LENGTH_mS      30          // Output pulse length in milliseconds

// Attiny24a Pins
#define PIN_INPUT                   2           // PORTPIN of port B for input pulse
#define PIN_OUTPUT                  2           // PORTPIN of port A for output pulse


/*-------------------------------------------------------------------*/
/*-----------------------------Vars----------------------------------*/
/*-------------------------------------------------------------------*/

volatile uint8_t  EdgesFound;                   // Number of edges found since last flush (EdgesFound >= DIVIDER_RATIO)
volatile uint32_t millis;                       // Milliseconds since bootup

uint32_t          PulseStartTime;               // Milliseconds at which output pulse was started
bool              GoPulse;                      // Set when enough edges were found, starts output pulse time

/*-------------------------------------------------------------------*/
/*------------------------------ISR----------------------------------*/
/*-------------------------------------------------------------------*/

//----------External interrupt 0----------
ISR (INT0_vect){
  EdgesFound++;
}


/*-------------------------------------------------------------------*/
/*-----------------------------Init----------------------------------*/
/*-------------------------------------------------------------------*/
int main() {
  
  //----------Port Settings----------
  // PORT A
  DDRA  = (1 << PIN_OUTPUT);  // 1 = Output
  // PORT B
  DDRB  = (0 << PIN_INPUT);   // 0 = Input
  PORTB = (1 << PIN_INPUT);   // Pullup

  //--------Interrupt attach---------
  GIMSK = (1 << INT0);                  // enable INT0 mask
  MCUCR = (1 << ISC00) | (1 << ISC01);  // enable INT0 rising edge detection
  sei();


/*-------------------------------------------------------------------*/
/*----------------------------while(1)-------------------------------*/
/*-------------------------------------------------------------------*/
  while(1){
    // if enough pulses are found when pulse flag is low
    if (!GoPulse && (EdgesFound >= DIVIDER_RATIO)) {
      // reset counter, start pulse flag
      EdgesFound      = 0;
      GoPulse         = 1;
      PulseStartTime  = millis;
    }

    // if pulse flag is high
    if (GoPulse) {
      // reset once time is elapsed or timer overflew, might be shorter than OUTPUT_PULSE_LENGTH_mS during overflow but doesn't matter. 
      // millis < PulseStartTime happens once every ~50 days only if overflow happens while GoPulse is high which is incredibly rare.
      if ((millis >= PulseStartTime + OUTPUT_PULSE_LENGTH_mS) ||
          (millis <  PulseStartTime)) {
        GoPulse = 0; 
      }
    }

    /*
      With timer:

      if (WhateverTimerElapsedFlag && (EdgesFound >= DIVIDER_RATIO)) {
        EdgesFound      = 0;
        
        TCNTwhatever              = 0   reset timer for new count
        WhateverTimerElapsedFlag  = 1   reset timer flag by writing logic 1 in register
      }

      Output TimerElapsedFlag directly. Will generate a single pulse at bootup until timer is elapsed once,
      but as long as the bit stays HIGH until we clear it manually, we should only output a pulse while timer is running and the flag is low
      PORTA = (WhateverTimerElapsedFlag << PIN_OUTPUT);

    */

    // output pulse flag
    PORTA = (!GoPulse << PIN_OUTPUT);
  }// end while(1)
}// end main