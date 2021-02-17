/*
Based on https://github.com/Deftaudio/MacroPot
BSD 3-Clause License

Copyright (c) 2019, Andrei Kudryavtsev
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MIDI.h>

#define STATUS_LED 21          // LED pin on Teensy
#define OLED_RESET 4           // RESET pin for OLED
#define POWERSAVE 15000        // How many milliseconds to keep the backlight on
#define MIDI_PANIC_TIME 1500   // How long a button press to invoke the MIDI panic
#define RESET_BUTTON_TIME 5000 // How long the button has been pressed to initiate a reset
#define BUTTON_UP_PIN 14       // "Up" button
#define BUTTON_DN_PIN 15       // "Down" button
#define ENCODER_PIN_A 12       // The rotary encoder is attached to pins 12 and 11
#define ENCODER_PIN_B 11       // and ground of course
#define ENCODER_BUTTON_PIN 2   // Button in the rotary encoder attached to this pin
#define ENCODER_LED_1 22       // LED on the encoder is attached to this pin
#define ENCODER_LED_2 23       // Other LED on the encoder is attached to this pin

Adafruit_SSD1306 display(OLED_RESET);

#include <Bounce.h>
#include <EEPROM.h>
#include <Encoder.h>

// Create a MIDI instance for each serial interface
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);

#define USBMIDIPORT 0
#define MIDI1PORT 1
#define MIDI2PORT 2
#define MIDI3PORT 3

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;

int i;
int blinkrate = 0;
bool activity = false;
bool dataintegrity = false;
const int totalChannels = 17;

// Setup buttons
Bounce encoderButton = Bounce(ENCODER_BUTTON_PIN, 5);
Bounce upButton = Bounce(BUTTON_UP_PIN, 5);
Bounce downButton = Bounce(BUTTON_DN_PIN, 5);

// Encoder acceleration slope
int height = 4;    //height
float slope = 0.3; //slope

unsigned long last_time;
unsigned long delta_time = 0;
unsigned long time_now = 0;
unsigned long encoderSlope = 1;

// Encoder config
Encoder encoder0(ENCODER_PIN_A, ENCODER_PIN_B);
//long encoderPosition  = -999;
long newEncoderReading = -999;
int maxEncoder = 512;
int maxValue = 127;
int encoderAcc = 10;
int progress = 0;
int val = 0;
int midiChannel = 1;  // The MIDI channel to output

//static uint8_t sysex[6] = {0x7D, 0x01, 0x10, 0x0e, 0x0f, 0x01};

int notesOn = 0; // How many notes are on
char *theNoteNames[] = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};
// The three kinds of parameters you can change
char *midiParameterName[] = {"Channel: ",
                             "Program: ",
                             "Volume:  "};
char *outputNames[] = {"USBmidi", "MIDI1", "MIDI2", "MIDI3"};

struct MIDIBabel
{
  char name[8] = {'C', 'h', 'a', 'n', '.', '0', '1', '\0'}; // Display name
  byte MIDImerge = 0;      // Bitmasked map of outputs bitmask (1 = send, 0 = don't send)
                           //  USBport | MIDI1 | MIDI2 | MIDI3 
  uint8_t type = 0;        // Unused for now
  uint8_t notenumber;      // Default MIDI note number
  uint8_t prognumber;      // Default program number
  uint8_t channel = 1;     // MIDI Channel
  uint8_t lowvalue;        // Low Value
  uint8_t highvalue;       // High Value
  uint8_t velocity;        // Velocity for Note ON messages
  uint8_t volume = 100;    // Default volume setting
  uint8_t mapstart;        // Low Value for CC, main value for Note
  uint8_t mapend;          // High Value for CC, ignored Note
  int deviation = 0;       // For creating curves for CC messages deviation -100 to 100.
  boolean toggled = false; // For Note or Program Change toggled tracking
  long encoderPosition = 0;  // Starting position
};

# MIDIBabel[currentChannel].encoderPosition
# MIDIBabel[currentChannel].MIDImerge

struct MIDIBabel MIDIBabel[totalChannels];
uint8_t currentChannel = 0;

// In this build, only usbMIDI and MIDI1 have MIDI IN capbility.
void setup() {
  Serial.begin(115200);   // Serial terminal console baud rate
  //Serial.begin(31250);  // MIDI hardware baud rate
  pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DN_PIN, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(ENCODER_LED_1, OUTPUT);
  pinMode(ENCODER_LED_2, OUTPUT);
  analogWrite(ENCODER_LED_1, 0);
  analogWrite(ENCODER_LED_2, 0);

  // MIDI
  MIDI1.begin(MIDI_CHANNEL_OMNI);
  MIDI2.begin(MIDI_CHANNEL_OMNI);
  MIDI3.begin(MIDI_CHANNEL_OMNI);
  MIDI1.turnThruOff();
  MIDI2.turnThruOff();
  MIDI3.turnThruOff();
  // Connect MIDI status changes involving a channel to handlers
  MIDI1.setHandleNoteOn(HandleNoteOn);
  MIDI1.setHandleNoteOff(HandleNoteOff);
  MIDI1.setHandleAfterTouchPoly(HandleAfterTouchPoly);
  MIDI1.setHandleControlChange(HandleControlChange);
  MIDI1.setHandleProgramChange(HandleProgramChange);
  MIDI1.setHandleAfterTouchChannel(HandleAfterTouchChannel);
  MIDI1.setHandlePitchBend(HandlePitchBend);

  /* want to handle these because my controller pushes them */
  MIDI1.setHandleContinue(HandleContinue);
  MIDI1.setHandleStart(HandleStart);
  MIDI1.setHandleStop(HandleStop);

  /* What to do with SysEx messages?
  usbMIDI.setHandleSystemExclusive(mySystemExclusiveChunk);
  MIDI1.setHandleSystemExclusive(mySystemExclusiveChunk);
 */
  usbMIDI.setHandleClock(myClock); // why we doin' this?

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(5);

  // Clear the display buffer
  display.clearDisplay();

  // Text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("MIDIBabel");
  display.drawRect(0, 17, display.width(), display.height() - 17, WHITE);
  display.display();
  delay(100);

  // Export all EPROM data
  // usbMIDI.sendSysEx(6, sysex);

  // Version LED blink
  analogWrite(ENCODER_LED_1, 255);
  digitalWriteFast(STATUS_LED, HIGH);
  delay(200);
  digitalWriteFast(STATUS_LED, LOW);
  analogWrite(ENCODER_LED_1, 0);
  
  analogWrite(ENCODER_LED_2, 255);
  digitalWriteFast(STATUS_LED, HIGH);
  delay(200);
  digitalWriteFast(STATUS_LED, LOW);
  analogWrite(ENCODER_LED_2, 0);
  
  upButton.update();
  downButton.update();

  // Zero Macro state
  currentChannel = 0;

  //EPROM LOAD
  EPROMload();

  //INIT button UP/DOWN code
  checkButtons();

  //INIT check encoder button
  checkEncButton();

  //initial display macros name
  DisplayTitleUpdate();
} // setup()

