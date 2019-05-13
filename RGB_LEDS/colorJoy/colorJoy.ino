#include <color.h>

// Joystick connected to A2 and A3
// The angle represents the hue (0-360), with red being 0 degrees, green 120 and blue 240.
// Saturation is distance from the center, with 0 (the center) being white and 1 (on the circumference) being fully saturated
// Lightness is fixed

#define REDPIN 9
#define GREENPIN 11
#define BLUEPIN 10
#define BUTTON 12
#define XJOY 3
#define YJOY 2
int diameter = 1024; // hypotenuse of 1024x1024
int centerY = 515; // Calibrate to joystick
int centerX = 525; // Calibrate
int radius = diameter - centerY;
int leds[] = {REDPIN, GREENPIN, BLUEPIN};
float lightness = 0.5;
Color cur_color = Color(1, 1, 1);
const unsigned int BAUD_RATE = 9600;

#define FADESPEED 5 // make this higher to slow down
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(YJOY, INPUT);
  pinMode(XJOY, INPUT);
  pinMode(BUTTON, INPUT);
  Serial.begin(BAUD_RATE);
}
void loop() {
  int y, x, r, g, b;
  y = analogRead(YJOY);
  x = analogRead(XJOY);

  float dx = abs(x - centerX);
  float dy = abs(y - centerY);
  float angle = atan(dy / dx) / PI * 180;
  float dist = pow((pow(dx, 2) + (pow(dy, 2))), 0.5);
  float saturation = dist / radius;
  if ( saturation > 1 ) saturation = 1;
  if ( saturation < 0 ) saturation = 0;  
  if ( dist < 2.4 ) saturation = 0; // center
  if (x < centerX) angle = 180 - angle;
  if (y > centerY) angle = 360 - angle;
  float hue = angle / 360;

  /*
  Serial.print("x(");
  Serial.print(x);
  Serial.print("), y(");
  Serial.print(y);
  Serial.print(");");
  
  if (digitalRead(BUTTON) == LOW) {
    Serial.print(" Button Up");
  } else {
    Serial.print(" Button Down");
  }
  Serial.print(" Angle: ");
  Serial.print(angle);
  Serial.print(", distance: ");
  Serial.print(dist);
  Serial.print(", Saturation: ");
  Serial.print(saturation);
  Serial.print(", Hue: ");
  Serial.print(hue);
  */
  
  cur_color.convert_hcl_to_rgb(hue, saturation, lightness);
  display_color(cur_color);
}

void display_color(Color c){
  /*
  Serial.print(" Colors: Red=");
  Serial.print(c.red);
  Serial.print(", Blue=");
  Serial.print(c.blue);
  Serial.print(", Green=");
  Serial.print(c.green);
  Serial.println(";");
  */
  analogWrite(leds[0], c.red);
  analogWrite(leds[1], c.green);
  analogWrite(leds[2], c.blue);
  delay(100);
  
}
