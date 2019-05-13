#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <PinChangeInt.h>
// Uncomment one or more of the followinig to indicate that port will not be used for pin change interrupts
//#define NO_PORTB_PINCHANGES // Digital pins 8 to 13
#define NO_PORTC_PINCHANGES // Digital pins 0 to 7
//#define NO_PORTD_PINCHANGES // Analong pins 0 to 5

//Button pins
#define BUTTON1 A0
#define BUTTON2 A1

//ICM7218 connections
#define MODE_PIN 10
#define WRITE_PIN 11
#define DISPLAY_DIGITS 8
#define SCROLL_DELAY 240
#define DISPLAY_ENABLE 1<<4
#define DECODE_HEX 1<<6
#define NO_DECODE 1<<5
#define SEQUENTIAL_DATA 1<<7

// map ICM7218 pins to ID0, ID1, ID2, ID3, ID4, ID5, ID6, ID7
int ID_PIN[] = { 2, 3, 4, 5, 6, 7, 8, 9};

time_t thisTime; // unixtime
time_t thatTime; // time placeholder

volatile boolean button1pressed = false;
volatile boolean button2pressed = false;

// Display modes
#define DATETIME 0
#define IDLEMODE 1
#define STOPWATCHTIMER 2
#define PAUSE 3
#define LAPTIMER 4
int displayMode;

void setup() {
  Serial.begin(115200);

  // Setup RTC, pin 17 
  // A0-A5
  // 14-19
  pinMode(16, OUTPUT); // A2
  pinMode(17, OUTPUT); // A3 for power
  digitalWrite(16, LOW);
  digitalWrite(17, HIGH);

  while (!Serial) ; // wait until Arduino Serial Monitor opens
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  
  while (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
    Serial.println("The time has not been set.  Please run the Time");
    Serial.println("TimeRTCSet example, or DS1307RTC SetTime example.");
    Serial.println();
    delay(4000);
  }

  // Setup ICM7218
  pinMode(WRITE_PIN, OUTPUT);
  digitalWrite(WRITE_PIN, HIGH);
  pinMode(MODE_PIN, OUTPUT);
  for (int i=0; i<8; i++) {
    pinMode(ID_PIN[i], OUTPUT);
    digitalWrite(ID_PIN[i], HIGH);
  }

/*
  // Hardware change (using a Schmitt trigger to do hardware debouncing)
  // prompted changes to this portion - WG
  
  // Setup interrupts on button pins
  pinMode(BUTTON1, INPUT); digitalWrite(BUTTON1, HIGH);
  PCintPort::attachInterrupt(BUTTON1, &button1func, FALLING);  // add more attachInterrupt code as required
  pinMode(BUTTON2, INPUT); digitalWrite(BUTTON2, HIGH);
  PCintPort::attachInterrupt(BUTTON2, &button2func, FALLING);
*/
  pinMode(BUTTON1, INPUT);
  PCintPort::attachInterrupt(BUTTON1, &button1func, RISING);  // add more attachInterrupt code as required
  pinMode(BUTTON2, INPUT);
  PCintPort::attachInterrupt(BUTTON2, &button2func, RISING);

  displayMode = 0;
  thisTime = now();
}

void button1func() {
  button1pressed = true;
  Serial.print("Button 1, pin "); Serial.print(BUTTON1, DEC); Serial.println(" pressed.");
}

void button2func() {
  button2pressed = true;
  Serial.print("Button 2, pin "); Serial.print(BUTTON2, DEC); Serial.println(" pressed.");
}

/********************
dateTimeMode
   mm.dd
   hh:mm
   BUTTON1 ==> idleMode
   BUTTON2 ==> n/a
idleMode
   00:00
   00.00
   BUTTON1 ==> dateTimeMode
   BUTTON2 ==> stopWatchTimerMode (with zero)
stopWatchTimerMode (elapsed time)
   hh:mm
   ss.tt  (running)
   BUTTON1 ==> lapTimerMode
   BUTTON2 ==> pauseMode (with current elapsed time)
pauseMode
   hh:mm
   ss.tt  (blinking numbers)
   BUTTON1 ==> idleMode
   BUTTON2 ==> stopWatchTimerMode (with current elapsed time)
lapTimerMode
   mm:ss  (of last lap)
   mm:ss  (continuing timer)
   BUTTON1 ==> lapTimerMode
   BUTTON2 ==> pauseMode (with current elapsed time)
***********************/

