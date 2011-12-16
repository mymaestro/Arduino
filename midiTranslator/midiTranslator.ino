/*
 * midiTranslator.ino
 * 
 * Copyright 2011 Warren Gill <mymaestro@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 * Using the MIDI library found here
 * http://www.arduino.cc/playground/Main/MIDILibrary
 * Also uses the LCD bar graph library
 * http://arduino.cc/playground/Code/LcdBarGraph
 *
 * This program takes MIDI signals arriving on any channel and forwards them 
 * to the channel assigned as the output channel
 *
 */

#include <MIDI.h>
#include <LiquidCrystal.h>
#include <LcdBarGraph.h>

#define STATUS_LED 13 // LED pin on Arduino board
#define BACKLIGHT_CONTROL 6 // LCD backlight wired through a 2N2222 to this pin
#define POWERSAVE 15000 // How many milliseconds to keep the backlight on
#define LCD_COLUMNS 20 // Number of columns on the LCD
#define LCD_ROWS 4 // Number of rows on the LCD
#define MIDI_PANIC_TIME 1500 // How long a button press to invoke the MIDI panic
#define RESET_BUTTON_TIME 5000 // How long the button has been pressed to initiate a reset

#define encoder0PinA 2  // The rotary encoder is attached to pins 2 and 4
#define encoder0PinB 4  // and ground of course
const int buttonPin = 3; // My encoder has a pushbutton; it's attached to pin 3

int Pos, oldPos;
volatile unsigned int encoder0Pos = 0; // variables changed within interrupts are volatile
volatile boolean buttonPressed;
boolean buttonLast;
long buttonDown;
long buttonPressTime;
long powerSave;

int notesOn = 0; // How many notes are on
// Draw an eighth note on the LCD
byte quaver[8] = { 0x02, 0x02, 0x03, 0x02, 0x02, 0x0E, 0x1E, 0x0C };

