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
#warning TODO: remove debugging boards ESP and UNO as only the Attiny will be used

/*-------------------------------------------------------------------*/
/*------------------------Project defines----------------------------*/
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*--------------------------Includes---------------------------------*/
/*-------------------------------------------------------------------*/

#include <Arduino.h>

/*-------------------------------------------------------------------*/
/*--------------------------Settings---------------------------------*/
/*-------------------------------------------------------------------*/

#define OUTPUT_PULSE_LENGTH_uS      30000       // Output pulse length in microseconds

// Attiny24a Pins
#define PIN_INPUT                   2           // PORTPIN of port B for input pulse
#define PIN_OUTPUT                  2           // PORTPIN of port A for output pulse


/*-------------------------------------------------------------------*/
/*-----------------------------Vars----------------------------------*/
/*-------------------------------------------------------------------*/

volatile uint8_t  EdgesFound;                   // Number of edges found since last flush (EdgesFound >= DIVIDER_RATIO)
unsigned long     PulseStartTime = 0;           // microseconds at which output pulse was started
bool              GoPulse;                      // set when enough edges were found, starts output pulse time

/*-------------------------------------------------------------------*/
/*------------------------------ISR----------------------------------*/
/*-------------------------------------------------------------------*/

void OutISR() {
  EdgesFound += 1;
}

/*-------------------------------------------------------------------*/
/*-----------------------------Init----------------------------------*/
/*-------------------------------------------------------------------*/
void setup() {

  // PORT A
  DDRA = (1 << PIN_OUTPUT); // 1 = Output
  // PORT B
  DDRB =  (0<< PIN_INPUT);  // 0 = Input
  PORTB = (1<< PIN_INPUT);  // Pullup

  // interrupt attach
  attachInterrupt(0, OutISR, RISING);
}


/*-------------------------------------------------------------------*/
/*-----------------------------while(1)----------------------------------*/
/*-------------------------------------------------------------------*/
void loop() {
  // if enough pulses are found when pulse flag is low
  if (!GoPulse && (EdgesFound >= DIVIDER_RATIO)) {
    // reset counter, start pulse flag
    EdgesFound      = 0;
    GoPulse         = HIGH;
    PulseStartTime  = micros();
  }

  // if pulse flag is high
  if (GoPulse) {
    // reset once time is elapsed
    if (micros() >= PulseStartTime + OUTPUT_PULSE_LENGTH_uS) {
      GoPulse = LOW;
    }
  }

  // output pulse flag
  PORTA = (!GoPulse << PIN_OUTPUT);
}// end loop
