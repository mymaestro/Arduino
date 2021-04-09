/*
  Display the current time on the TM1636
  Add buttons to set time and alarm
  Use RTC library instead of Wire

  Mode button: Change between clock, stopwatch, light (LDR), and temperature modes
*/

#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>
#include "Wire.h"
#include <TTSDisplay.h>
#include <TTSTemp.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
TTSTemp temp;
TTSDisplay rtcshield;
// RTC_DS1307 rtc;

// Buzzer and input buttons
#define BUZZER 6
#define DOWN 9
#define UP 10
#define MODE 11
#define BLUE 2
#define GREEN 3
#define RED1 4
#define RED2 5
#define TOTAL_PINS 19
#define DISPLAYTIME 0
#define DISPLAYSTOPWATCH 1
#define DISPLAYTEMP 2
#define DISPLAYLIGHT 3

int LDR = 0;
int DEGREES = 0;
float DEGREESF = 0;

int HOUR = 0;
int MINUT = 11;
int BLINKTIME = 1000;
float waiting = 0;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char *monthName[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

tmElements_t tm;

byte second_s;
byte minute_s;
byte hour_s;
byte dayOfTheWeek;
byte day_s;
byte month_s;
int OLDDISPLAYMODE;
uint16_t year_s;
bool blink = false;
bool debug = false;
volatile bool UP_PRESSED = false;
volatile bool DOWN_PRESSED = false;
volatile int DISPLAYMODE;
volatile uint8_t pinChangeInterruptFlag = 0;
volatile uint8_t pinState = 0;
volatile uint8_t buttonpressed[TOTAL_PINS] = {0}; // possible arduino pins

void setup()
{
  bool parse = false;
  bool config = false;
  while (!Serial);
  Serial.begin(115200);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  enableInterrupt(DOWN, isr_handler, CHANGE);
  enableInterrupt(UP, isr_handler, RISING);
  enableInterrupt(MODE, isr_handler, RISING);

  pinMode(BUZZER, OUTPUT);
  pinMode(RED1, OUTPUT);
  pinMode(RED2, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);

  setSyncProvider(RTC.get); // the function to get the time from the RTC
  if (timeStatus() != timeSet)
  {
    Serial.println("Unable to sync with the RTC");

    // get the date and time the compiler was run
    if (getDate(__DATE__) && getTime(__TIME__))
    {
      parse = true;
      // and configure the RTC with this info
      if (RTC.write(tm))
      {
        config = true;
      }
    }
    delay(200);
    if (parse && config)
    {
      Serial.print("DS1307 configured Time=");
      Serial.print(__TIME__);
      Serial.print(", Date=");
      Serial.println(__DATE__);
    }
    else if (parse)
    {
      Serial.println("DS1307 Communication Error :-{");
      Serial.println("Please check your circuitry");
    }
    else
    {
      Serial.print("Could not parse info from the compiler, Time=\"");
      Serial.print(__TIME__);
      Serial.print("\", Date=\"");
      Serial.print(__DATE__);
      Serial.println("\"");
    }
  }
  else
    Serial.println("RTC has set the system time");

  rtcshield.clear();
  rtcshield.pointOn();
  waiting = millis();
  DISPLAYMODE = DISPLAYSTOPWATCH;
  OLDDISPLAYMODE = DISPLAYTIME;
}

void isr_handler()
{
  pinChangeInterruptFlag = arduinoInterruptedPin;
  pinState = arduinoPinState;
  switch (pinChangeInterruptFlag)
  {
  case MODE:
    DISPLAYMODE++;
    DISPLAYMODE = (DISPLAYMODE++ % 4);
    break;
  case UP:
    UP_PRESSED = true;
    break;
  case DOWN:
    DOWN_PRESSED = true;
    break;
  }
  //  pinChangeInterruptFlag = 0;
}

void displayTime()
{
  while (DISPLAYMODE == DISPLAYTIME)
  {
    Serial.print("MODE: ");
    Serial.println(DISPLAYMODE);
    time_t now = now();
    hour_s = hour();
    minute_s = minute();
    second_s = second();
    year = year();
    month = month();
    dayOfTheWeek = dayOfTheWeek();
    day_s = day();

    rtcshield.time(hour_s, minute_s);

    if ((millis() - waiting) > BLINKTIME)
    {
      blink = !blink;
      waiting = millis();
    }
    (blink) ? rtcshield.pointOn() : rtcshield.pointOff();

    // Start RTC drift compensation
    // oops, this only works if the program is in the time display loop at 9:01 AM
    if (hour_s == 9 && minute_s == 1 && second_s == 0)
    {               // run daily at exactly 9:01 A.M.
      delay(80500); // wait 80 seconds
                    // rtc.adjust(time_t(yyyy, mm, dd, hh, MM, ss));
      rtc.adjust(time_t(year, month, day_s, 9, 1, 1));

      /* Time drift is approx 90 seconds gain per 24 hours. The extra one second is to prevent the program from being stuck
        in an endless loop. Formula is time drift <in seconds> + 1 second leeway + set time to 1 min, 1 second past the hour.
        This will result in the clock being put back 30 seconds at 9:01:01 every morning. Adjust formula accordingly.
        DS3231 crystal is temperature sensitive so during the summer the gain will slow down. Colder temps = faster speeds.
        Often cheap Chinese modules use watch crystals which are calibrated at blood temperature not room temperature.
        The above code is not perfect but will help to compensate for this problem. */

    } // end RTC drift compensation

    // Send it to the serial monitor
    if (debug == true)
    {
      Serial.print(hour_s, DEC);
      Serial.print(":");
      if (minute_s < 10)
        Serial.print("0");
      Serial.print(minute_s, DEC);
      Serial.print(":");
      if (second_s < 10)
        Serial.print("0");
      Serial.print(second_s, DEC);
      Serial.print(" ");
      Serial.print(month, DEC);
      Serial.print("-");
      Serial.print(day_s, DEC);
      Serial.print("-");
      Serial.print(year);
      Serial.print(" (");
      switch (dayOfTheWeek)
      {
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
    delay(500);
  }
}

void displayStopwatch()
{
  time_t start = rtc.now();
  rtcshield.pointOn();
  while (DISPLAYMODE == DISPLAYSTOPWATCH)
  {
    Serial.print("MODE: ");
    Serial.println(DISPLAYMODE);
    time_t now = rtc.now();
    TimeSpan elapsed = now - start;
    minute_s = elapsed.minutes();
    second_s = elapsed.seconds();
    rtcshield.time(minute_s, second_s);
    if (UP_PRESSED)
    { // pause
      UP_PRESSED = false;
      while (!UP_PRESSED)
      {
        delay(1000);
      }
      start = now - elapsed; // this doesn't exactly work
      UP_PRESSED = false;
    }
    if (DOWN_PRESSED)
    {
      start = rtc.now();
      DOWN_PRESSED = false;
    }
    // ( blink ) ? rtcshield.pointOn() : rtcshield.pointOff();
    delay(1000);
  }
}

// Sound the buzzer
void beep()
{
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  delay(500);
}

// Display the light sensor

void displayLDR()
{
  while (DISPLAYMODE == DISPLAYLIGHT)
  {
    Serial.print("MODE: ");
    Serial.println(DISPLAYMODE);

    LDR = analogRead(A1); // show value on display
    rtcshield.num(LDR);
    delay(250);
  }
}

// display the temperature

void displayTemperature()
{
  while (DISPLAYMODE == DISPLAYTEMP)
  {
    Serial.print("MODE: ");
    Serial.println(DISPLAYMODE);

    DEGREES = temp.get();
    DEGREESF = DEGREES * 1.8 + 32;
    rtcshield.num(DEGREESF);
    delay(250);
  }
}

void loop()
{
  switch (DISPLAYMODE)
  {
  case DISPLAYTIME:
    Serial.println("Time display mode:");
    if (DISPLAYMODE != OLDDISPLAYMODE)
    {
      digitalWrite(GREEN, LOW);
      digitalWrite(RED1, HIGH);
      digitalWrite(RED2, LOW);
      digitalWrite(BLUE, LOW);
      OLDDISPLAYMODE = DISPLAYMODE;
    }
    displayTime();
    break;
  case DISPLAYSTOPWATCH:
    Serial.println("Stopwatch:");
    if (DISPLAYMODE != OLDDISPLAYMODE)
    {
      digitalWrite(GREEN, LOW);
      digitalWrite(RED1, LOW);
      digitalWrite(RED2, HIGH);
      digitalWrite(BLUE, LOW);
      OLDDISPLAYMODE = DISPLAYMODE;
    }
    displayStopwatch();
    break;
  case DISPLAYTEMP:
    Serial.println("Temperature:");
    if (DISPLAYMODE != OLDDISPLAYMODE)
    {
      digitalWrite(GREEN, LOW);
      digitalWrite(RED1, LOW);
      digitalWrite(RED2, LOW);
      digitalWrite(BLUE, HIGH);
      OLDDISPLAYMODE = DISPLAYMODE;
    }
    displayTemperature();
    break;
  case DISPLAYLIGHT:
    Serial.println("Lumens:");
    if (DISPLAYMODE != OLDDISPLAYMODE)
    {
      digitalWrite(GREEN, HIGH);
      digitalWrite(RED1, LOW);
      digitalWrite(RED2, LOW);
      digitalWrite(BLUE, LOW);
      OLDDISPLAYMODE = DISPLAYMODE;
    }
    displayLDR();
    break;
  }
  /*
  if (pinChangeInterruptFlag) {
    switch (pinChangeInterruptFlag) {
      case MODE:
        DISPLAYMODE++;
        DISPLAYMODE = (DISPLAYMODE % 4);
        break;
      case UP:
        break;
      case DOWN:
        break;
    }
    pinChangeInterruptFlag = 0;
  } */
}