char* theNoteNames[] = { "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};

// The three kinds of parameters you can change
char* midiParameterName[] = { "Channel: ",
                              "Program: ",
                              "Volume:  " };

// The sounds I have for each MIDI channel on my sound module
char* midiChannelMessages[] = {
  "MIDI Babel Fish 0.79",
  "01 Grand  ",
  "02 Hammond",
  "03 Rhodes ",
  "04 Wurly  ",
  "05 DX7    ",
  "06 Clavi  ",
  "07 Vibes  ",
  "08 Harmoni",
  "09 Miles  ",
  "10 Drums  ",
  "11 Jump   ",
  "12 Oohs   ",
  "13 Bass   ",
  "14 Squeeze",
  "15 Whistle",
  "16 Banjo  " };

// The MIDI instrument for each channel
// General MIDI values
//{ 0, 8, 3, 59, 40, 65, 73, 114, 33, 112, 5, 5, 79, 100, 118, 79, 106 };

// Values for Roland SonicCell
byte mProg[] = {
    0, // nothing
    8, //"01 Grand  "
    3, //"02 Hammond"
   59, //"03 Rhodes "
   40, //"04 Wurly  "
   65, //"05 DX7    "
   73, //"06 Clavi  "
  114, //"07 Vibes  "
   33, //"08 Harmoni"
  112,  //"09 Miles  "
    5, //"10 Drums  "
    5, //"11 Jump   "
   79, //"12 Oohs   "
  100, //"13 Bass   "
  118, //"14 Squeeze"
   79, //"15 Whistle"
  106, //"16 Banjo  "
  };

// Individual volume level for each channel
byte mVol[] = {
    0, // nothing
  100, //"01 Grand  "
  100, //"02 Hammond"
  100, //"03 Rhodes "
  100, //"04 Wurly  "
  100, //"05 DX7    "
  100, //"06 Clavi  "
  100, //"07 Vibes  "
  100, //"08 Harmoni"
  100, //"09 Miles  "
  100, //"10 Drums  "
  100, //"11 Jump   "
  100, //"12 Oohs   "
  100, //"13 Bass   "
  100, //"14 Squeeze"
  100, //"15 Whistle"
  100, //"16 Banjo  "
  };


int midiParameterIndex = 0;  // Which parameter is changing
int midiChannel = 1;        // User selected channel (1-16)
byte midiProgram[17];
byte midiVolume[17];

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
// The bar graph library uses LiquidCrystal
LcdBarGraph lbg(&lcd, LCD_COLUMNS);

void setup() {
  buttonPressed = false;
  buttonLast = false;
  // Pins for status LED and the LCD backlight
  pinMode(STATUS_LED, OUTPUT);
  pinMode(BACKLIGHT_CONTROL, OUTPUT);
  // Initiate pins for rotary encoder and button
  pinMode(encoder0PinA, INPUT);
  digitalWrite(encoder0PinA, HIGH);
  pinMode(encoder0PinB, INPUT);
  digitalWrite(encoder0PinB, HIGH);

  // Set up the rotary encoder
  attachInterrupt(0, doEncoder, CHANGE); // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(1, doButtonPress, CHANGE);      // button pin on interrupt 1 (pin 3)
  /* if this was set to RISING it would be able to tell if the button was really down
   * but the interrupt would have to detach and attach to FALLING, and vice versa
   */

  // Initiate MIDI communications, listen to all channels, turn off thru mode
  MIDI.begin(MIDI_CHANNEL_OMNI);
  // By default, the MIDI library sends everything THRU. We do NOT want that! 
  MIDI.turnThruOff();

  // Connect MIDI status changes involving a channel to handlers
  MIDI.setHandleNoteOn(HandleNoteOn);
  MIDI.setHandleNoteOff(HandleNoteOff);
  MIDI.setHandleAfterTouchPoly(HandleAfterTouchPoly);
  MIDI.setHandleControlChange(HandleControlChange);
  MIDI.setHandleProgramChange(HandleProgramChange);
  MIDI.setHandleAfterTouchChannel(HandleAfterTouchChannel);
  MIDI.setHandlePitchBend(HandlePitchBend);

  turnBacklightOn();
  // set up the LCD's number of columns and rows: 
  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.createChar(5, quaver); // it's a picture of a note
  
  // Print a message to the LCD.
  lcd.home();
  EnterSetupMenu();
}


void loop() {
  // Turn off the interrupt while scanning it
  uint8_t oldSREG = SREG;
  cli();
  Pos = encoder0Pos;
  SREG = oldSREG;
  // The rotary encoder has turned
  if(Pos != oldPos) HandleEncoderTurn();
  // The button changed since last loop
  if (buttonPressed != buttonLast) HandleButtonPress();
  // Turn the back light off after timeout
  if (millis() > powerSave) turnBacklightOff();
  MIDI.read();
}

void doBlink(int times) {
  for (int i = 0; i < times; i++ ) {
    digitalWrite(STATUS_LED, HIGH);
    delay(150);
    digitalWrite(STATUS_LED, LOW);
    delay(150);
  }
}

void doButtonPress()
{
  buttonPressed = !buttonPressed;
}

void HandleButtonPress() {
  if (buttonPressed != buttonLast) { // The button changed since last
    if (buttonPressed) {             // The button is down
      buttonDown = millis();         // Start button down timing
      lcd.setCursor(19, 2);
      lcd.write(5);
      turnBacklightOn();
    } else {                         // The button is up
      buttonPressTime = millis() - buttonDown; // How long the button was down
      lcd.setCursor(19, 2);
      lcd.print(" ");
      if ( Pos = oldPos ) {          // The encoder did not turn
        // If the button was pressed a long time but didn't turn, perform 
        //   the MIDI panic button or a reset, depending on how long
        if (buttonPressTime > RESET_BUTTON_TIME) {
          EnterSetupMenu();
        }    
        if ((buttonPressTime > MIDI_PANIC_TIME) && (buttonPressTime < RESET_BUTTON_TIME)) {
          midiPanicButton();
        }
      }
    }
    buttonLast = buttonPressed;
  }
}
void HandleEncoderTurn() {
  turnBacklightOn();
  if(!buttonPressed) { // encoder is turning but not pressed in
    switch (midiParameterIndex) {
      case 0: // Channel
        if(Pos > oldPos) { // turning clockwise
          midiChannel++;
          if (midiChannel > 16) midiChannel = 1;
        } else { // turning counterclockwise
            midiChannel--;
            if (midiChannel < 1) midiChannel = 16;
        }
        displayChannelMessage(midiChannel);
        displayProgramMessage(midiProgram[midiChannel]);
        break;
      case 1: // Program
        if (Pos > oldPos) { // turning clockwise
          midiProgram[midiChannel]++;
          if (midiProgram[midiChannel] > 128) midiProgram[midiChannel] = 1;
        } else { // turning counterclockwise
          midiProgram[midiChannel]--;
          if (midiProgram[midiChannel] < 1) midiProgram[midiChannel] = 128;
        }
        MIDI.sendProgramChange(midiProgram[midiChannel] - 1, midiChannel);
        displayProgramMessage(midiProgram[midiChannel]);
        break;

      case 2: // Volume
        if (Pos > oldPos) { // turning clockwise
          midiVolume[midiChannel]++;
          if (midiVolume[midiChannel] > 127) midiVolume[midiChannel] = 0;
        } else {
          midiVolume[midiChannel]--;
          if (midiVolume[midiChannel] < 0) midiVolume[midiChannel] = 127;
        }
        MIDI.sendControlChange(0x07, midiVolume[midiChannel], midiChannel);
        displayVolumeMessage(midiVolume[midiChannel]);
        lcd.setCursor(0, 3);
        lbg.drawValue(midiVolume[midiChannel], 127);
        break;
      }
      delay(20);
    } else { // button being held down
      lcd.setCursor(0, midiParameterIndex);
      lcd.print(" ");
      if(Pos > oldPos) { // turning clockwise
        midiParameterIndex++;
        if (midiParameterIndex > 2) midiParameterIndex = 0;
      } else { // turning counterclockwise
        midiParameterIndex--;
        if (midiParameterIndex < 0) midiParameterIndex = 2;
      }
      lcd.setCursor(0, midiParameterIndex);
      lcd.write(5);
      delay(20);
    }
  oldPos = Pos;
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
    MIDI.sendControlChange(120, 0, channel);
    MIDI.sendControlChange(123, 0, channel);
  }
  notesOn = 0;
}

