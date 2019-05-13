/*
  ICM7218.h - Library for Arduino to drive ICM7218 LED number displays
  Copyright (c) 2011 Jeff Heidel.  All rights reserved.

  This library should work with most Arduino varients, though it has only
  been tested on the MEGA.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

    This library was based off the following datasheet:
    http://www.intersil.com/data/fn/fn3159.pdf



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

#include <inttypes.h>
#include "Print.h"
#include <WProgram.h>


//Timing values for interface
#define WRITEDELAY 20
#define SMALLDELAY 7


class ICM7218 
{
public:
  ICM7218(uint8_t id0, uint8_t id1,
		uint8_t id2, uint8_t id3, uint8_t id7, uint8_t wr, uint8_t md);

  void writeFloat(float, int);
  void writeInt(int);
  void writeNum(int, int);
  void clear();

private:
  void write4bits(uint8_t);
  void controlWord();

  uint8_t _wr;
  uint8_t _md;
  uint8_t _id7;
  uint8_t _data_pins[4];

};


#endif
