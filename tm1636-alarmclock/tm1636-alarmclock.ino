/*
  Display the current time on the TM1636
  Add buttons to set time and alarm
  Use RTC library instead of Wire


  Hold the DOWN button to read and display the brightness
  Hold the UP button to read and display the temperature
*/

#include "Wire.h"
#include <TTSDisplay.h>
#include <TTSTemp.h>
#include <RTClib.h>
#include <EEPROM.h>
TTSTemp  temp;
TTSDisplay rtcshield;
RTC_DS1307 rtc;

// Buzzer and input buttons
#define  BUZZER 6
#define  DOWN   9
#define  UP     10
#define  MODE   11
#define  BLUE   2
#define  GREEN  3
#define  RED1   4
#define  RED2   5

int      LDR      = 0;
int      DEGREES  = 0;
float    DEGREESF = 0;

int      HOUR     = 0;
int      MINUT    = 11;

char     daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
byte     second;
byte     minute;
byte     hour;
byte     dayOfTheWeek;
byte     day;
byte     month;
uint16_t year;
bool     blink    = false;
bool     debug    = false;

void setup() {
  pinMode(DOWN,   INPUT_PULLUP);
  pinMode(UP,     INPUT_PULLUP);
  pinMode(MODE,   INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED1,   OUTPUT);
  pinMode(RED2,   OUTPUT);
  pinMode(BLUE,   OUTPUT);
  pinMode(GREEN,  OUTPUT);

  // Wire.begin();
  while (!Serial);
  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    /* The following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); */
    rtc.adjust(DateTime(__DATE__, __TIME__));
    /* This line sets the RTC with an explicit date & time, for example to set
    January 21, 2014 at 3am you would call: yyyy, mm, dd, hh, MM, ss
       rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));  */
  }
  rtcshield.clear();
  rtcshield.pointOn();
}

void displayTime() {
  DateTime now = rtc.now();
  hour =   now.hour();
  minute = now.minute();
  second = now.second();
  year =   now.year();
  month =  now.month();
  dayOfTheWeek = now.dayOfTheWeek();
  day =    now.day();

  rtcshield.time(hour, minute);
  
  ( blink ) ? rtcshield.pointOn() : rtcshield.pointOff();

  // Start RTC drift compensation
  if (hour == 9 && minute == 1 && second == 0) { // run daily at exactly 9:01 A.M.
    delay(90000); // wait 90 seconds
 // rtc.adjust(DateTime(yyyy, mm, dd, hh, MM, ss));
    rtc.adjust(DateTime(year, month, day, 9, 1, 1));

    /* Time drift is approx 90 seconds gain per 24 hours. The extra one second is to prevent the program from being stuck
       in an endless loop. Formula is time drift <in seconds> + 1 second leeway + set time to 1 min, 1 second past the hour.
       This will result in the clock being put back 30 seconds at 9:01:01 every morning. Adjust formula accordingly.
       DS3231 crystal is temperature sensitive so during the summer the gain will slow down. Colder temps = faster speeds.
       Often cheap Chinese modules use watch crystals which are calibrated at blood temperature not room temperature.
       The above code is not perfect but will help to compensate for this problem. */

  } // end RTC drift compensation

  // Send it to the serial monitor
  if (debug == true) {
    Serial.print(hour, DEC);   Serial.print(":");
    if (minute < 10) Serial.print("0");
    Serial.print(minute, DEC); Serial.print(":");
    if (second < 10) Serial.print("0");
    Serial.print(second, DEC); Serial.print(" ");
    Serial.print(month, DEC);  Serial.print("-");
    Serial.print(day, DEC);    Serial.print("-");
    Serial.print(year);        Serial.print(" (");
    switch (dayOfTheWeek) {
      case 1:
        Serial.print("Sunday");
        break;
      case 2:
        Serial.print("Monday");
        break;
      case 3:
        Serial.print("Tuesday");
        break;
      case 4:
        Serial.print("Wednesday");
        break;
      case 5:
        Serial.print("Thursday");
        break;
      case 6:
        Serial.print("Friday");
        break;
      case 7:
        Serial.print("Saturday");
        break;
    }
    Serial.println(")");
  }
}
void loop() {
  displayTime();

  if (digitalRead(DOWN) == LOW) {
    digitalWrite(RED1, HIGH);
    LDR = analogRead(A1); // show value on display
    rtcshield.num(LDR);
    delay(2500);
    digitalWrite(RED1, LOW);
  }

  if (digitalRead(UP) == LOW) {
    digitalWrite(BLUE, HIGH);
    DEGREES = temp.get();
    DEGREESF = DEGREES * 1.8 + 32;
    rtcshield.num(DEGREESF);
    delay(2500);
    digitalWrite(BLUE, LOW);
  }

  if (digitalRead(MODE) == LOW) {
    digitalWrite(GREEN, HIGH);
    delay(10);
    digitalWrite(GREEN, LOW);
  }
  delay(1000);
  blink = !blink;
}

/* Set alarm time and feed time into Internal eeprom*/

void time() {
  int temp = 1, minuts = 0, hours = 0, seconds = 0, addr = 0;
  while (temp == 1) {
    if (digitalRead(UP) == 0) {
      HOUR++;
      if (HOUR == 24) HOUR = 0;
      while (digitalRead(UP) == 0);
    }
    rtcshield.clear();
    rtcshield.time(HOUR, MINUT);
    delay(100);
    if (digitalRead(DOWN) == 0) {
      hours = HOUR;
      EEPROM.write(addr++, hours);
      temp = 2;
      while (digitalRead(DOWN) == 0);
    }
  }
  while (temp == 2) {
    if (digitalRead(UP) == 0) {
      MINUT++;
      if (MINUT == 60) MINUT = 0;
      while (digitalRead(UP) == 0);
    }
    //rtcshield.clear();
    rtcshield.time(HOUR, MINUT);
    delay(100);
    if (digitalRead(UP) == 0) {
      minuts = MINUT;
      EEPROM.write(addr++, minuts);
      temp = 0;
      while (digitalRead(UP) == 0);
    }
  }
  delay(1000);
}

/* Function to chack medication time */

void match() {
  int tem[17];
  for (int i = 11; i < 17; i++) {
    tem[i] = EEPROM.read(i);
  }
  if (HOUR == tem[11] && MINUT == tem[12]) {
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
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  delay(500);
}
