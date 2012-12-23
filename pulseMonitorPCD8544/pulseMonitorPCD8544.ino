/*
   >> Pulse Sensor Amped 1.1 <<
This code is for Pulse Sensor Amped by Joel Murphy and Yury Gitman
    www.pulsesensor.com 
    >>> Pulse Sensor purple wire goes to Analog Pin 0 <<<
Pulse Sensor sample aquisition and processing happens in the background via Timer 1 interrupt. 2mS sample rate.
PWM on pins 9 and 10 will not work when using this code, because we are using Timer 1!
The following variables are automatically updated:
Signal :    int that holds the analog signal data straight from the sensor. updated every 2mS.
IBI  :      int that holds the time interval between beats. 2mS resolution.
BPM  :      int that holds the heart rate value, derived every beat, from averaging previous 10 IBI values.
QS  :       boolean that is made true whenever Pulse is found and BPM is updated. User must reset.
Pulse :     boolean that is true when a heartbeat is sensed then false in time with pin13 LED going out.

All the work to find the heartbeat and determine the heartrate happens in the code below.
Pin 13 LED will blink with heartbeat.
If you want to use pin 13 for something else, adjust the interrupt handler
It will also fade an LED on pin fadePin with every beat. Put an LED and series resistor from fadePin to GND.
Check here for detailed code walkthrough:
http://pulsesensor.myshopify.com/pages/pulse-sensor-amped-arduino-v1dot1

Code Version 02 by Joel Murphy & Yury Gitman  Fall 2012
This update changes the HRV variable name to IBI, which stands for Inter-Beat Interval, for clarity.
Switched the interrupt to Timer1.  500Hz sample rate, 2mS resolution IBI value.
Fade LED pin moved to pin 12 (use of Timer1 disables PWM on pins 9 & 10).
Tidied up inefficiencies since the last version.

Warren Gill: Changed the code to use Timer1 for Arduino Leonardo and Arduino Micro instead of Timer2
  Display on a PCD8544 (Nokia 5110) display, using the Adafruit graphics and PCD8544 libraries

*/

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Bounce.h>

/*********************
* Pin assignments:
  Pins going to the PCD8544 routed through a 4050 level shifter
  Arduino___  Device
  A0          Pulse sensor
  D2          Pushbutton
  D3          Pin 4 on LCD: reset (RST) 
  D4          Pin 3 on LCD: Chip select (CS)
  D5          Pin 5 on LCD: Data/Command select (D/C)
  D6          Pin 6 on LCD: Serial data out (DIN)
  D7          Pin 7 on LCD: Serial clock out (SCLK)
  D8          Backlight (through a transistor)
  D11         LED through a resistor, for fading heartbeat
  D13         LED heartbeat detected
*/

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

// For drawing on the LCD
int textX = 45;   // Coordinates for text
int textY = 2;    // 
int heartX = 2;   // Coordinates for heart icon
int heartY = 1;   //
int graphX;       // Current coordinates for pulse graph
int graphY;       //
int lastgraphX;   // Previous coordinates for pulse graph
int lastgraphY;   //
int xMin = 1;     // Minimum and maximum horizontal graph values
int xMax = 84;    //
int yMin = 19;    // Minimum and maximum vertical graph values
int yMax = 47;    //

boolean backlit = false; // Is the display backlit

// Pins and variables
const int buttonPin = 2;    // A button on this pin controls the backlightPin
const int blinkPin = 13;    // Blinks an LED at each beat
const int fadePin = 11;     // Do fancy classy fading blink at each beat
const int backlightPin = 8; // Control the display backlight through a 2222 transistor
int pulsePin = 0;           // Pulse Sensor purple wire connected to analog pin 0
int fadeRate = 0;           // used to fade LED on with PWM on fadePin

Bounce buttonPress = Bounce(buttonPin,10);

// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // holds the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

int signalMax = 0;
int signalMin = 0;

void setup()
{ 
  pinMode(blinkPin, OUTPUT);        // set the LED pins as outputs
  pinMode(fadePin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(backlightPin, OUTPUT);
  
  Serial.begin(115200);             // we agree to talk fast!
  
  // Initialize the LCD
  display.begin();
  xMax = display.width();   // set x and y maximums to actual values
  yMax = display.height();
  
  // Change the contrast around to adapt the display for the best viewing!
  display.setContrast(47);  // somewhere around 45-50 seems to be best
  display.setTextSize(2);
  display.setTextColor(BLACK, WHITE);
  display.clearDisplay();
  
  // Inititalize graph variables
  graphX = xMin;
  graphY = yMin;
  lastgraphX = xMin;
  lastgraphY = yMax;
  
  interruptSetup();   // sets up to read Pulse Sensor signal every 2mS 
}

void loop()
{
  graphHeartbeat(Signal); // add the data to the graph
  
  sendDataToProcessing('S', Signal);     // send Processing the raw Pulse Sensor data

  if (QS == true){   // Quantified Self flag is true when arduino finds a heartbeat
    fadeRate = 255;  // Set 'fadeRate' Variable to 255 to fade LED with pulse
    graphBPM(BPM);   // Send heart rate to screen
    sendDataToProcessing('B',BPM);   // send heart rate with a 'B' prefix
    sendDataToProcessing('Q',IBI);   // send time between beats with a 'Q' prefix
    QS = false;      // reset the Quantified Self flag for next time    
  }
  
  ledFadeToBeat();
  
  //delay(10);                             //  take a break
  
  buttonPress.update();
  
  if ( buttonPress.read() == HIGH ) {      // check if backlight requested
    backlit = !backlit;                    // toggle backlight
    digitalWrite(backlightPin, backlit);
  }
}

void graphHeartbeat(int Signal) {
  if ( Signal > signalMax ) signalMax = Signal;
  if ( Signal < signalMin ) signalMin = Signal;
  graphY = yMax - map(Signal, 0, 1024, yMin, yMax) + yMin;
  if (graphY < yMin) {
    graphY = yMin;
  }
  if (graphY > yMax) {
    graphY = yMax;
  } // shouldn't need this code any more with map function

  graphX++;

  if (graphX > xMax) { // when graph gets to the right margin
    graphX = xMin;
    lastgraphX = xMin;
    display.fillRect(xMin, yMin, xMax, yMax, WHITE);
  }
  
  display.drawLine(lastgraphX, lastgraphY, graphX, graphY, BLACK); // Draw line graph

  display.display();        // show the changes to the buffer
  delay(20);  // break for 20mS. Processing frame-rate = 100.

  // save graphX and graphY
  lastgraphX = graphX;
  lastgraphY = graphY;        
  // move to next pixel
}

void graphBPM(int BPM) {
  int posX = textX;
  if (BPM > 99) posX = posX - 12;  // Right justify
  display.setCursor(posX, textY);
  display.print(" ");
  display.println(BPM);
  display.display();
}

void ledFadeToBeat() {
    fadeRate -= 15;                         //  set LED fade value
    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin,fadeRate);          //  fade LED
  }

void sendDataToProcessing(char symbol, int data ){
    Serial.print(symbol);                // symbol prefix tells Processing what type of data is coming
    Serial.println(data);                // the data to send culminating in a carriage return
  }
