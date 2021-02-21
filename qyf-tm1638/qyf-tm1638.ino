/*
   Using the QYF-TM1638 module with Arduino UNO
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
 
void setup()
{
  module.setupDisplay(true, 7);
  mode = 0;
}
 
void update(TM1638QYF* module, word* mode) {
  word buttons = module->getButtons();
 
  // button pressed - change mode
  if (buttons != 0) {
    *mode = buttons >> 1;
 
    if (*mode < 128) {
      module->clearDisplay();
      delay(100);
    }
  }
 
  switch (*mode) {
    case 0:
      module->setDisplayToString("press 1");
      break;
    case 1:
      module->setDisplayToString("press 2");
      break;
    case 2:
      module->setDisplayToString("press 3");
      break;
    case 4:
      module->setDisplayToString("press 4");
      break;
    case 8:
      module->setDisplayToString("press 5");
      break;
    case 16:
      module->setDisplayToString("press 6");
      break;
    case 32:
      module->setDisplayToString("press 7");
      break;
    case 64:
      module->setDisplayToString("press 8");
      break;
    case 128:
      module->setDisplayToString("press 9");
      break;
    case 256:
      module->setDisplayToString("press 10");
      break;
    case 512:
      module->setDisplayToString("press 11");
      break;
    case 1024:
      module->setDisplayToString("press 12");
      break;
    case 2048:
      module->setDisplayToString("press 13");
      break;
    case 4096:
      module->setDisplayToString("press 14");
      break;
    case 8192:
      module->setDisplayToString("press 15");
      break;
    case 16384:
      module->setDisplayToString("press 16");
      break;
    default:
      module->setDisplayToString("?????");
      break;
  }
}
 
void loop()
{
  update(&module, &mode);
}