void cycleBits(byte MIDImerge)
{
  for (int i = 0, mask = 1; i < 4; i++, mask = mask << 1)
  {
    if (MIDImerge & mask)
    {
      // bit is on
    }
    else
    {
      // bit is off
    }
  }
}
/*
 * Sometimes the synth gets a noteOn but loses the noteOff, and the note keeps playing forever
 * These two channel control messages turn everything off, commonly called the "MIDI Panic" button
 *
 * CC 120 is All Sound Off
 * CC 123 is All Notes Off
 */
void midiPanicButton() {
  lcd.setCursor(0, 3);
  lcd.print("ALL NOTES OFF");
  for (int channel=1; channel <= 16; channel++) {
    MIDI1.sendControlChange(120, 0, channel);
    MIDI1.sendControlChange(123, 0, channel);
    MIDI2.sendControlChange(120, 0, channel);
    MIDI2.sendControlChange(123, 0, channel);
    MIDI3.sendControlChange(120, 0, channel);
    MIDI3.sendControlChange(123, 0, channel);
    usbMIDI.sendControlChange(120, 0, channel);
    usbMIDI.sendControlChange(123, 0, channel);
  }
  notesOn = 0;
}

void HandleNoteOn(byte channel, byte pitch, byte velocity)
{
  if (MIDImerge & (1 << USBMIDIPORT)) usbMIDI.sendNoteOn(pitch, velocity, midiChannel);
  if (MIDImerge & (1 << MIDI1PORT)) MIDI1.sendNoteOn(pitch, velocity, midiChannel);
  if (MIDImerge & (1 << MIDI2PORT)) MIDI2.sendNoteOn(pitch, velocity, midiChannel);
  if (MIDImerge & (1 << MIDI3PORT)) MIDI3.sendNoteOn(pitch, velocity, midiChannel);
  notesOn++;
  int noteIndex = (pitch - 12) % 12;
  int noteOctave = (pitch / 12) - 1;
  turnBacklightOn(); // Turn the light on when a MIDI key is pressed
  lcd.setCursor(0, 3);
  lbg.drawValue(velocity, 127);
  lcd.setCursor(0, 3);
  lcd.print("[");
  lcd.print(theNoteNames[noteIndex]);
  lcd.print(noteOctave);
  lcd.print("]");
  if (velocity == 0)
  {
    digitalWrite(STATUS_LED, LOW);
  }
  else
  {
    digitalWrite(STATUS_LED, HIGH);
  }
}
void HandleNoteOff(byte channel, byte pitch, byte velocity) {
  if (MIDImerge & (1 << USBMIDIPORT)) usbMIDI.sendNoteOff(pitch, velocity, midiChannel);
  if (MIDImerge & (1 << MIDI1PORT)) MIDI1.sendNoteOff(pitch, velocity, midiChannel);
  if (MIDImerge & (1 << MIDI2PORT)) MIDI2.sendNoteOff(pitch, velocity, midiChannel);
  if (MIDImerge & (1 << MIDI3PORT)) MIDI3.sendNoteOff(pitch, velocity, midiChannel);
  notesOn--;
  if (notesOn <= 0)
  {
    digitalWrite(STATUS_LED, LOW);
    lcd.setCursor(0, 3);
    lbg.drawValue(0, 127);
  }
}
void HandleAfterTouchPoly(byte channel, byte note, byte pressure) {
  if (MIDImerge & (1 << USBMIDIPORT)) usbMIDI.sendAfterTouch(pressure, midiChannel);
  if (MIDImerge & (1 << MIDI1PORT)) MIDI1.sendAfterTouch(pressure, midiChannel);
  if (MIDImerge & (1 << MIDI2PORT)) MIDI2.sendAfterTouch(pressure, midiChannel);
  if (MIDImerge & (1 << MIDI3PORT)) MIDI3.sendAfterTouch(pressure, midiChannel);
}
void HandleControlChange(byte channel, byte number, byte value) {
  if (MIDImerge & (1 << USBMIDIPORT)) usbMIDI.sendControlChange(number, value, midiChannel);
  if (MIDImerge & (1 << MIDI1PORT)) MIDI1.sendControlChange(number, value, midiChannel);
  if (MIDImerge & (1 << MIDI2PORT)) MIDI2.sendControlChange(number, value, midiChannel);
  if (MIDImerge & (1 << MIDI3PORT)) MIDI3.sendControlChange(number, value, midiChannel);
}
void HandleProgramChange(byte channel, byte number) {
  if (MIDImerge & (1 << USBMIDIPORT)) usbMIDI.sendProgramChange(number, midiChannel);
  if (MIDImerge & (1 << MIDI1PORT)) MIDI1.sendProgramChange(number, midiChannel);
  if (MIDImerge & (1 << MIDI2PORT)) MIDI2.sendProgramChange(number, midiChannel);
  if (MIDImerge & (1 << MIDI3PORT)) MIDI3.sendProgramChange(number, midiChannel);
}
void HandleAfterTouchChannel(byte channel, byte pressure) {
  if (MIDImerge & (1 << USBMIDIPORT)) usbMIDI.sendAfterTouch(pressure, midiChannel);
  if (MIDImerge & (1 << MIDI1PORT)) MIDI1.sendAfterTouch(pressure, midiChannel);
  if (MIDImerge & (1 << MIDI2PORT)) MIDI2.sendAfterTouch(pressure, midiChannel);
  if (MIDImerge & (1 << MIDI3PORT)) MIDI3.sendAfterTouch(pressure, midiChannel);
}
void HandlePitchBend(byte channel, int bend) {
  if (MIDImerge & (1 << USBMIDIPORT)) usbMIDI.sendPitchBend(bend, midiChannel)
  if (MIDImerge & (1 << MIDI1PORT)) MIDI1.sendPitchBend(bend, midiChannel)
  if (MIDImerge & (1 << MIDI2PORT)) MIDI2.sendPitchBend(bend, midiChannel)
  if (MIDImerge & (1 << MIDI3PORT)) MIDI3.sendPitchBend(bend, midiChannel)
}
void HandleContinue() {
}
void HandleStart() {
}
void HandleStop() {
}

