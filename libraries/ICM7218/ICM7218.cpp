/*
   ICM7218.cpp
 
 * Arduino Library for Intersil ICM7218 7-segment LED driver
 * 
 * Copyright 2012 Warren Gill <mymaestro@gmail.com>
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
 */

#include "ICM7218.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

static const uint8_t segments[] = {
//  unsigned char segments[] = {
  /* Control chars */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  /* Symbols and numbers */
  0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x20, /*  !"#$%&' */
  0x00, 0x00, 0x00, 0x00, 0x11, 0x04, 0x00, 0x2c, /* ()*+,-./ */
  0x7b, 0x30, 0x6d, 0x75, 0x36, 0x57, 0x5f, 0x70, /* 01234567 */
  0x7f, 0x77, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, /* 89:;<=>? */

  /* Uppercase */
  0x6f, 0x7e, 0x1f, 0x4b, 0x3d, 0x4f, 0x4e, 0x5b, /* @ABCDEFG */
  0x3e, 0x10, 0x39, 0x5e, 0x0b, 0x58, 0x1c, 0x1d, /* HIJKLMNO */
  0x6e, 0x67, 0x0c, 0x53, 0x0f, 0x3b, 0x19, 0x23, /* PQRSTUVW */
  0x5c, 0x37, 0x69, 0x00, 0x16, 0x00, 0x00, 0x01, /* XYZ[\]^_ */

  /* Lowercase */
  0x06, 0x7e, 0x1f, 0x4b, 0x3d, 0x4f, 0x4e, 0x5b, /* `abcdefg */
  0x3e, 0x10, 0x39, 0x5e, 0x0b, 0x58, 0x1c, 0x1d, /* hijklmno */
  0x6e, 0x67, 0x0c, 0x53, 0x0f, 0x3b, 0x19, 0x23, /* pqrstuvw */
  0x5c, 0x37, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, /* xyz{|}~  */
};

// Constructor
ICM7218::ICM7218(uint8_t id0, uint8_t id1, uint8_t id2,
                 uint8_t id3, uint8_t id4, uint8_t id5,
                 uint8_t id6, uint8_t id7) {
					 
// map pins to ID0, ID1, ID2, ID3, ID4, ID5, ID6, ID7
int ID_PIN[] = {
  id0, id1, id2, id3, id4, id5, id6, id7 };
  
    char buf[DISPLAY_DIGITS + 1];

  unsigned char strData;
  int dPoints = 0;

  int i, j;
}

/***** public functions *****/
void ICM7218::begin() {
  pinMode(WRITE_PIN, OUTPUT);
  digitalWrite(WRITE_PIN, HIGH);
  pinMode(MODE_PIN, OUTPUT);

  for (int i=0; i<8; i++) {
    pinMode(ID_PIN[i], OUTPUT);
    digitalWrite(ID_PIN[i], HIGH);
  }
}

uint8_t ICM7218::write(uint8_t *buffer) {
  /*
   * code to display a letter when given the ASCII code for it
   */
}

void ICM7218::writeNumber(long n) {
  Serial.print("Writing number = ");
  Serial.println(n);
  if ( n < 100000000 ) {
    unsigned char digit[8];
    for (long power=1, i=0; i<8; i++, power*=10) {
      digit[i] = (n/power % 10) | 0x80;
    }
    sendCommand(DISPLAY_ENABLE | DECODE_HEX | SEQUENTIAL_DATA, digit, 8);
    delay(50);
  }
}

void ICM7218::writeBuffer(char buffer[DISPLAY_DIGITS + 1], int decimalPoints) {
  // No decode mode digits from segments font
  for (int digit=0; digit<DISPLAY_DIGITS; digit++) {
    strData = buffer[7 - digit];
    strData = 0x80 ^ ((strData & 0x80) | (segments[strData & 0x7f])); // 0x00 for decimal points
    sendCommand(digit | DISPLAY_ENABLE | NO_DECODE, &strData, 1);
    delay(50);
  }
}

void ICM7218::writeText(char *currentString) {
  // Scrolling long string  
  int l = strlen(currentString);
  if ( l > DISPLAY_DIGITS ) {

    char *bufferString = "";
    strcat(bufferString, currentString);

    for (i = 0; bufferString[i] != NULL; i++){
      strncpy(buf, bufferString, DISPLAY_DIGITS);
      Serial.println(bufferString);
      writeBuffer(buf, 0);
      delay(SCROLL_DELAY);

      strData = bufferString[0];
      for (j = 1; bufferString[j] != NULL; j++)
        bufferString[j - 1] = bufferString[j];
      bufferString[j-1] = strData;
    }
  } else {
    for (int i = 0; i < DISPLAY_DIGITS; i++)
      buf[i] = ' '; 
    strncpy(buf, currentString, DISPLAY_DIGITS);
    writeBuffer(buf, 0);
  }
}

// writes float to the screen rounded to the precision value (0 = ones, 1 = tenths, 2 = hundredths, etc)
void ICM7218::writeFloat(float digit, int precision) {
  // mult = 10 ^ precision (pow avoided to prevent import of math library)
  int mult = 1;
  for(int i = 0; i < precision; i++){
    mult *= 10;
  }
  digit *= mult;
  if(digit > 0){
    digit += 0.5; //rounding positive
  } else {
    digit -= 0.5; //rounding negative
  }

  int output = (int) digit;

 // writeNumber(output, precision);
  writeNumber(output);
} 


