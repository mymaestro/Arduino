/*
 This sketch controls a door alarm
 Enhanced to watch multiple doors
 
 Hall effect sensors detect wheh a door is open or closed.
 When the door changes (opens or closes) an entry is made in the web site database.
 
 Uses an Arduino Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 *  - The Arduino Ethernet shield also uses pin 4 for SD card
 * Hall effect sensor connected to +5V and Gnd, and Analog pin 2 through a 10k pullup resistor
 * LED's connected to pins 7 and 8 for notifications through 1k resistors
 
 * 7/7/2011 add alarm function if the door is open longer than x seconds
 * 9/5/2011 expanding to have more than one sensor
 * 1/29/2012 update for Arduino 1.0
 * 2/29/2012 fixed several defects
 */

#define DEBUG
#include "Arduino.h"
#include <debugUtil.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Time.h>
#include <TimeAlarms.h>

/* For hardware connections */
//const unsigned int HALL_SENSOR_PIN = 2;
const unsigned int RED_LED_PIN = 7;
const unsigned int GREEN_LED_PIN = 8;
const unsigned int BAUD_RATE = 9600;

int      ALARM_WARN_TIMEOUT = 59;       // if the door is open this many seconds send an alarm
int      ALARM_REPOST_TIMEOUT = 3600;   // and repeat the alarm this often until the door is closed
int      BLINK_DELAY = 100;             // ms for flashing the LEDs
int      sensorSize = 0;
int      hallSensorPins[] = { 2 };
boolean  hallSensorState[] = { false }; // keep the status of all the doors
boolean  doorChanged = false;           // did any door change since the last look
boolean  doorAlarmSent = false;         // keep track of whether we are in alarm state
boolean  doorsOpen = false;             // if *any* doors are open

/* For ethernet client */
int      stringPos = 0;                 // string index
char     inString[32];                  // for incoming data
boolean  startRead = false;             // are we reading now?

AlarmId  oDelay;                        // setup alarms

/* Set up the ethernet shield */
byte mymac[]     = { 0x90, 0xA2, 0xDA, 0x00, 0x3F, 0x50 };
byte myip[]      = { 192, 168,  83, 223 }; // address of the Arduino
/* other shield
byte mymac[]     = { 0x90, 0xA2, 0xDA, 0x00, 0x47, 0x78 };
byte myip[]      = { 192, 168,  83, 224 }; // address of the Arduino
*/
byte myServer[]  = { 192, 168,  83,   6 }; // ranchero
byte myGateway[] = { 192, 168,  83, 254 }; // fishblue
byte mySubnet[]  = { 255, 255, 255,   0 };

char uMsg[ ] = "type=C&description=XyzzyX"; // declare the string URL
EthernetClient client;

void setup() {
  setTime(6,0,0,1,1,11); // Set the time to 6:00 January 1st 2011
  // for this application we don't care about the exact time, just elapsed time
  pinMode(RED_LED_PIN, OUTPUT); // Stays lit when the door is open
  pinMode(GREEN_LED_PIN, OUTPUT); // Flashes when the door opens or closes
  sensorSize = sizeof(hallSensorPins) - 1;
  for (int i = 0; i < sensorSize; i++) {
    pinMode(hallSensorPins[i], INPUT);
  }
  AlarmId oDelay = Alarm.timerOnce(ALARM_WARN_TIMEOUT, OnceOnly); // Initialize alarm
  Serial.begin(BAUD_RATE);

  // Initialize the Ethernet library
  Ethernet.begin(mymac, myip, myGateway, mySubnet); // start the Ethernet connection
  delay(1000);  // give the Ethernet shield a second to initialize
}

void loop()
{
  // presets
  doorChanged = false;
  doorsOpen = false;

//  DEBUG_PRINT("SENSOR SIZE");
//  DEBUG_PRINT(sensorSize);
  
  // read the sensors
  for (int i = 0; i < sensorSize; i++) {
    
    boolean sensorState = sensor_closed(hallSensorPins[i]);
    if (!sensorState) doorsOpen = true; // a door is open
    
    if (sensorState != hallSensorState[i]) { // A door changed!
      doorChanged = true;
      DEBUG_PRINT("Changed.");
      if (sensorState) { // If the sensor detected the door is closed
        // Door number i is closed
        DEBUG_PRINT("Closed.");
        // turn off alarms for i
      } else {
        //on
        DEBUG_PRINT("Open."); // turn on alarms for i
      }
      writeDoorEntry(i, sensorState);
    }
    hallSensorState[i] = sensorState;
  }
  if (doorChanged) {
    flashWarningLED();
    if (doorsOpen) {  // at least one door is open!
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
    } else { // all the doors are closed now
      Alarm.disable(oDelay); // prevent the alarm from triggering
      DEBUG_PRINT("Disabling alarm");
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
    }
  }
  Alarm.delay(500);
}

