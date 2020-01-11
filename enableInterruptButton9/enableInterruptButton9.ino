/*
 *  This example shows how to combine the EnableInterrupt
 *  library with some simple debouncing code for reading
 *  button presses consistently.
 *  
 *  The interrupt is attached to digital input 5 and the
 *  service routine just toggles the onboard LED status.
 *  
 *  Tested on: ATmega328P
 *  
 *  Example by Lorenzo Cafaro <lorenzo@ibisco.net>
 *
*/
#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>

#define BUTTON_PIN 9
#define  DOWN   9
#define  UP     10
#define  MODE   11
#define  BLUE   2
#define  GREEN  3
#define  RED1   4
#define  RED2   5
#define  TOTAL_PINS 19

#define DEBOUNCE_DELAY 100 // in ms

volatile uint8_t pinChangeInterruptFlag=0;
volatile uint8_t pinState=0;
volatile uint8_t buttonpressed[TOTAL_PINS]={0}; // possible arduino pins

void setup() {
  Serial.begin(115200);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED1, OUTPUT);
  enableInterrupt(DOWN, isr_handler, CHANGE);
  enableInterrupt(UP, isr_handler, CHANGE);
  enableInterrupt(MODE, isr_handler, CHANGE);
}

void loop() {
  if (pinChangeInterruptFlag) {
    switch (pinChangeInterruptFlag) {
      case MODE:
        ( pinState == 0 ) ? digitalWrite(RED1, HIGH) : digitalWrite(RED1, LOW);
        break;
      case UP:
        ( pinState == 0 ) ? digitalWrite(GREEN, HIGH) : digitalWrite(GREEN, LOW);
        break;
      case DOWN:
        ( pinState == 0 ) ? digitalWrite(BLUE, HIGH) : digitalWrite(BLUE, LOW);
        break;
    }
    pinChangeInterruptFlag = 0;
  }
}
void isr_handler() {
  pinChangeInterruptFlag=arduinoInterruptedPin;
  pinState=arduinoPinState;
}
