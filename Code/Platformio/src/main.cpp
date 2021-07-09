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
#ifndef BOARD_TINY
  #define DEBUG                                 // enabled serial prints, attiny environment does not have serial out
#endif

/*-------------------------------------------------------------------*/
/*--------------------------Includes---------------------------------*/
/*-------------------------------------------------------------------*/

#include <Arduino.h>
#ifdef BOARD_ESP
  #include <ESP8266WiFi.h>                      // to turn off WiFi for ESP chip
#endif

/*-------------------------------------------------------------------*/
/*--------------------------Settings---------------------------------*/
/*-------------------------------------------------------------------*/

// only if DEBUG mode is on, prints get compiled into Serial.print
#ifdef DEBUG
  #define PRINT(...)      Serial.print(__VA_ARGS__)
  #define PRINTLN(...)    Serial.println(__VA_ARGS__)
#else
  #define PRINT(...)
  #define PRINTLN(...)
#endif

#define SERIAL_BAUD                 115200      // Serial monitor baud rate
#ifndef DIVIDER_RATIO
  #define DIVIDER_RATIO             10          // Number of edges to detect until new edge is output 
#endif
#define OUTPUT_PULSE_LENGTH_uS      30000       // Output pulse length in microseconds

// ESP-Hiro ESP8266 Pins
#ifdef BOARD_ESP
  #define PIN_INPUT                 13          // GPIO for input pulse
  #define PIN_OUTPUT                12          // GPIO for output pulse
#endif

// Atmega328 Pins
#ifdef BOARD_UNO
  #define PIN_INPUT                 2           // GPIO for input pulse, PB0, PCINT0
  #define PIN_OUTPUT                4           // PORTNUM for output pulse, PB4, D12
  #define PORT_OUTPUT               PORTB       // PORT for output pulse

#endif

// Attiny24a Pins
#ifdef BOARD_TINY
  #define PIN_INPUT                   2         // PORTPIN of port B for input pulse
  #define PIN_OUTPUT                  2         // PORTPIN of port A for output pulse
#endif

/*-------------------------------------------------------------------*/
/*-----------------------------Vars----------------------------------*/
/*-------------------------------------------------------------------*/

volatile uint8_t  EdgesFound;                   // Number of edges found since last flush (EdgesFound >= DIVIDER_RATIO)
unsigned long     PulseStartTime = 0;           // microseconds at which output pulse was started
bool              GoPulse;                      // set when enough edges were found, starts output pulse time

/*-------------------------------------------------------------------*/
/*------------------------------ISR----------------------------------*/
/*-------------------------------------------------------------------*/
#ifdef BOARD_ESP
  void IRAM_ATTR OutISR() {
    EdgesFound += 1;
    PRINTLN("Edge found");
  }
#endif
#ifdef BOARD_UNO
  void OutISR() {
    EdgesFound += 1;
    PRINTLN("Edge found");
  }
#endif
#ifdef BOARD_TINY
  void OutISR() {
    EdgesFound += 1;
  }
#endif


/*-------------------------------------------------------------------*/
/*-----------------------------Init----------------------------------*/
/*-------------------------------------------------------------------*/
void setup() {

  #ifdef DEBUG
    Serial.begin(SERIAL_BAUD);
  #endif

  PRINTLN("Hello!");

  #ifdef BOARD_ESP
    PRINTLN("ESP_BOARD set, disabling WiFi");
    WiFi.mode(WIFI_OFF);
  #endif

  // pin settings
  PRINTLN("Applying port settings");
  PRINT("Input: ");
  PRINTLN(PIN_INPUT);
  PRINT("Output: ");
  PRINTLN(PIN_OUTPUT);

  #ifdef BOARD_ESP
    pinMode(PIN_OUTPUT, OUTPUT);
    pinMode(PIN_INPUT, INPUT);
  #endif
  #ifdef BOARD_UNO
    pinMode(12, OUTPUT);
    pinMode(PIN_INPUT, INPUT_PULLUP);
  #endif
  #ifdef BOARD_TINY
    // PORT A
    DDRA = (1 << PIN_OUTPUT); // 1 = Output
    // PORT B
    DDRB =  (0<< PIN_INPUT);  // 0 = Input
    PORTB = (1<< PIN_INPUT);  // Pullup
  #endif


  // interrupt attach
  PRINT("Attaching interrupt to: ");
  PRINTLN(PIN_INPUT);
  #ifdef BOARD_ESP
    attachInterrupt(digitalPinToInterrupt(PIN_INPUT), OutISR, FALLING);
  #endif
  #ifdef BOARD_UNO
    attachInterrupt(digitalPinToInterrupt(PIN_INPUT), OutISR, FALLING);
  #endif
  #ifdef BOARD_TINY
    attachInterrupt(0, OutISR, RISING);
  #endif
}


/*-------------------------------------------------------------------*/
/*-----------------------------Main----------------------------------*/
/*-------------------------------------------------------------------*/
void loop() {
  // if enough pulses are found when pulse flag is low
  if (!GoPulse && (EdgesFound >= DIVIDER_RATIO)) {
    PRINT(DIVIDER_RATIO);
    PRINT(" Edges found, executing pulse of ");
    PRINT(OUTPUT_PULSE_LENGTH_uS);
    PRINTLN(" microseconds");
    // reset counter, start pulse flag
    EdgesFound      = 0;
    GoPulse         = HIGH;
    PulseStartTime  = micros();
  }

  // if pulse flag is high
  if (GoPulse) {
    // reset once time is elapsed
    if (micros() >= PulseStartTime + OUTPUT_PULSE_LENGTH_uS) {
      PRINT("Pulse finished with ");
      PRINT(micros() - PulseStartTime);
      PRINTLN(" microseconds");
      GoPulse = LOW;
    }
  }

  // output pulse flag
  #ifdef BOARD_ESP
    digitalWrite(PIN_OUTPUT, !GoPulse);  // ESPs digitalWrite is fast enough to not use ports directly
  #endif
  #ifdef BOARD_UNO
    PORT_OUTPUT = (GoPulse << PIN_OUTPUT);
  #endif
  #ifdef BOARD_TINY
    PORTA = (!GoPulse << PIN_OUTPUT);
  #endif

}// end loop