void turnBacklightOn() {
  powerSave = millis() + POWERSAVE;
  digitalWrite(BACKLIGHT_CONTROL, HIGH);
}

void turnBacklightOff() {
  digitalWrite(BACKLIGHT_CONTROL, LOW);
}

void EnterSetupMenu() {
  // Clear the screen and print the welcome message
  lcd.clear();
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print(midiChannelMessages[0]);
  lcd.setCursor(0, 1);
  lcd.print("Turn: change setting");
  lcd.setCursor(0, 2);
  lcd.print("Press+turn:");
  lcd.setCursor(0, 3);
  lcd.print("   Change parameter ");
  // Rest the indices, channel, volume, and program settings
  midiParameterIndex = 0;
  midiChannel = 1;
  memcpy(midiVolume, mVol, sizeof mVol[0] * 17);
  memcpy(midiProgram, mProg, sizeof mProg[0] * 17);
  
  // Wait 2 seconds to read the screen
  delay(2000);
  lcd.clear();
  lcd.write(5);
  lcd.setCursor(1, 0);
  displayChannelMessage(midiChannel);
  displayProgramMessage(midiProgram[midiChannel]);
  displayVolumeMessage(midiVolume[midiChannel]);
  buttonPressed = false;
}

void displayChannelMessage(int messageNum) {
  lcd.setCursor(1, 0);
  lcd.print(midiParameterName[0]);
  lcd.print(midiChannelMessages[messageNum]);
}

void displayProgramMessage(int midiProgram) {
  lcd.setCursor(1, 1);
  lcd.print(midiParameterName[1]);
  lcd.print(midiProgram);
  lcd.print("  ");
}

void displayVolumeMessage(int midiVolume) {
  lcd.setCursor(1, 2);
  lcd.print(midiParameterName[2]);
  lcd.print(midiVolume);
  lcd.print(" ");
}


/* See this expanded function to get a better understanding of the
 * meanings of the four possible (pinA, pinB) value pairs:
 */
void doEncoder(){
  delay(4);
  if (digitalRead(encoder0PinA) == HIGH) {   // found a low-to-high on channel A
    if (digitalRead(encoder0PinB) == LOW) {  // check channel B to see which way
      // encoder is turning
      encoder0Pos = encoder0Pos - 1;         // CCW
    } 
    else {
      encoder0Pos = encoder0Pos + 1;         // CW
    }
  }
  else                                        // found a high-to-low on channel A
  { 
    if (digitalRead(encoder0PinB) == LOW) {   // check channel B to see which way
      // encoder is turning  
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }

  }
  
}

void HandleNoteOn(byte channel, byte pitch, byte velocity) {
  MIDI.sendNoteOn(pitch, velocity, midiChannel);
  notesOn++;
  int noteIndex = (pitch - 12) % 12;
  int noteOctave = (pitch / 12) - 1;
  turnBacklightOn();  // Turn the light on when a MIDI key is pressed
  lcd.setCursor(0, 3);
  lbg.drawValue(velocity, 127);
  lcd.setCursor(0, 3);
  lcd.print("[");
  lcd.print(theNoteNames[noteIndex]);
  lcd.print(noteOctave);
  lcd.print("]");
  if (velocity == 0) {
    digitalWrite(STATUS_LED, LOW);
  } 
  else {
    digitalWrite(STATUS_LED, HIGH);
  }
}
void HandleNoteOff(byte channel, byte pitch, byte velocity) {
  MIDI.sendNoteOff(pitch, velocity, midiChannel);
  notesOn--;
  if ( notesOn <= 0 ) {
    digitalWrite(STATUS_LED, LOW);
    lcd.setCursor(0, 3);
    lbg.drawValue(0, 127);
  }
}
void HandleAfterTouchPoly(byte channel, byte note, byte pressure) {
  MIDI.sendAfterTouch(pressure, midiChannel);
}
void HandleControlChange(byte channel, byte number, byte value) {
  MIDI.sendControlChange(number, value, midiChannel);
}
void HandleProgramChange(byte channel, byte number) {
  MIDI.sendProgramChange(number, midiChannel);
}
void HandleAfterTouchChannel(byte channel, byte pressure) {
  MIDI.sendAfterTouch(pressure, midiChannel);
}
void HandlePitchBend(byte channel, int bend) {
  MIDI.sendPitchBend (bend, midiChannel);
}