void loop() {
  switch (displayMode) {
    case DATETIME:
      Serial.println("Date-time mode.");
      dateTimeMode();
      break;
    case IDLEMODE:
      Serial.println("Idle mode.");
      idleMode();
      break;
    case STOPWATCHTIMER:
      Serial.println("Timer mode.");
      thisTime = stopWatchTimerMode(thisTime);
      break;
    case PAUSE:
      Serial.println("Pause mode.");
      thisTime = pauseMode(thisTime);
      break;
    case LAPTIMER:
      Serial.println("Lap time mode.");
      thisTime = lapTimerMode(thisTime);
      break;
    default:
      Serial.println("Unknown mode.");
      break;
  }
}


/*
 * pauseMode flashes the elapsed time passed to it
 * blinking all 8 digits off and on
 */
time_t pauseMode(time_t pauseTime) {
  Serial.print("Entering pause mode for ");
  showTime(pauseTime);
  int ms = ((millis() / 10) % 100);
  while ((!button1pressed) && (!button2pressed)) {
      long dispTime = 
        ((long) hour(pauseTime)   * (long) 1000000) +
        ((long) minute(pauseTime) * (long) 10000)+
        ((long) second(pauseTime) * (long) 100) +
         (long) ms;
      writeTime(dispTime);
      delay(1000);
      sendCommand(0x00, 0, 0); // put the 7218 to sleep
      delay(1000);
      sendCommand(DISPLAY_ENABLE, 0, 0);
  }
  Serial.println("Exiting pause mode.");
  if (button1pressed) {
    button1pressed = false;
    button2pressed = false;
    displayMode = IDLEMODE;
    return(0);
  }
  if (button2pressed) {
    button1pressed = false;
    button2pressed = false;
    displayMode = STOPWATCHTIMER;
    return(pauseTime);
  }
}

time_t lapTimerMode(time_t elapsedStart) {
  Serial.print("Entering lap timer mode  ");
  Serial.print("starting at unixtime: ");

  time_t longStartTime = now();
  time_t elapsed = 0;
  
  showTime(longStartTime);
  
  while ((!button1pressed) && (!button2pressed)) {
    time_t timeNow = now();
    elapsed = timeNow - longStartTime;
    long dispTime =
      ((long) minute(elapsedStart) * (long) 1000000) +
      ((long) second(elapsedStart) * (long) 10000)+
      ((long) minute(elapsed) * (long) 100) +
       (long) second(elapsed);
    writeLapTime(dispTime);
  }
  Serial.println("Exiting lap timer mode.");
  if (button1pressed) {
    displayMode = LAPTIMER;
  }
  if (button2pressed) {
    displayMode = PAUSE;
  }
  button1pressed = false;
  button2pressed = false;
  return(elapsed);
}

time_t stopWatchTimerMode(time_t elapsedStart) {
  Serial.print("Entering timer mode ");
  Serial.print("starting at unixtime: ");

  time_t longStartTime = now();
  time_t elapsed = 0;
  showTime(longStartTime);
  
  while ((!button1pressed) && (!button2pressed)) {
    time_t timeNow = now();
    elapsed = (timeNow - longStartTime ) + elapsedStart;
    long dispTime = 
      ((long) hour(elapsed)   * (long) 1000000) +
      ((long) minute(elapsed) * (long) 10000)+
      ((long) second(elapsed) * (long) 100) +
       (long) ((millis() / 10) % 100);
    writeTime(dispTime);
  }
  Serial.println("Exiting stopwatch timer mode.");
  if (button1pressed) {
    displayMode = LAPTIMER;
  }
  if (button2pressed) {
    displayMode = PAUSE;
  }
  button1pressed = false;
  button2pressed = false;
  return(elapsed);
}

void dateTimeMode() {
  Serial.println("Date and time mode.");
  boolean blinking = true;
  while ((!button1pressed) && (!button2pressed)) {
    if (timeStatus() == timeSet) {
      time_t timeNow = now();
      showTime(timeNow);
      long dispTime = 
        ((long) month(timeNow) * (long) 1000000) +
        ((long) day(timeNow)   * (long) 10000)+
        ((long) hour(timeNow)  * (long) 100) +
         (long) minute(timeNow);
        Serial.print("DATETIME: ");
        Serial.println(dispTime);
        writeDateTime(dispTime, blinking);
    } // if timeStatus
    blinking = !blinking;
    delay(1000);
  } // while not button pressed
  Serial.println("Exiting DATETIME mode.");
  if (button1pressed) {
    button1pressed = false;
    button2pressed = false;
    displayMode = IDLEMODE;
  }
  if (button2pressed) {
    button1pressed = false;
    button2pressed = false;
    displayMode = DATETIME;
  }
}

