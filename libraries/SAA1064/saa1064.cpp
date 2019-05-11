/*
 * saa1064.cpp
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
 *
 * Thanks to John Boxall at tronixstuff
 * Example 39.3 - NXP SAA1064 I2C LED Driver IC Demo III
 * Displaying numbers on command
 * http://tronixstuff.wordpress.com/tutorials > chapter 39
 * John Boxall July 2011 | CC by-sa-nc
 */

#include <Wire.h>
#include <avr/pgmspace.h>
#include "Arduino.h"
#include "saa1064.h"

// Byte representations of pins required to display each digit 0~9
// then A~F, and blank digit
int SAA1064::_digits[]={ 63, 6, 91, 79, 102, 109, 125,7, 127, 111, 119, 124, 57, 94, 121, 113, 0};
uint8_t SAA1064::_brightness[]={
  (byte)B00000111, // 0ma
  (byte)B00010111, // 3ma
  (byte)B00100111, // 6ma
  (byte)B00110111, // 9ma
  (byte)B01000111, // 12ma
  (byte)B01100111, // 18ma
  (byte)B01110111  // 21ma
};

void SAA1064::begin(uint8_t ioaddr, int brightness) {
  // define the I2C address for the SAA1064 
  _addr = ioaddr >> 1; 

  if (brightness > 6) brightness = 6;
 
  Wire.begin(); // Start the I2C bus
  Wire.beginTransmission(_addr);
  Wire.write((byte)B00000000); // this is the instruction byte.
  // Zero means the next byte is the control byte

  // control byte (dynamic mode on, digits 1+3 on, digits 2+4 on, 
  //   9mA segment current
  // Wire.write((byte)B00110111); // 9ma
  Wire.write(_brightness[brightness]);
  Wire.endTransmission();
}

void SAA1064::begin(uint8_t ioaddr) {
	begin(ioaddr, 2);
}
    
void SAA1064::begin() {
	begin(0x70, 2);
}

void SAA1064::clear() {
  Wire.beginTransmission(_addr);
  Wire.write(0x01); // start with digit 1 (right-hand side)
  Wire.write((byte)0); // blank digit 1
  Wire.write((byte)0); // blank digit 2
  Wire.write((byte)0); // blank digit 3
  Wire.write((byte)0); // blank digit 4
  Wire.endTransmission();
}

void SAA1064::setBrightness(int brightness) {
  if (brightness > 6) brightness = 6;
  Wire.beginTransmission(_addr);
  Wire.write((byte)B00000000); // this is the instruction byte.
  Wire.write(_brightness[brightness]);
  Wire.endTransmission();
}

/*
 displayInt
 displays the number 'num'
 zero = 0 - no leading zero
 zero = 1 - leading zero
 */
void SAA1064::displayInt(int num, int zero) {
  int thousand, hundred, ten, one;
  // breakdown number into columns
  thousand = num/1000;
  hundred = (num-(thousand*1000))/100;
  ten = (num-((thousand*1000)+(hundred*100)))/10;
  one = num-((thousand*1000)+(hundred*100)+(ten*10));
  if (zero==1)  { // yes to leading zero
    Wire.beginTransmission(_addr);
    Wire.write(0x01);
    Wire.write(_digits[one]);
    Wire.write(_digits[ten]);
    Wire.write(_digits[hundred]);
    Wire.write(_digits[thousand]);
    Wire.endTransmission();
    delay(10);
  } else
    if (zero==0) { // no to leading zero
      if (thousand==0) { 
        thousand=16; 
      }
      if (hundred==0 && num<100) { 
        hundred=16; 
      }
      if (ten==0 && num<10) { 
        ten=16; 
      }
      Wire.beginTransmission(_addr);
      Wire.write(1);
      Wire.write(_digits[one]);
      Wire.write(_digits[ten]);
      Wire.write(_digits[hundred]);
      Wire.write(_digits[thousand]);
      Wire.endTransmission();
      delay(10);
    }
}
