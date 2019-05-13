/*
  ICM7218.cpp - Library for Arduino to drive ICM7218 LED number displays
  Copyright (c) 2011 Jeff Heidel.  All rights reserved.
*/

#include "ICM7218.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "WProgram.h"

//constructor
  ICM7218::ICM7218(uint8_t id0, uint8_t id1,
		uint8_t id2, uint8_t id3, uint8_t id7, uint8_t wr, uint8_t md)
{
	_wr = wr;
	_md = md;
	_id7 = id7;
	_data_pins[0] = id0;
	_data_pins[1] = id1;
	_data_pins[2] = id2;
	_data_pins[3] = id3; 

	pinMode(_wr, OUTPUT);
	digitalWrite(_wr, HIGH);
	pinMode(_md, OUTPUT);
	pinMode(_id7, OUTPUT);
}

/***** public functions *****/

//clears display
void ICM7218::clear(){
  writeNum(0, -1);
}

//writes float to the screen rounded to the precision value (0 = ones, 1 = tenths, 2 = hundredths, etc)
void ICM7218::writeFloat(float x, int precision){
  // mult = 10 ^ precision (pow avoided to prevent import of math library)
  int mult = 1;
  for(int i = 0; i < precision; i++){
    mult *= 10;
  }  
  
  x *= mult;

  if(x>0){
    x += 0.5; //rounding positive
  }else{
    x -= 0.5; //rounding negative
  }

  int output = (int) x;

  writeNum(output, precision);

} 

//writes the given integer to the screen
void ICM7218::writeInt(int x){
  writeNum(x, 0);
}


//writes integer x to the display with decimal at dlocation (0 = after least significant value) (use -1 for no decimal)
void ICM7218::writeNum(int x, int dlocation){
  boolean negativenum = false;
  boolean past = false;

  if(x<0){ //support for negative numbers
    x *= -1;
    negativenum = true;
  }

  //construct int array
  uint8_t intAr[8];

  //places integer into int array
  for(int i = 0; i<8; i++){

    if(x!=0){
      intAr[i] = x % 10;
      x /= 10;    
    }
    else{
      //if we're still below decimal level
      if(i<= dlocation){
        intAr[i] = 0; //zero
      }
      else{
        //adds dash if this number is negative
        if(negativenum){
          intAr[i] = 10; //dash
          negativenum = false;
        }
        else{
          intAr[i] = 15; //blank
        }
      }
    }
  }

  controlWord();

  for(int k = 0; k<8; k++){

    write4bits(intAr[k]);
    if(k==dlocation){
      digitalWrite(_id7, LOW); //decimal
    }
    else{
      digitalWrite(_id7, HIGH); //decimal
    }
    delayMicroseconds(SMALLDELAY);
    digitalWrite(_wr, LOW);
    delayMicroseconds(WRITEDELAY);
    digitalWrite(_wr, HIGH);
    delayMicroseconds(WRITEDELAY);

  }

  digitalWrite(_id7, LOW); //disable incoming data

}


/***** private functions ***/

//writes bits to the data ports
void ICM7218::write4bits(uint8_t value){
  for (int i = 0; i < 4; i++) {
    pinMode(_data_pins[i], OUTPUT);
    digitalWrite(_data_pins[i], (value >> i) & 0x01);
  } 
}

//writes the control word sequence
void ICM7218::controlWord(){
  //write control word
  digitalWrite(_md, HIGH);
  digitalWrite(_id7, HIGH); //enable incoming data
  delayMicroseconds(SMALLDELAY);
  digitalWrite(_wr, LOW);
  delayMicroseconds(WRITEDELAY);
  digitalWrite(_wr, HIGH);
  delayMicroseconds(WRITEDELAY);
  digitalWrite(_md, LOW);
  delayMicroseconds(WRITEDELAY);
}