void HandleSysEx() { // Yeah, rewrite this
  // SysEx messages are special.  The message length is given in data1 & data2
  unsigned int SysExLength = data1 + data2 * 256;
  usbMIDI.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 0);
  usbMIDI.send_now();

  if (bitRead(MIDIBabel[currentChannel].MIDImerge, 0) == true)
  {
    MIDI1.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
  }
  if (bitRead(MIDIBabel[currentChannel].MIDImerge, 1) == true)
  {
    MIDI2.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
  }
  if (bitRead(MIDIBabel[currentChannel].MIDImerge, 2) == true)
  {
    MIDI3.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
  }
}
void myClock()
{
  digitalWrite(STATUS_LED, LOW); // Any Note-On turns on LED
  analogWrite(ENCODER_LED_1, 0);
  blinkrate++;
  if (blinkrate >= 4)
  {
    blinkrate = -16;
    digitalWrite(STATUS_LED, HIGH);
    analogWrite(ENCODER_LED_1, 255);
  }
}

void EPROMsave()
{
  double addr = 0;
  for (i = 0; i < totalChannels; i++)
  {
    EEPROM.put(addr, MIDIBabel[i]);
    addr = addr + sizeof(MIDIBabel[i]);
  }
  EEPROM.put(addr + sizeof(MIDIBabel[totalChannels]), currentChannel);
}

void EPROMload()
{
  double addr = 0;
  for (i = 0; i < totalChannels; i++)
  {
    EEPROM.get(addr, MIDIBabel[i]);
    addr = addr + sizeof(MIDIBabel[i]);
  }
  EEPROM.get(addr + sizeof(MIDIBabel[totalChannels]), currentChannel);
}

