/*
 This sketch controls a door alarm
 
 Uses an Arduino Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
  
 */
const unsigned int LED_PIN = 4;
const unsigned int BUTTON_PIN = 5;
const unsigned int BAUD_RATE = 9600;

#include <SPI.h>
#include <Ethernet.h>

// set up the ethernet shield
byte mymac[]     = {  0x90, 0xA2, 0xDA, 0x00, 0x3F, 0x50 };
byte myip[]      = { 192, 168,  83, 222 }; // address of the Arduino
byte myServer[]  = { 192, 168,  83,   6 }; // ranchero
byte myGateway[] = { 192, 168,  83, 254 }; // fishblue
byte mySubnet[]  = { 255, 255, 255,   0 };

int val = 0;
int saveVal = 0;
char uMsg[ ] = "type=C&description=Closed";

// Initialize the Ethernet library
Client client(myServer, 80);

void setup() {
  pinMode(LED_PIN, OUTPUT); // Stays lit when the door is open
  pinMode(BUTTON_PIN, INPUT); // Flashes when the door opens or closes
  Serial.begin(BAUD_RATE);
  // this part needs to be remodeled to allow retry
  
  Ethernet.begin(mymac, myip, myGateway, mySubnet); // start the Ethernet connection
  delay(1000);  // give the Ethernet shield a second to initialize
}

void loop()
{
  // read the sensor
  val = sensor_closed();
  if (val) {
    Serial.println("Closed");
    digitalWrite(LED_PIN, LOW);
  } else {
    Serial.println("Open");
    digitalWrite(LED_PIN, HIGH);
  }
  if ( val != saveVal ) {
    Serial.println("Changed!");
    flashWarningLED();
    postDBentry(val);
  }
  saveVal = val;
  delay(1000);  
}

int postDBentry(int val) {
  Ethernet.begin(mymac, myip, myGateway, mySubnet); // start the Ethernet connection
  delay(1000);  // give the Ethernet shield a second to initialize

  if (client.connect()) { // received connection to the server
    Serial.println("Connected.");
    client.print("GET /door.php?");
    if (val) {
      client.print("type=C&description=Closed");
      Serial.println("Door was closed.");
    } else {
      client.print("type=O&description=Opened");
      Serial.println("Door was opened.");
    }
    client.println(" HTTP/1.0");
    client.println();
    return 0;
  } else {
    Serial.println("Connection failed.");  // if you didn't get a connection to the server
    // I want to put code to keep trying
    return 1;
  }
}

const int sensor_closed() {
  return analogRead(HALL_SENSOR_PIN) == 0;
}

const int flashWarningLED() {
  digitalWrite(WARN_PIN, HIGH);
  delay(100);
  digitalWrite(WARN_PIN, LOW);
  delay(100);
  digitalWrite(WARN_PIN, HIGH);
  delay(100);
  digitalWrite(WARN_PIN, LOW);
  delay(100);
  digitalWrite(WARN_PIN, HIGH);
  delay(100);
  digitalWrite(WARN_PIN, LOW);
  return HIGH;
}
