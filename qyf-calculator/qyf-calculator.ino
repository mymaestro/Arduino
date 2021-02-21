/*
   Using the QYF-TM1638 module with Arduino UNO to create a four-function calculator
   Connect DIO, CLK, and STB to digital pins
   Connect VCC to 5V
   Connect GND to GND
*/
#include <TM1638.h>
#include <TM1638QYF.h>

/* TM1638QYF dataPin, clockPin, strobePin */
const int DIOPIN = 8;  // TM1638QYF data pin "DIO"
const int CLKPIN = 9;  // TM1638QYF clock pin "CLK"
const int STBPIN = 10; // TM1638QYF strobe pin "STB"

TM1638QYF module(DIOPIN, CLKPIN, STBPIN);
word mode;
word buttons, lastbuttons; 
unsigned long inputNum;

/*
const byte ROWS = 4;
const byte COLS = 4;

char keys [ROWS] [COLS] = {
  {'7', '8', '9', '/'},
  {'4', '5', '6', '*'},
  {'1', '2', '3', '-'},
  {'0', '.', '=', '+'}
};
byte rowPins[ROWS] = {13,12,11,10};
byte colPins[COLS] = {9,8,7,6};
*/

void setup()
{
  Serial.begin(115200);
  module.setupDisplay(true, 7);
  buttons = module.getButtons();
  lastbuttons = buttons;
  mode = 0;
  inputNum = 0;
}
 
void loop()
{
  buttons = module.getButtons();
  if (buttons != lastbuttons) {
    // do we need debounce?
    if (buttons > lastbuttons) {
      // Button down
      Serial.println("keypress");
      keypress(buttons - lastbuttons);
      module.setDisplayToSignedDecNumber(inputNum, 1 << 7, false);
    } else {
      // Button up
      Serial.println("key release");
    }
    lastbuttons = buttons;
  }
}

void catnumber(int incr) {
  Serial.print("Button ");
  Serial.println(inputNum);
  Serial.println(incr);
  if (inputNum == 0) {
    inputNum = incr;
    } else {
    inputNum = (inputNum * 10) + incr;
  }
  Serial.println(inputNum);
}

void keypress(word key) { 
  switch (key) {
    case 0: // I don't know
      Serial.println("unk");
      break;
    case 1: // Button 1
      catnumber(7); 
      break;
    case 2: // Button 2
      catnumber(8);
      break;
    case 4: // Button 9
      catnumber(9);
      break;
    case 8: // div
      Serial.println("button /");
      module.setDisplayToString("div");
      delay(2);
      break;
    case 16: // 4
      catnumber(4);
      break;
    case 32: // 5
      catnumber(5);
      break;
    case 64: // 6
      catnumber(6);
      break;
    case 128: // *
      Serial.println("button *");
      module.setDisplayToString("mult");
      delay(2);
      module.setDisplayToString("multiply");
      break;
    case 256: // 1
      catnumber(1);
      break;
    case 512: // 2
      catnumber(2);
      break;
    case 1024: // 3
      catnumber(3);
      break;
    case 2048: // -
      Serial.println("button -");
      module.setDisplayToString("subtract");
      delay(2);
      break;
    case 4096: // 0
      catnumber(0);
      break;
    case 8192: // .
      Serial.println("button .");
      module.setDisplayToString("decimal");
      delay(2);
      break;
    case 16384: // =
      Serial.println("button =");
      module.setDisplayToString("equal");
      delay(2);
      break;
    case 32768: // +
      Serial.println("button +");
      module.setDisplayToString("add");
      delay(2);
      break;
    default:
      module.setDisplayToString("?????");
      break;
  }
}
