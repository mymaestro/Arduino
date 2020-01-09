/*
  Display the current time on the TM1636
  Add buttons to set time and alarm
  Use RTC library instead of Wire
*/

#include "Wire.h"
#include <TTSDisplay.h>
#include <RTClib.h>
#include <EEPROM.h>
TTSDisplay rtcshield;
RTC_DS1307 rtc;

// Buzzer and input buttons
#define BUZZER 6
#define DOWN   9
#define UP     10
#define MODE   11
#define BLUE   2
#define GREEN  3
#define RED1   4
#define RED2   5

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool blink = false;
bool debug = false;

void setup() {
  // Wire.begin();
  while (!Serial);
  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while(1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    rtc.adjust(DateTime(__DATE__,__TIME__));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call: yyyy, mm, dd, hh, MM, ss
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  rtcshield.clear();
  rtcshield.pointOn();

  pinMode(DOWN,   INPUT);
  pinMode(UP,     INPUT);
  pinMode(MODE,   INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED1,   OUTPUT);
  pinMode(RED2,   OUTPUT);
  pinMode(BLUE,   OUTPUT);
  pinMode(GREEN,  OUTPUT);
}
 
void loop()
{
  DateTime now = rtc.now();
  rtcshield.time(now.hour(), now.minute());
  if (!blink) { // Blink the colon separator
    rtcshield.pointOn();
  } else {
    rtcshield.pointOff();
  }
  if(digitalRead(MODE) == 0) { // Mode button pressed; set Alarm time
    delay(2000);
    rtcshield.time(12, 00);
    time();
    delay(2000);
  }
   
  match(); // Medication timer
  delay(200);
  blink = !blink;
}

/* Set alarm time and feed time into Internal eeprom*/

void time() {
  int temp=1, minuts=0, hours=0, seconds=0;
  while (temp==1) {
    if (digitalRead(UP)==0) {
      HOUR++;
      if (HOUR==24) HOUR=0;
      while(digitalRead(UP)==0);
    }
    rtcshield.clear();
    rtcshield.time(HOUR, MINUT);
    delay(100);
    if (digitalRead(DOWN)==0) {
      hours1=HOUR;
      EEPROM.write(add++, hours1);
      temp=2;
      while(digitalRead(DOWN)==0);
    }
  }
  while (temp==2) {
    if (digitalRead(UP)==0) {
      MINUT++;
      if (MINUT==60) MINUT=0;
      while (digitalRead(UP)==0);
    }
    //rtcshield.clear();
    rtcshield.time(HOUR, MINUT);
    delay(100);
    if (digitalRead(next)==0) {
      minut=MINUT;
      EEPROM.write(add++, minut);
      temp=0;
      while(digitalRead(next)==0);
    }
  }
  delay(1000);
}

/* Function to chack medication time */

void match() {
  int tem[17];
  for (int i=11; i<17; i++) {
    tem[i]=EEPROM.read(i);
  }
  if(HOUR == tem[11] && MINUT == tem[12]) {
    beep();
    beep();
    beep();
    beep();
    beep();
    beep();
    beep();
    beep();
  }
}

/* function to buzzer indication */

void beep() {
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
}