/* Here is the loop */
void loop()
{
  activity = false;

// What to do when the knob is turned

// What to do when the encoder button is pressed

// What to do when the up button is pressed

// What to do when the down button is pressed

  MIDI.read();
  
  //END OF HARDWARE FLOW
  // USB FLOW: coming out of USB
  if (usbMIDI.read())
  {
    // Get the USB MIDI message, defined by these 5 numbers (except SysEX)
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();
    byte cable = usbMIDI.getCable();

    // Forward this message to 1 of the 3 Serial MIDI OUT ports
    if (type != usbMIDI.SystemExclusive)
    {
      // Normal messages, first we must convert usbMIDI's type (an ordinary
      // byte) to the MIDI library's special MidiType.
      midi::MidiType mtype = (midi::MidiType)type;

      // Then simply give the data to the MIDI library send()
      switch (cable)
      {
      case 0:
        MIDI1.send(mtype, data1, data2, channel);
        break;
      case 1:
        MIDI2.send(mtype, data1, data2, channel);
        break;
      case 2:
        MIDI3.send(mtype, data1, data2, channel);
        break;
      }
    }
    else
    {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;

      // Unpack SysEx
      byte *sysbytes = usbMIDI.getSysExArray();

      switch (cable)
      {
      case 0:
        MIDI1.sendSysEx(SysExLength, sysbytes, true);
        break;
      case 1:
        MIDI2.sendSysEx(SysExLength, sysbytes, true);
        break;
      case 2:
        MIDI3.sendSysEx(SysExLength, sysbytes, true);
        break;
      }
    }
    activity = true;
  }

  // Button UP/DOWN code
  checkButtons();

  // Check encoder button
  checkEncButton();

  // Encoder read and min/max values adjust
  newEncoderReading = encoder0.read();
  if (MIDIBabel[currentChannel].Controller[0].type != 5)
  {
    if (newEncoderReading > maxEncoder)
    {
      newEncoderReading = maxEncoder;
      encoder0.write(maxEncoder);
    }

    if (newEncoderReading < 0)
    {
      newEncoderReading = 0;
      encoder0.write(0);
    }
  }

  // Update on encoder change
  if (newEncoderReading != MIDIBabel[currentChannel].encoderPosition)
  {

    //encoder acceleration calc
    time_now = millis();
    delta_time = time_now - last_time;
    if (delta_time < 27)
    {
      encoderSlope = int(0.3 * (27 - delta_time) + 1);
    }
    last_time = time_now;
    encoderAcc = (newEncoderReading - MIDIBabel[currentChannel].encoderPosition) * encoderSlope;

    if ((newEncoderReading > MIDIBabel[currentChannel].encoderPosition) && (newEncoderReading + encoderAcc < maxEncoder))
    {
      MIDIBabel[currentChannel].encoderPosition = newEncoderReading + encoderAcc;
      encoder0.write(MIDIBabel[currentChannel].encoderPosition);
    }
    else if ((newEncoderReading < MIDIBabel[currentChannel].encoderPosition) && (newEncoderReading + encoderAcc > 0))
    {
      MIDIBabel[currentChannel].encoderPosition = newEncoderReading + encoderAcc;
      encoder0.write(MIDIBabel[currentChannel].encoderPosition);
    }
    else
      MIDIBabel[currentChannel].encoderPosition = newEncoderReading;

    if (MIDIBabel[currentChannel].Controller[0].type != 5)
    {
      UpdateState();
    }

    EncoderProcessing();
  }

  // blink the LED when any activity has happened
  if (activity)
  {
    digitalWriteFast(STATUS_LED, HIGH); // LED on
    ledOnMillis = 0;
  }
  if (ledOnMillis > 15)
  {
    digitalWriteFast(STATUS_LED, LOW); // LED off
  }

  // Clean up for Synclavier mode - autozero function.
  if (ledOnMillis > 200 && MIDIBabel[currentChannel].Controller[0].type == 5 && MIDIBabel[currentChannel].Controller[0].toggled == true)
  {
    encoderAcc = 0;
    EncoderProcessing();
    MIDIBabel[currentChannel].Controller[0].toggled = false;
    analogWrite(ENCODER_LED_2, 0);
  }
  delayMicroseconds(50); // limits message frequency
} //loop()

void DisplayTitleUpdate()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(String(MIDIBabel[currentChannel].name));
  display.drawRect(0, 17, display.width(), display.height() - 17, WHITE);
  display.display();
}

