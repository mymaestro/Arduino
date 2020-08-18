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
  Display on 128x32 OLED display, using the Adafruit graphics and SSD1306 libraries

*/
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce.h>

/*********************
* Pin assignments:
  Pins going to the SSD1306 128x32 OLED
  Arduino Micro Device
  A0          Pulse sensor
  D2          Pin 1 on OLED: (SDA)
  D3          Pin 2 on OLED: (SCL) 
  D4          Pin 3 on OLED: reset (RST)
  D5          Button
  D11         LED through a resistor, for fading heartbeat
  D13         LED heartbeat detected
*/
/*
   The format of our output.

   Set this to PROCESSING_VISUALIZER if you're going to run
    the Processing Visualizer Sketch.
    See https://github.com/WorldFamousElectronics/PulseSensor_Amped_Processing_Visualizer

   Set this to SERIAL_PLOTTER if you're going to run
    the Arduino IDE's Serial Plotter.
*/
const int OUTPUT_TYPE = SERIAL_PLOTTER;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins and variables
const int buttonPin = 5;    // A button on this pin controls the backlightPin
const int blinkPin = 13;    // Blinks an LED at each beat
const int fadePin = 11;     // Do fancy classy fading blink at each beat
const int backlightPin = 8; // Control the display backlight through a 2222 transistor

Bounce buttonPress = Bounce(buttonPin, 10);

int _circularBuffer[SCREEN_WIDTH]; //fast way to store values (rather than an ArrayList with remove calls)
int _curWriteIndex = 0;            // tracks where we are in the circular buffer

// for tracking fps
unsigned long _totalFrameCount = 0;
unsigned long _startTimeStamp = 0;

// status bar
boolean _drawStatusBar = true; // change to show/hide status bar
int _graphHeight = SCREEN_HEIGHT;

/*
Pinout:
 PULSE_INPUT = Analog Input. Connected to the pulse sensor purple (signal) wire.
 PULSE_BLINK = digital Output. Connected to an LED (and 220 ohm resistor) that will flash on each detected pulse.
 PULSE_FADE = digital Output. PWM pin onnected to an LED (and resistor) that will smoothly fade with each pulse.
 
 NOTE: PULSE_FADE must be a pin that supports PWM. Do not use pin 9 or 10, because those pins' PWM interferes with the sample timer.
*/

const int PULSE_INPUT      = A0;
const int PULSE_BLINK      = 13;   // Pin 13 is the on-board LED
const int PULSE_FADE       = 5;
const int MIN_ANALOG_INPUT = 0;
const int MAX_ANALOG_INPUT = 1023;
const int THRESHOLD        = 550;  // Adjust this number to avoid noise when idle
const int DELAY_LOOP_MS    = 5;    // change to slow down how often to read and graph value

/*
   All the PulseSensor Playground functions.
*/
PulseSensorPlayground pulseSensor;

void setup()
{
  pinMode(blinkPin, OUTPUT); // set the LED pins as outputs
  pinMode(fadePin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(backlightPin, OUTPUT);

  Serial.begin(115200); // we agree to talk fast!

  // Configure the PulseSensor manager.

  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);
  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin())
  {
    /*
       PulseSensor initialization failed, likely because our particular Arduino
       platform interrupts aren't supported yet.

       If your Sketch hangs here, try PulseSensor_BPM_Alternative.ino,
       which doesn't use interrupts.
    */
    for (;;)
    {
      // Flash the led to show things didn't work.
      digitalWrite(PULSE_BLINK, LOW);
      delay(50);
      digitalWrite(PULSE_BLINK, HIGH);
      delay(50);
    }
  }

  // Initialize the OLED
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64, 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);
  display.println("Screen initialized!");
  display.display();
  delay(500);
  display.clearDisplay();

  if (_drawStatusBar)
  {
    _graphHeight = SCREEN_HEIGHT - 10;
  }
}

void loop()
{
  /*
     Wait a bit. We don't output every sample, because our baud rate won't support that much I/O.
  */
  delay(DELAY_LOOP_MS); // change to slow down how often to read and graph value

  if (_startTimeStamp == 0)
  {
    _startTimeStamp = millis();
  }

  int analogVal = pulseSensor.getLatestSample();
  pulseSensor.outputSample();

  _circularBuffer[_curWriteIndex++] = analogVal;

  if (_curWriteIndex >= display.width())
  {
    _curWriteIndex = 0;
  }

  int myBPM = pulseSensor.getBeatsPerMinute(); // Calls pulseSensor object to get beats/min

  // If a beat has happened since we last checked, write the per-beat information to Serial.
  if (pulseSensor.sawStartOfBeat())
  {
    pulseSensor.outputBeat();
  }

  display.clearDisplay();
  if (_drawStatusBar)
  {
    // Draw current beats per minute
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds("XXS BPM", 0, 0, &x1, &y1, &w, &h);
    display.setCursor(display.width() - w, 0);
    display.print(myBPM);
    display.print(" BPM");
  }

  // Draw the line graph
  int xPos = 0;
  for (int i = _curWriteIndex; i < display.width(); i++)
  {
    // Draw the line
    int analogVal = _circularBuffer[i];
    drawLine(xPos, analogVal);
    xPos++;
  }

  for (int i = 0; i < _curWriteIndex; i++)
  {
    // Draw the line
    int analogVal = _circularBuffer[i];
    drawLine(xPos, analogVal);
    xPos++;
    ;
  }

  display.display();
  _totalFrameCount++;
  delay(DELAY_LOOP_MS);
}

void drawLine(int xPos, int analogVal)
{
  int lineHeight = map(analogVal, MIN_ANALOG_INPUT, MAX_ANALOG_INPUT, 0, _graphHeight);
  int yPos = display.height() - lineHeight;
  display.drawFastVLine(xPos, yPos, lineHeight, SSD1306_WHITE);
}
