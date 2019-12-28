/*
  Display the current time on the TM1636
*/

#include "Wire.h"
#include <TTSDisplay.h>
TTSDisplay rtcshield;
#define DS3231_I2C_ADDRESS 0x68
bool blink = false;
bool debug = false;

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val) {
  return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val) {
  return( (val/16*10) + (val%16) );
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day-of-week (1=Sunday), date, month, year
  // setDS3231time(0,51,12,4,20,12,25);
  rtcshield.clear();
  rtcshield.pointOn();
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte monthDay, byte month, byte year) {
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(monthDay)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *monthDay, byte *month, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *monthDay = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime() {
  byte second, minute, hour, dayOfWeek, monthDay, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &monthDay, &month, &year);
  // display on the LED

  rtcshield.time(hour, minute);
  if (!blink) {
    rtcshield.pointOn();
  } else {
    rtcshield.pointOff();
  }
  // Start RTC drift compensation
  if (hour == 9 && minute == 1 && second == 0) { // run daily at exactly 9:01 A.M.
     delay(60000); // wait 60 seconds

     setDS3231time(1,1,9,dayOfWeek,monthDay,month,year);

  // Time drift is approx 30 seconds gain per 24 hours. The extra one second is to prevent the program from being stuck
  // in an endless loop. Formula is time drift <in seconds> + 1 second leeway + set time to 1 min, 1 second past the hour.
  // This will result in the clock being put back 30 seconds at 9:01:01 every morning. Adjust formula accordingly.
  // DS3231 crystal is temperature sensitive so during the summer the gain will slow down. Colder temps = faster speeds.
  // Often cheap Chinese modules use watch crystals which are calibrated at blood temperature not room temperature.
  // The above code is not perfect but will help to compensate for this problem.
  
  }
  // end RTC drift compensation
  
  // send it to the serial monitor
  if (debug == true) {
    Serial.print(hour, DEC);
  
    // convert the byte variable to a decimal number when displayed
    Serial.print(":");
    if (minute<10) {
      Serial.print("0");
    }
    Serial.print(minute, DEC);
    Serial.print(":");
    if (second<10) {
      Serial.print("0");
    }
    Serial.print(second, DEC);
    Serial.print(" ");
    Serial.print(month, DEC);
    Serial.print("-");
    Serial.print(monthDay, DEC);
    Serial.print("-");
    Serial.print("20");
    Serial.print(year, DEC);
    Serial.print(" (");
    switch(dayOfWeek){
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
  blink = !blink;
  displayTime(); // display the real-time clock data on the Serial Monitor,

  delay(1000); // every second
}
