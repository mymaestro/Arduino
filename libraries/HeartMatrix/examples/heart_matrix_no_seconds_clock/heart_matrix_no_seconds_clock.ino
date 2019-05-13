/* LucidTronix Heart Matrix
 * LED Display and rtc clock based on ds1307 chip
 * For instructions, details and schematic, See:
 * http://www.lucidtronix.com/tutorials/36
 * Uses the RTClib.h library from adafruit
 */
#include <Wire.h>
#include "RTClib.h"
#include <FrequencyTimer2.h>
#include <HeartMatrix.h>

// dataPin is on 5, Latch is on 6, and clock is on 7
HeartMatrix hm = HeartMatrix(5,6,7);
void displayer2(){  
  hm.displayer();
}

int mode = 0;
int num_modes = 4;
int cur_sec = 0;
int cur_min = 0;
int millisecond_six = 0;
unsigned int last_sec = 0;

unsigned int last_press = 0;

RTC_DS1307 RTC;
DateTime cur_time, alarm_time;
void setup(){
  
  Serial.begin(57600);


  delay(1000);
  Wire.begin(); // Start the I2C interface.
  RTC.begin();

  if (! RTC.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  delay(500);
  FrequencyTimer2::disable();
   // Set refresh rate (interrupt timeout period)
   // if things break try making this number bigger
   FrequencyTimer2::setPeriod(2000);
    // Set interrupt routine to be called
   FrequencyTimer2::setOnOverflow(displayer2); 
   hm.set_message(" ");
}
String stime = "";
unsigned int last_update = 0;
void loop(){
  hm.on();  
  if (millis() - last_update > 1000){
    stime = "";
    cur_time = RTC.now();
    if (cur_time.hour() == 0 || cur_time.hour() == 12 ) stime += String(12); 
    else if (cur_time.hour()%12 < 10 ) stime += String(cur_time.hour()%12);  
    else stime += String(cur_time.hour()%12);  
    stime += ":";
    if (cur_time.minute() < 10 ) stime += String(0);
    stime += String(cur_time.minute());
    String ampm = "am  ";
    if (cur_time.hour() > 12) ampm = "pm  ";
    hm.set_message(stime+ampm );
  }
}

