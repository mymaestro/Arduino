/*
   ICM7218.h
 
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



	Public methods:
		writeFloat(float number, int decimalPositions)
		writeInt(int number)
		writeNum(int number, int decimalPosition)
		clear()

	Example declaration (pinout):
		ICM7218 myDisplay(id0,id1,id2,id3,-id7-,write,mode); 
		
		If you add a second display, 
		  all pins can be shared (including data pins!) 
 		  except for -id7- (must be unique for each display)


*/

#ifndef ICM7218_h
#define ICM7218_h

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <inttypes.h>
#include "Print.h"

//Timing values for interface
#define WRITEDELAY 20
#define SMALLDELAY 7

// These might be tied to hardware
#define MODE_PIN 10
#define WRITE_PIN 11

#define DISPLAY_DIGITS 8
#define SCROLL_DELAY 240

#define DISPLAY_ENABLE 1<<4
#define DECODE_HEX 1<<6
#define NO_DECODE 1<<5
#define SEQUENTIAL_DATA 1<<7

class ICM7218 : public Print {
public:
  ICM7218(uint8_t id0, uint8_t id1, uint8_t id2,
                   uint8_t id3, uint8_t id4, uint8_t id5,
                   uint8_t id6, uint8_t id7);
  void begin();
  void writeBuffer(char*, int);
  void writeText(char*);
  void writeFloat(float, int) ;
  void writeNumber(long);
  // virtual uint8_t write(uint8_t) = 0;

  // virtual size_t write(uint8_t) = 0;
  uint8_t write(uint8_t *buffer);
  size_t write(uint8_t);
  size_t write(const char *str) { return write((const uint8_t *)str, strlen(str)); }
  virtual size_t write(const uint8_t *buffer, size_t size);
   
  size_t print(char);
  size_t print(unsigned char, int = DEC);
  size_t print(int, int = DEC);
  size_t print(unsigned int, int = DEC);
  size_t print(long, int = DEC);
  size_t print(unsigned long, int = DEC);
  size_t print(double, int = 2);
  
  size_t println(char);
  size_t println(unsigned char, int = DEC);
  size_t println(int, int = DEC);
  size_t println(unsigned int, int = DEC);
  size_t println(long, int = DEC);
  size_t println(unsigned long, int = DEC);
  size_t println(double, int = 2);
  
private:
  void writePulse();
  void sendBytes(unsigned char, int, boolean);
  void sendCommand(unsigned char, unsigned char, int);
  void printError(void);

  int ID_PIN[];
  uint8_t ID0_PIN;
  uint8_t ID1_PIN;
  uint8_t ID2_PIN;
  uint8_t ID3_PIN;
  uint8_t ID7_PIN;
  
  char buf[DISPLAY_DIGITS + 1];

  unsigned char segments[];

  unsigned char strData;
  uint8_t dPoints;
  uint8_t i;
  uint8_t j;

};


#endif
