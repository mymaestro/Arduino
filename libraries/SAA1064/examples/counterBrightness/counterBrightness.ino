/*
 * Write to 7-segment LEDs using the SAA1064
 */
#include "Wire.h" // enable I2C bus
#include <saa1064.h>
byte addr = 0x70; // Chip select address (pin 1 to GND)
int brightness = 2; // 0 = off, 6 = bright (21ma)
#define INPUT A0

SAA1064 saa1064;

void setup() {
  saa1064.begin(addr, brightness);
  pinMode(INPUT, input);
}

void loop() {
  saa1064.clear();
  for (int i=0; i<10000; i++) {
    saa1064.displayInt(i, 0);
    delay(125);
  }
}