void idleMode() {
  Serial.println("IDLE MODE 00:00:00.00");
  while ((!button1pressed) && (!button2pressed)) {
    thisTime = ((long) 0);
    writeTime(thisTime);
    delay(1000);
  }
  Serial.println("Exiting idle mode...");
  if (button1pressed) displayMode = DATETIME;
  if (button2pressed) displayMode = STOPWATCHTIMER;
  button1pressed = false;
  button2pressed = false;
}

void showTime(time_t t){
  // display the given time 
  Serial.print(month(t));
  Serial.print("/");
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(year(t)); 
  Serial.print(" ");
  Serial.print(hour(t));
  printDigits(minute(t));
  printDigits(second(t));
  Serial.println();
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void writePulse() {
  digitalWrite(WRITE_PIN, LOW);
  // 250ns delay not explicitly required
  digitalWrite(WRITE_PIN, HIGH);
}

void sendBytes(unsigned char data[], int length, boolean mode)
{
  digitalWrite(MODE_PIN, mode);
  for (int i=0; i<length; i++) {
    for (int c=0; c<8; c++) {
      digitalWrite(ID_PIN[c], data[i] & 1<<c);
    }
    writePulse();
  }
}

void sendCommand(unsigned char control, unsigned char digit[], int length)
{
  sendBytes(&control, 1, 1);
  sendBytes(digit, length, 0);
}

void writeNumber(long n) {
  if ( n < 100000000 ) {
    unsigned char digit[8];
    for (long power=1, i=0; i<8; i++, power*=10) {
      digit[i] = (n/power % 10) | 0x80;
    }
    sendCommand(DISPLAY_ENABLE | DECODE_HEX | SEQUENTIAL_DATA, digit, 8);
    delay(50);
  }
}

// writes 8 digits with decimal points for time "colon"
void writeTime(long n) {
  if ( n < 100000000 ) {
    unsigned char digit[8];
    for (long power=1, i=0; i<8; i++, power*=10) {
      switch (i) {
        case 2:
        case 5:
        case 6:
          digit[i] = (n/power % 10) | 0x00; // With decimal points
          break;
        case 0:
        case 1:
        case 3:
        case 4:
        case 7:
          digit[i] = (n/power % 10) | 0x80; // Without decimal points
          break;
      }
    }
    sendCommand(DISPLAY_ENABLE | DECODE_HEX | SEQUENTIAL_DATA, digit, 8);
    delay(50);
  }
}
// writes 8 digits with decimal points for time "colon"
void writeLapTime(long n) {
  if ( n < 100000000 ) {
    unsigned char digit[8];
    for (long power=1, i=0; i<8; i++, power*=10) {
      switch (i) {
        case 1:
        case 2:
        case 5:
        case 6:
          digit[i] = (n/power % 10) | 0x00; // With decimal points
          break;
        case 0:
        case 3:
        case 4:
        case 7:
          digit[i] = (n/power % 10) | 0x80; // Without decimal points
          break;
      }
    }
    sendCommand(DISPLAY_ENABLE | DECODE_HEX | SEQUENTIAL_DATA, digit, 8);
    delay(50);
  }
}

// writes 8 digits with decimal points for time "colon" and period for mm.dd
// MM.DD
// hh:mm
// Colon is blinking once per second
void writeDateTime(long n, boolean blinking) {
  if ( n < 100000000 ) {
    unsigned char digit[8];
    for (long power=1, i=0; i<8; i++, power*=10) {
      switch (i) {
        case 1:
        case 2:
          if (blinking){
            digit[i] = (n/power % 10) | 0x00; // With decimal points
          } else {
            digit[i] = (n/power % 10) | 0x80; // Without decimal points
          }
          break;
        case 6:
          digit[i] = (n/power % 10) | 0x00; // With decimal points
          break;
        default :
          digit[i] = (n/power % 10) | 0x80; // Without decimal points
          break;
      }
    }
    sendCommand(DISPLAY_ENABLE | DECODE_HEX | SEQUENTIAL_DATA, digit, 8);
    delay(50);
  }
}
