// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
#define REDPIN 9
#define GREENPIN 11
#define BLUEPIN 10
#define XJOY 3

int r = 0;
int  g  = 0;
int b = 0;
int blinkSpeed = 500;

void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(XJOY, INPUT);
  randomSeed(analogRead(0));
}
void loop() {
  
  blinkSpeed = analogRead(XJOY);
  
  r = random(255);
  b = random(255);
  g = random(255);

  analogWrite(REDPIN, r);
  analogWrite(GREENPIN, g);
  analogWrite(BLUEPIN, b);
  delay(blinkSpeed);

}
