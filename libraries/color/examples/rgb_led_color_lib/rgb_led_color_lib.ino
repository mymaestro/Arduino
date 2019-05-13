#include <color.h>

// these are the pins connected to
// the RGB LEDs
int leds[] = {5,6,9}; 
Color cur_color = Color(1,1,1);
float hue = 0;

// run once, when the sketch starts
void setup()                    
{
  for(int i = 0 ; i < 3; i++ ){
    pinMode(leds[i], OUTPUT); 
  }
}

// run over and over again
void loop()                   
{
  display_color(cur_color);
  rainbow();
}

void rainbow(){
  hue += 0.06;
  if ( hue >=1 ) hue = 0;
  sat = 1.0;
  val = 0.4;
  cur_color.convert_hcl_to_rgb(hue,sat,val);
  display_color(cur_color);
  delay(20);
}

void display_color(Color c){
  analogWrite(leds[0], c.red);
  analogWrite(leds[1], c.green);
  analogWrite(leds[2], c.blue);
  delay(100);
}