// returns true if the sensor detects the door is closed
boolean sensor_closed(int sensor) {
  int j = 0;
  int sensorVal = analogRead(sensor);
//  DEBUG_PRINT("Sensor on pin:");
//  DEBUG_PRINT(sensor);
//  DEBUG_PRINT(sensorVal);
  if ( sensorVal < 256 ) j++;
  return j > 0; // this should be 1 or 0 but there seems to be some noise
}

void writeDoorEntry(int doorNumber, boolean doorState) {
//  postDBentry(doorNumber);
  if (doorState) { // the door closed
    if ( doorAlarmSent ) { // we already sent an alert so cancel it
       postDBentry(3);
       doorAlarmSent = false;
       DEBUG_PRINT("Setting doorAlarmSent false");
    }
    postDBentry(1);
  } else { // Door open
    postDBentry(0);
    Alarm.enable(oDelay); //enable the alarm
    DEBUG_PRINT("Enabling alarm");
  }
}

void OnceOnly() { // The door has been open too long!
  postDBentry(2);  // enter alarm into the database
  DEBUG_PRINT("Posting alarm");
  flashWarningLED;
  oDelay = Alarm.timerOnce(ALARM_REPOST_TIMEOUT, OnceOnly); // start a new alarm
  doorAlarmSent = true;
  DEBUG_PRINT("Setting doorAlarmSent true");
  DEBUG_PRINT("Enabling alarm again");
}

int postDBentry(int val) { // Puts an entry into the database on the server
  if (client.connect(myServer, 80)) { // received connection to the server
    DEBUG_PRINT("Connected.");
    client.print("GET /door.php?");
    switch (val) {
    case 0:
      client.print("type=O&description=Opened");
      DEBUG_PRINT("Database enter Door was opened.");
      break;
    case 1:
      client.print("type=C&description=Closed");
      DEBUG_PRINT("Database enter Door was closed.");
      break;
    case 2:
      client.print("type=A&description=Alarm");
      DEBUG_PRINT("Database enter Door open alarm.");
      break;
    case 3:
      client.print("type=N&description=Cancel");
      DEBUG_PRINT("Database enter cancel alarm.");
      break;
    default:
      DEBUG_PRINT("Error.");
    }
    client.println(" HTTP/1.0");
    client.println();
    DEBUG_PRINT("Client write complete. Disconnecting.");
    /* Read the response back from the server. It could post back a QUIT message
     that would tell this program to turn off the alert system or do other
     nice things like enable/disable sensors etc */

    //  stringPos = 0;
    //  memset( &inString, 0, 32 ); //clear inString memory

    // get response from the server
    //  while(true) {
    //    if (client.available()) {
    //      char c = client.read();
    //      if (c == '<' ) { //'<' is our beginning character
    //        startRead = true; //Ready to start reading the part 
    //      } else if (startRead) {
    //        if(c != '\n') { // end with carriage return
    //          inString[stringPos] = c;
    //          stringPos ++;
    //        } else {
    //          //got what we need here! We can disconnect now
    //          startRead = false;
    //          client.stop();
    //          client.flush();
    //          DEBUG_PRINT("disconnecting.");
    //          return inString;
    //        }
    //      }
    //    }
    //  }
    client.stop();
    return 0;
  } 
  else {
    DEBUG_PRINT("Connection failed.");  // if you didn't get a connection to the server
    return 1;
  }
}

const int flashWarningLED() { // alternate flashing red and green LEDs
  for ( int i = 0; i < 11; i++ ) {
    // DEBUG_PRINT("blink.");
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    Alarm.delay(BLINK_DELAY);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
    Alarm.delay(BLINK_DELAY);
  }
  digitalWrite(GREEN_LED_PIN, HIGH);  
  return 0;
}


