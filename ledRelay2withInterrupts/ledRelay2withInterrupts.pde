#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>

// Definitions for the Interrupt library
#define NO_PORTC_PINCHANGES
#define NO_PORTD_PINCHANGES  // Only using Arduino pin 12, PORTB
#define PIN_COUNT 1          // Only using one input pin
#define MAX_PIN_CHANGE_PINS PIN_COUNT

#define BUTTON_PIN 12       // Which pin is attached to the button
#define BAUD_RATE 9600      // Serial port baud rate
#define STATUS_LED_PIN 13   // select the pin for the status LED

// Global variables
int state = 255;            // the current status (changes with button press)
int delayVal = 50;          // variable to store the delay betweeen blinks
int chaseVal = 6;           // variable to store how many leds are off
int ledArraySize = 8;       // how many leds in the set
boolean forward = true;     // will the chase be forward or backwards
int turningON = 2; // Holders for the light chaser loop
int turningOFF = 2;
unsigned long started;
unsigned long finished;

byte ledPin[] = { 2, 3, 4, 5, 6, 7, 8, 9 };

void setup() {
  Serial.begin(BAUD_RATE);  // Set up the serial port monitor
  Serial.println("Chaser array");
  Serial.print("Putting an interrupt on button pin ");
  Serial.println(BUTTON_PIN);
  
  ledArraySize = sizeof(ledPin);

  pinMode(BUTTON_PIN, INPUT);  // Setup the pins
  for ( int p = 0; p < ledArraySize; p++ ) {
    pinMode (ledPin[p], OUTPUT);
  }
  pinMode(STATUS_LED_PIN, OUTPUT);  // declare the STATUS_LED_PIN as an OUTPUT
  
  PCintPort::attachInterrupt(BUTTON_PIN, fall, FALLING); // attach a PinChange Interrupt to the button
}

void loop() {
  for ( int c = 0; c < ledArraySize; c++ ) {
    switch (forward) {
      case true:
        Serial.print("F(");
        turningON = c % ledArraySize;
        turningOFF = (c + chaseVal) % ledArraySize;
        break;
      case false:
        Serial.print("R(");
        turningON = ledArraySize - (c % ledArraySize) - 1;
        turningOFF = ledArraySize - ((c  + chaseVal) % ledArraySize) - 1 ;
        break;
    }
    Serial.print(c);
    Serial.print("): Turn LED connected to pin ");
    Serial.print(turningON);
    Serial.print(" on, turn LED connected to pin ");
    Serial.print(turningOFF);
    Serial.print(" off, then delay for ");
    Serial.print(delayVal);
    Serial.println("ms.");
    digitalWrite(ledPin[turningON], HIGH);
    digitalWrite(ledPin[turningOFF], LOW);
    switch (state)
    {
        case RISING: //we have just seen a rising edge
            PCintPort::detachInterrupt(BUTTON_PIN);
            PCintPort::attachInterrupt(BUTTON_PIN, fall, FALLING); //attach the falling end
            state=255;
            //delayVal = finished - started;
            break;
        case FALLING: //we just saw a falling edge
            PCintPort::detachInterrupt(BUTTON_PIN);
            PCintPort::attachInterrupt(BUTTON_PIN, rise, RISING);
            forward = !forward;
            state=255;
            break;
        /*default:
            //do nothing
            break;*/
    }
    delay(delayVal);
  }
}

void rise() {
  Serial.println("                               ");
  Serial.print("Interrupt rise, button up! ");
  state = RISING;
  finished = millis();
  Serial.println(finished);
  digitalWrite(STATUS_LED_PIN, LOW);
}
void fall() {
  Serial.println("                               ");
  Serial.print("Interrupt fall, button down! ");
  state = FALLING;
  started = millis();
  Serial.println(started);
  digitalWrite(STATUS_LED_PIN, HIGH);
}

