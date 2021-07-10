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

/*-------------------------------------------------------------------*/
/*--------------------------Includes---------------------------------*/
/*-------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/interrupt.h>

/*-------------------------------------------------------------------*/
/*--------------------------Settings---------------------------------*/
/*-------------------------------------------------------------------*/

/* defined via PlatformIO environment build flags
#define DIVIDER_RATIO               1           // Divider ratio between input pulses and output pulses
#define OUTPUT_PULSE_LENGTH_mS      30          // Output pulse length in milliseconds, max 500ms
#define OUTPUT_CONFIG               1           // Inverted output if LOW, normal output if HIGH
#define RISING_EDGE_INPUT                       // Rising edge input if set
*/
// Attiny24a Pins
#define PIN_INPUT                   2           // PORTPIN of port B for input pulse
#define PIN_OUTPUT                  2           // PORTPIN of port A for output pulse


/*-------------------------------------------------------------------*/
/*-----------------------------Vars----------------------------------*/
/*-------------------------------------------------------------------*/

volatile uint8_t  EdgesFound;                   // Number of edges found since last flush (EdgesFound >= DIVIDER_RATIO)

bool              testo;

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
//DDRB  = (0 << PIN_INPUT);   // 0 = Input, not required to set
//PORTB = (1 << PIN_INPUT);   // Pullup, optional, not needed in my case

  //--------Timer settings---------
  /*
  F_Timer   = F_CPU / Prescaler
  F_Timer   = 8 MHz / 64      = 125 KHz
  T_Tick    = 1 s / 125 KHz   = 8 µs
  T_Max     = 8 µs X 65536   = 0.524 288 s

  Example OCR1A value:
  30ms / 8 µs = 3750 ticks
  */

  TCCR1B |= (1 << CS10) | (1 << CS11);        // set clock source /64 prescaler, starting timer
  OCR1A   = OUTPUT_PULSE_LENGTH_mS / 0.008;   // set compare window to number of timer ticks required to hit OUTPUT_PULSE_LENGTH_mS

  //--------Interrupt attach---------
  GIMSK = (1 << INT0);                  // enable INT0 mask
  MCUCR = (1 << ISC00) | (1 << ISC01);  // enable INT0 rising edge detection
  sei();

/*-------------------------------------------------------------------*/
/*----------------------------while(1)-------------------------------*/
/*-------------------------------------------------------------------*/
  while(1){
    // if enough pulses are found when timer flag is HIGH (Timer not already running)
    if (/*(TIFR1 & (1 << OCF1A)) && */(EdgesFound >= DIVIDER_RATIO)) {
      EdgesFound      = 0;
      testo != testo;
      //TCNT1           = 0;    // reset timer value
      //TIFR1 |= (1 << OCF1A);  // clear timer elapsed flag and therefore set S0 output
    }

    //Output TimerElapsedFlag directly. Will generate a single pulse at bootup until timer is elapsed once,
    //but as long as the bit stays HIGH until we clear it manually, we should only output a pulse while timer is running and the flag is low
    //PORTA = ((TIFR1 & (1<<OCF1A)) << PIN_OUTPUT);
    PORTA = (testo << PIN_OUTPUT);
  }// end while(1)
}// end main