void checkButtons()
{
  upButton.update();
  downButton.update();
  /*
  // This is encoder button
  encoderButton.update();
  if (encoderButton.fallingEdge()) {
   display.clearDisplay();
   display.setCursor(0,0);
   display.setTextColor(BLACK, WHITE);
   display.print("Not avail.");
   display.display();
   delay(500);
   display.clearDisplay();
   DisplayTitleUpdate();
   UpdateState();
   activity = true;
  }
*/
  // This is UP button
  if (upButton.fallingEdge())
  {
    delay(100);
    downButton.update();
    if (downButton.fallingEdge())
    { // Both buttons are pressed
      SysexMode();
    }
    else
    {
      //display.setCursor(40,18);
      //display.print("-b1next-");
      //display.display();
      if (currentChannel < totalChannels - 1)
      {
        currentChannel++;
      }
      else
      {
        analogWrite(ENCODER_LED_1, 255);
        delay(100);
        digitalWriteFast(STATUS_LED, LOW);
        analogWrite(ENCODER_LED_1, 0);
      }
      if (MIDIBabel[currentChannel].Controller[0].type != 5)
      {
        DisplayTitleUpdate();
        UpdateState();
      }
      activity = true;
      EPROMsave();
    }
  }
  // This is Down button
  else if (downButton.fallingEdge())
  {
    delay(100);
    upButton.update();
    if (upButton.fallingEdge())
    { // Both buttons are pressed
      SysexMode();
    }
    else
    {
      //display.setCursor(40,18);
      //display.print("-b2prev-");
      //display.display();
      if (currentChannel > 0)
      {
        currentChannel--;
      }
      else
      {
        analogWrite(ENCODER_LED_1, 255);
        delay(100);
        digitalWriteFast(STATUS_LED, LOW);
        analogWrite(ENCODER_LED_1, 0);
      }
      if (MIDIBabel[currentChannel].Controller[0].type != 5)
      {
        DisplayTitleUpdate();
        UpdateState();
      }
      activity = true;
      EPROMsave();
    }
  }

  /* 
  if (upButton.risingEdge()) {
      display.setCursor(40,18);
      display.print("-b1off-");
      display.display();
   }
  
  
  if (downButton.risingEdge()) {
      display.setCursor(40,18);
      display.print("-b2off-");
      display.display();
   }
*/
}

void UpdateState()
{
  //WRITE EPROM HERE TODO----
  encoder0.write(MIDIBabel[currentChannel].encoderPosition);
  val = map(MIDIBabel[currentChannel].encoderPosition, 0, maxEncoder, 0, maxValue);
  // Display update
  display.setTextColor(BLACK, WHITE);
  display.setCursor(90, 0);
  display.print("   ");
  display.setCursor(90, 0);
  display.print(val);
  progress = map(MIDIBabel[currentChannel].encoderPosition, 0, maxEncoder, 2, display.width() - 2);
  display.drawRect(0, 17, display.width(), display.height() - 17, WHITE);
  display.fillRect(progress, 19, display.width() - progress - 2, display.height() - 21, BLACK);
  display.fillRect(2, 19, progress - 2, display.height() - 21, WHITE);
  /* TEST AREA
        display.setCursor(0,0);
        display.print("     ");
        display.setCursor(0,0);
        display.print(encoderAcc);
       */
  display.display();
  analogWrite(ENCODER_LED_2, map(MIDIBabel[currentChannel].encoderPosition, 0, maxEncoder, 0, 255));
}