/***** private functions ***/

void ICM7218::writePulse() {
  digitalWrite(WRITE_PIN, LOW);
  // 250ns delay not explicitly required
  digitalWrite(WRITE_PIN, HIGH);
}

void ICM7218::sendBytes(unsigned char data[], int length, boolean mode)
{
  digitalWrite(MODE_PIN, mode);
  for (int i=0; i<length; i++) {
    for (int c=0; c<8; c++) {
      digitalWrite(ID_PIN[c], data[i] & 1<<c);
    }
    writePulse();
  }
}

void ICM7218::sendCommand(unsigned char control, unsigned char digit[], int length)
{
  sendBytes(&control, 1, 1);
  sendBytes(digit, length, 0);
}


ICM7218::ICM7218(void) {
  position = 0;
}

void ICM7218::print(unsigned long n, int base)
{
  if (base == 0) write(n);
  else printNumber(n, base);
}

void ICM7218::print(char c, int base)
{
  print((long) c, base);
}

void ICM7218::print(unsigned char b, int base)
{
  print((unsigned long) b, base);
}

void ICM7218::print(int n, int base)
{
  print((long) n, base);
}

void ICM7218::print(unsigned int n, int base)
{
  print((unsigned long) n, base);
}

void  ICM7218::println(void) {
  position = 0;
}

void  ICM7218::println(char c, int base)
{
  print(c, base);
  println();
}

void  ICM7218::println(unsigned char b, int base)
{
  print(b, base);
  println();
}

void  ICM7218::println(int n, int base)
{
  print(n, base);
  println();
}

void  ICM7218::println(unsigned int n, int base)
{
  print(n, base);
  println();
}

void  ICM7218::println(long n, int base)
{
  print(n, base);
  println();
}

void  ICM7218::println(unsigned long n, int base)
{
  print(n, base);
  println();
}

void  ICM7218::println(double n, int digits)
{
  print(n, digits);
  println();
}

void  ICM7218::print(double n, int digits)
{
  printFloat(n, digits);
}


size_t ICM7218::write(uint8_t c) {

  if (c == '\n') position = 0;
  if (c == '\r') position = 0;

  if ((c >= '0') && (c <= '9'))
    writeDigitNum(position, c-'0');

  position++;
  if (position == 2) position++;
}

void ICM7218::writeDigitRaw(uint8_t d, uint8_t bitmask) {
  if (d > 4) return;
  displaybuffer[d] = bitmask;
}

void ICM7218::drawColon(boolean state) {
  if (state)
    displaybuffer[2] = 0xFF;
  else
    displaybuffer[2] = 0;
}

void ICM7218::writeDigitNum(uint8_t d, uint8_t num, boolean dot) {
  if (d > 4) return;

  writeDigitRaw(d, numbertable[num] | (dot << 7));
}

void ICM7218::print(long n, int base)
{
  printNumber(n, base);
}

void ICM7218::printNumber(long n, uint8_t base)
{
    printFloat(n, 0, base);
}

void ICM7218::printFloat(double n, uint8_t fracDigits, uint8_t base) 
{ 
  uint8_t numericDigits = 4;   // available digits on display
  boolean isNegative = false;  // true if the number is negative
  
  // is the number negative?
  if(n < 0) {
    isNegative = true;  // need to draw sign later
    --numericDigits;    // the sign will take up one digit
    n *= -1;            // pretend the number is positive
  }
  
  // calculate the factor required to shift all fractional digits
  // into the integer part of the number
  double toIntFactor = 1.0;
  for(int i = 0; i < fracDigits; ++i) toIntFactor *= base;
  
  // create integer containing digits to display by applying
  // shifting factor and rounding adjustment
  uint32_t displayNumber = n * toIntFactor + 0.5;
  
  // calculate upper bound on displayNumber given
  // available digits on display
  uint32_t tooBig = 1;
  for(int i = 0; i < numericDigits; ++i) tooBig *= base;
  
  // if displayNumber is too large, try fewer fractional digits
  while(displayNumber >= tooBig) {
    --fracDigits;
    toIntFactor /= base;
    displayNumber = n * toIntFactor + 0.5;
  }
  
  // did toIntFactor shift the decimal off the display?
  if (toIntFactor < 1) {
    printError();
  } else {
    // otherwise, display the number
    int8_t displayPos = 4;
    
    for(uint8_t i = 0; displayNumber; ++i) {
      boolean displayDecimal = (fracDigits != 0 && i == fracDigits);
      writeDigitNum(displayPos--, displayNumber % base, displayDecimal);
      if(displayPos == 2) writeDigitRaw(displayPos--, 0x00);
      displayNumber /= base;
    }
  
    // display negative sign if negative
    if(isNegative) writeDigitRaw(displayPos--, 0x40);
  
    // clear remaining display positions
    while(displayPos >= 0) writeDigitRaw(displayPos--, 0x00);
  }
}

void ICM7218::printError(void) {
  for(uint8_t i = 0; i < SEVENSEG_DIGITS; ++i) {
    writeDigitRaw(i, (i == 2 ? 0x00 : 0x40));
  }
}
