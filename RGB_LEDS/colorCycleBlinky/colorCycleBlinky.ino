// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
#define REDPIN 9
#define GREENPIN 11
#define BLUEPIN 10
#define XJOY 3
#define FADESPEED 5 // make this higher to slow down
int r = 0;
int  g  = 0;
int b = 0;
int blinkSpeed = 500;

void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(XJOY, INPUT);
}
void loop() {
  
  blinkSpeed = analogRead(XJOY);
   // fade from blue to violet
  for (r = 0; r < 256; r++) {
   blinkSpeed = analogRead(XJOY);
   blinkOn(r, g, b, blinkSpeed);
  }
  // fade from violet to red
  for (b = 255; b > 0; b--) {
    blinkSpeed = analogRead(XJOY);
    blinkOn(r, g, b, blinkSpeed);
  }
  // fade from red to yellow
  for (g = 0; g < 256; g++) {
    blinkSpeed = analogRead(XJOY);
    blinkOn(r, g, b, blinkSpeed);
  }
  // fade from yellow to green
  for (r = 255; r > 0; r--) {
    blinkSpeed = analogRead(XJOY);
    blinkOn(r, g, b, blinkSpeed);
  }
  // fade from green to teal
  for (b = 0; b < 256; b++) {
    blinkSpeed = analogRead(XJOY);
    blinkOn(r, g, b, blinkSpeed);
  }
  // fade from teal to blue
  for (g = 255; g > 0; g--) {
    blinkSpeed = analogRead(XJOY);
    blinkOn(r, g, b, blinkSpeed);
  }
}
void blinkOn(int r, int g, int b, int blinkSpeed) {
  analogWrite(REDPIN, r);
  analogWrite(GREENPIN, g);
  analogWrite(BLUEPIN, b);
  delay(blinkSpeed / 2);
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);
  analogWrite(BLUEPIN, 0);
  delay(blinkSpeed / 2);
}