void checkEncButton()
{
  encoderButton.update();
  if (MIDIBabel[currentChannel].Button.push)
  {
    // Button was pushed
    if (encoderButton.fallingEdge() && (MIDIBabel[currentChannel].Button.type > 0))
    {
      // Press event for this macro
      if (MIDIBabel[currentChannel].Button.type == 1)
      {
        // CC
        if (MIDIBabel[currentChannel].Button.USBport > 0)
        {
          usbMIDI.sendControlChange(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel, MIDIBabel[currentChannel].Button.USBport - 1);
          usbMIDI.send_now();
        }
        switch (MIDIBabel[currentChannel].Button.MIDIport)
        {
        case 1:
          MIDI1.sendControlChange(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
          break;
        case 2:
          MIDI2.sendControlChange(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
          break;
        case 3:
          MIDI3.sendControlChange(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
          break;
        }
      }
      if (MIDIBabel[currentChannel].Button.type == 2)
      {
        // Note
        if (MIDIBabel[currentChannel].Button.USBport > 0)
        {
          usbMIDI.sendNoteOn(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel, MIDIBabel[currentChannel].Button.USBport - 1);
          usbMIDI.send_now();
        }
        switch (MIDIBabel[currentChannel].Button.MIDIport)
        {
        case 1:
          MIDI1.sendNoteOn(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
          break;
        case 2:
          MIDI2.sendNoteOn(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
          break;
        case 3:
          MIDI3.sendNoteOn(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
          break;
        }
      }
    }

    if (encoderButton.risingEdge() && (MIDIBabel[currentChannel].Button.type > 0))
    {
      // Release event for this macro
      if (MIDIBabel[currentChannel].Button.type == 1)
      {
        // CC
        if (MIDIBabel[currentChannel].Button.USBport > 0)
        {
          usbMIDI.sendControlChange(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel, MIDIBabel[currentChannel].Button.USBport - 1);
          usbMIDI.send_now();
        }
        switch (MIDIBabel[currentChannel].Button.MIDIport)
        {
        case 1:
          MIDI1.sendControlChange(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
          break;
        case 2:
          MIDI2.sendControlChange(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
          break;
        case 3:
          MIDI3.sendControlChange(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
          break;
        }
      }

      if (MIDIBabel[currentChannel].Button.type == 2)
      {
        // Note
        if (MIDIBabel[currentChannel].Button.USBport > 0)
        {
          usbMIDI.sendNoteOff(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel, MIDIBabel[currentChannel].Button.USBport - 1);
          usbMIDI.send_now();
        }
        switch (MIDIBabel[currentChannel].Button.MIDIport)
        {
        case 1:
          MIDI1.sendNoteOff(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
          break;
        case 2:
          MIDI2.sendNoteOff(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
          break;
        case 3:
          MIDI3.sendNoteOff(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
          break;
        }
      }
    }
  }
  else
  {
    // Toggle mode
    if (encoderButton.fallingEdge() && (MIDIBabel[currentChannel].Button.type > 0))
    {
      if (MIDIBabel[currentChannel].Button.toggled)
      {
        if (MIDIBabel[currentChannel].Button.type == 1)
        {
          // CC
          if (MIDIBabel[currentChannel].Button.USBport > 0)
          {
            usbMIDI.sendControlChange(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel, MIDIBabel[currentChannel].Button.USBport - 1);
            usbMIDI.send_now();
          }
          switch (MIDIBabel[currentChannel].Button.MIDIport)
          {
          case 1:
            MIDI1.sendControlChange(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
            break;
          case 2:
            MIDI2.sendControlChange(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
            break;
          case 3:
            MIDI3.sendControlChange(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
            break;
          }
        }

        if (MIDIBabel[currentChannel].Button.type == 2)
        {
          // Note
          if (MIDIBabel[currentChannel].Button.USBport > 0)
          {
            usbMIDI.sendNoteOff(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel, MIDIBabel[currentChannel].Button.USBport - 1);
            usbMIDI.send_now();
          }
          switch (MIDIBabel[currentChannel].Button.MIDIport)
          {
          case 1:
            MIDI1.sendNoteOff(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
            break;
          case 2:
            MIDI2.sendNoteOff(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
            break;
          case 3:
            MIDI3.sendNoteOff(MIDIBabel[currentChannel].Button.number, 0, MIDIBabel[currentChannel].Button.channel);
            break;
          }
        }
        MIDIBabel[currentChannel].Button.toggled = false;
      }
      else
      {
        if (MIDIBabel[currentChannel].Button.type == 1)
        {
          //CC
          if (MIDIBabel[currentChannel].Button.USBport > 0)
          {
            usbMIDI.sendControlChange(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel, MIDIBabel[currentChannel].Button.USBport - 1);
            usbMIDI.send_now();
          }
          switch (MIDIBabel[currentChannel].Button.MIDIport)
          {
          case 1:
            MIDI1.sendControlChange(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
            break;
          case 2:
            MIDI2.sendControlChange(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
            break;
          case 3:
            MIDI3.sendControlChange(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
            break;
          }
        }

        if (MIDIBabel[currentChannel].Button.type == 2)
        {
          //note
          if (MIDIBabel[currentChannel].Button.USBport > 0)
          {
            usbMIDI.sendNoteOn(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel, MIDIBabel[currentChannel].Button.USBport - 1);
            usbMIDI.send_now();
          }
          switch (MIDIBabel[currentChannel].Button.MIDIport)
          {
          case 1:
            MIDI1.sendNoteOn(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
            break;
          case 2:
            MIDI2.sendNoteOn(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
            break;
          case 3:
            MIDI3.sendNoteOn(MIDIBabel[currentChannel].Button.number, MIDIBabel[currentChannel].Button.value, MIDIBabel[currentChannel].Button.channel);
            break;
          }
        }

        MIDIBabel[currentChannel].Button.toggled = true;
      }
    }
  }
}
int Deviation(int value, double deviation)
//https://stackoverflow.com/questions/41134365/formula-to-create-simple-midi-velocity-curves
{
  float minMidiValue = 0;
  float maxMidiValue = 127;
  float midMidiValue = 63.5;

  // This is our control point for the quadratic bezier curve
  // We want this to be between 0 (min) and 63.5 (max)
  double controlPointX = midMidiValue + ((deviation / 100) * midMidiValue);

  // Get the percent position of the incoming value in relation to the max
  double t = (double)value / maxMidiValue;

  // The quadratic bezier curve formula
  // B(t) = ((1 - t) * (1 - t) * p0) + (2 * (1 - t) * t * p1) + (t * t * p2)

  // t  = the position on the curve between (0 and 1)
  // p0 = minMidiValue (0)
  // p1 = controlPointX (the bezier control point)
  // p2 = maxMidiValue (127)

  // Formula can now be simplified as:
  // B(t) = ((1 - t) * (1 - t) * minMidiValue) + (2 * (1 - t) * t * controlPointX) + (t * t * maxMidiValue)

  // What is the deviation from our value?
  int delta = (int)round((2 * (1 - t) * t * controlPointX) + (t * t * maxMidiValue));
  return (value - delta) + value;
}

void EncoderProcessing()
{
  for (i = 0; i < 5; i++)
  {
    switch (MIDIBabel[currentChannel].Controller[i].type)
    {
    case 1:
      // CC mode
      val = map(MIDIBabel[currentChannel].encoderPosition, 0, maxEncoder, 0, 127);
      val = Deviation(val, (double)map(MIDIBabel[currentChannel].Controller[i].deviation, 0, 127, -100, 100));
      val = map(val, MIDIBabel[currentChannel].Controller[i].mapstart, MIDIBabel[currentChannel].Controller[i].mapend, MIDIBabel[currentChannel].Controller[i].lowvalue, MIDIBabel[currentChannel].Controller[i].highvalue);

      if (map(MIDIBabel[currentChannel].encoderPosition, 0, maxEncoder, 0, 127) > MIDIBabel[currentChannel].Controller[i].mapstart && map(MIDIBabel[currentChannel].encoderPosition, 0, maxEncoder, 0, 127) < MIDIBabel[currentChannel].Controller[i].mapend)
      {
        if (MIDIBabel[currentChannel].Controller[i].USBport > 0)
        {
          usbMIDI.sendControlChange(MIDIBabel[currentChannel].Controller[i].number, val, MIDIBabel[currentChannel].Controller[i].channel, MIDIBabel[currentChannel].Controller[i].USBport - 1);
          usbMIDI.send_now();
        }

        switch (MIDIBabel[currentChannel].Controller[i].MIDIport)
        {
        case 1:
          MIDI1.sendControlChange(MIDIBabel[currentChannel].Controller[i].number, val, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        case 2:
          MIDI2.sendControlChange(MIDIBabel[currentChannel].Controller[i].number, val, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        case 3:
          MIDI3.sendControlChange(MIDIBabel[currentChannel].Controller[i].number, val, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        }
      }

      break;

    case 2:
      // Note mode
      val = map(MIDIBabel[currentChannel].encoderPosition, 0, maxEncoder, 0, 127);

      if (val > MIDIBabel[currentChannel].Controller[i].mapstart && val < MIDIBabel[currentChannel].Controller[i].mapend && MIDIBabel[currentChannel].Controller[i].toggled == false)
      {
        if (MIDIBabel[currentChannel].Controller[i].USBport > 0)
        {
          usbMIDI.sendNoteOn(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].velocity, MIDIBabel[currentChannel].Controller[i].channel, MIDIBabel[currentChannel].Controller[i].USBport - 1);
          usbMIDI.send_now();
        }

        switch (MIDIBabel[currentChannel].Controller[i].MIDIport)
        {
        case 1:
          MIDI1.sendNoteOn(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].velocity, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        case 2:
          MIDI2.sendNoteOn(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].velocity, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        case 3:
          MIDI3.sendNoteOn(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].velocity, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        }

        MIDIBabel[currentChannel].Controller[i].toggled = true;
      }
      if ((val < MIDIBabel[currentChannel].Controller[i].mapstart || val > MIDIBabel[currentChannel].Controller[i].mapend) && MIDIBabel[currentChannel].Controller[i].toggled == true)
      {
        if (MIDIBabel[currentChannel].Controller[i].USBport > 0)
        {
          usbMIDI.sendNoteOff(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].velocity, MIDIBabel[currentChannel].Controller[i].channel, MIDIBabel[currentChannel].Controller[i].USBport - 1);
          usbMIDI.send_now();
        }

        switch (MIDIBabel[currentChannel].Controller[i].MIDIport)
        {
        case 1:
          MIDI1.sendNoteOff(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].velocity, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        case 2:
          MIDI2.sendNoteOff(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].velocity, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        case 3:
          MIDI3.sendNoteOff(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].velocity, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        }

        MIDIBabel[currentChannel].Controller[i].toggled = false;
      }

      break;

    case 3:
      // Program Change
      val = map(MIDIBabel[currentChannel].encoderPosition, 0, maxEncoder, 0, 127);
      if (val > MIDIBabel[currentChannel].Controller[i].mapstart && val < MIDIBabel[currentChannel].Controller[i].mapend && MIDIBabel[currentChannel].Controller[i].toggled == false)
      {
        if (MIDIBabel[currentChannel].Controller[i].USBport > 0)
        {
          usbMIDI.sendProgramChange(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].channel, MIDIBabel[currentChannel].Controller[i].USBport - 1);
          usbMIDI.send_now();
        }

        switch (MIDIBabel[currentChannel].Controller[i].MIDIport)
        {
        case 1:
          MIDI1.sendProgramChange(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        case 2:
          MIDI2.sendProgramChange(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        case 3:
          MIDI3.sendProgramChange(MIDIBabel[currentChannel].Controller[i].number, MIDIBabel[currentChannel].Controller[i].channel);
          break;
        }
        MIDIBabel[currentChannel].Controller[i].toggled = true;
      }

      if ((val < MIDIBabel[currentChannel].Controller[i].mapstart || val > MIDIBabel[currentChannel].Controller[i].mapend) && MIDIBabel[currentChannel].Controller[i].toggled == true)
      {
        MIDIBabel[currentChannel].Controller[i].toggled = false;
      }

      break;

    case 4:
      // NRPN
      // Not implemented
      break;

    case 5:
      // SynclavierMODE
      if (MIDIBabel[currentChannel].Controller[i].USBport > 0)
      {
        usbMIDI.sendPitchBend(500 * encoderAcc, MIDIBabel[currentChannel].Controller[i].channel, MIDIBabel[currentChannel].Controller[i].USBport - 1);
        usbMIDI.send_now();
      }

      switch (MIDIBabel[currentChannel].Controller[i].MIDIport)
      {
      case 1:
        MIDI1.sendPitchBend(500 * encoderAcc, MIDIBabel[currentChannel].Controller[i].channel);

        break;
      case 2:
        MIDI2.sendPitchBend(500 * encoderAcc, MIDIBabel[currentChannel].Controller[i].channel);

        break;
      case 3:
        MIDI3.sendPitchBend(500 * encoderAcc, MIDIBabel[currentChannel].Controller[i].channel);

        break;
      }
      analogWrite(ENCODER_LED_2, map(500 * encoderAcc, -8000, 8000, 0, 255));
      display.setTextColor(WHITE, BLACK);
      display.setCursor(40, 18);
      display.print("         ");
      display.setCursor(40, 18);
      display.println(50 * encoderAcc);
      display.display();
      ledOnMillis = 0;
      MIDIBabel[currentChannel].Controller[i].toggled = true;
      break;
    }
  }
}

void SysexMode()
{
  // Special Sysex MODE
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(BLACK, WHITE);
  display.print("Sysex Mod");
  display.display();
  upButton.update();
  downButton.update();

  //WAIT HERE:
  while (!upButton.fallingEdge())
  {
    upButton.update();

    if (usbMIDI.read())
    {
      byte type = usbMIDI.getType();
      //byte channel = usbMIDI.getChannel();
      //byte data1 = usbMIDI.getData1();
      //byte data2 = usbMIDI.getData2();
      //byte cable = usbMIDI.getCable();

      if (type == usbMIDI.SystemExclusive)
      {
        // SysEx messages are special.  The message length is given in data1 & data2
        //unsigned int SysExLength = data1 + data2 * 256;
        // Unpack SysEx
        byte *sysbytes = usbMIDI.getSysExArray();

        /*
        for (i = 0; i <= 16; i++) { 
        display.clearDisplay();
        display.setTextColor(WHITE, BLACK);
        display.setCursor(40,18);
        display.print("         ");
        display.setCursor(40,18);
        display.print(i);
        display.print(": ");
        display.print(sysbytes[i], HEX);
        display.display();
        delay (1000); 
        }
      */
        // Check for SysEx config message
        if (sysbytes[0] == 0xf0 && sysbytes[1] == 0x7d)
        { // check start f0 and ID 7D.
          //currentChannel = sysbytes[2];
          switch (sysbytes[3])
          {
          // Main parameters
          case 100:
            MIDIBabel[sysbytes[2]].name[0] = sysbytes[4];
            MIDIBabel[sysbytes[2]].name[1] = sysbytes[5];
            MIDIBabel[sysbytes[2]].name[2] = sysbytes[6];
            MIDIBabel[sysbytes[2]].name[3] = sysbytes[7];
            MIDIBabel[sysbytes[2]].name[4] = sysbytes[8];
            MIDIBabel[sysbytes[2]].name[5] = sysbytes[9];
            MIDIBabel[sysbytes[2]].name[6] = sysbytes[10];
            MIDIBabel[sysbytes[2]].name[7] = {'\0'};
            MIDIBabel[sysbytes[2]].MIDImerge = sysbytes[11];

            if (sysbytes[12] == 0x7f)
            {
              dataintegrity = true;
            } // check the last byte
            display.clearDisplay();
            display.setTextColor(WHITE, BLACK);
            display.setCursor(0, 0);
            display.println("OK: UP for exit");
            display.display();
            break;

          // Button configuration
          case 0:
            MIDIBabel[sysbytes[2]].Button.USBport = sysbytes[4];
            MIDIBabel[sysbytes[2]].Button.MIDIport = sysbytes[5];
            MIDIBabel[sysbytes[2]].Button.type = sysbytes[6];
            MIDIBabel[sysbytes[2]].Button.channel = sysbytes[7];
            MIDIBabel[sysbytes[2]].Button.number = sysbytes[8];
            MIDIBabel[sysbytes[2]].Button.value = sysbytes[9];
            MIDIBabel[sysbytes[2]].Button.push = sysbytes[10];

            if (sysbytes[11] == 0x7f)
            {
              dataintegrity = true;
            } // check the last byte
            display.clearDisplay();
            display.setTextColor(WHITE, BLACK);
            display.setCursor(0, 0);
            display.println("OK: UP for exit");
            display.display();
            break;

          // Controller configuration
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].USBport = sysbytes[4];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].MIDIport = sysbytes[5];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].type = sysbytes[6];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].channel = sysbytes[7];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].number = sysbytes[8];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].lowvalue = sysbytes[9];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].highvalue = sysbytes[10];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].mapstart = sysbytes[11];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].mapend = sysbytes[12];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].deviation = sysbytes[13];
            MIDIBabel[sysbytes[2]].Controller[sysbytes[3] - 1].velocity = sysbytes[14];

            if (sysbytes[15] == 0x7f)
            {
              dataintegrity = true;
            } // check the last byte
            display.clearDisplay();
            display.setTextColor(WHITE, BLACK);
            display.setCursor(0, 0);
            display.println("OK: UP for exit");
            //display.println(MIDIBabel[sysbytes[2]].Controller[sysbytes[3]-1].deviation);
            display.display();
            break;
          }
        }
      }
    }
  }
  display.clearDisplay();

  // Save ALL
  if (dataintegrity)
  {
    EPROMsave();
  }
  DisplayTitleUpdate();
  UpdateState();
  activity = true;
}
