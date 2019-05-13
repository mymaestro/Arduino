#include <OnionLibrary.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

Onion* onionSetup;

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led0 = 13;
int led1 = 2;

// the setup routine runs once when you press reset:
void setup() {      
  onionSetup = new Onion;          
  
  // initialize the digital pin as an output.
  pinMode(led0, OUTPUT);     
  pinMode(led1, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led0, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led1, HIGH);
  delay(1000);               // wait for a second
  digitalWrite(led0, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led1, LOW);
  delay(1000);               // wait for a second
}
