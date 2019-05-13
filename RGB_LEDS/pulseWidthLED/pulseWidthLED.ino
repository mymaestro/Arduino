/* 
 * Pulsating LEDs with Pulse Width Modulation 
 */  
      
int green   = 6;            // Digital pin 11 - Green LED  
int red     = 5;            // Digital pin 10 - Red LED  
int blue    = 3;             // Digital pin 9  - Blue LED  
int time    = 5;             // define delay element  
int pulsewidth;              // define pulsewidth (0-255)  
      
void setup() {  
      // None required for analogWrite!  
}  
      
void loop() {  
// slowly fade the LEDs to full brightness  
  for (pulsewidth=0; pulsewidth <= 255; pulsewidth++){  
    analogWrite(green, pulsewidth);  
    analogWrite(red, pulsewidth);  
    analogWrite(blue, pulsewidth);  
    delay(time);  
  }  
// slowly dim the LEDs  
  for (pulsewidth=255; pulsewidth >= 0; pulsewidth--){  
    analogWrite(green, pulsewidth);  
    analogWrite(red, pulsewidth);  
    analogWrite(blue, pulsewidth);  
    delay(time);  
  }  
}  
