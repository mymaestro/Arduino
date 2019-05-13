///////////////////////////////////////////////////////////////
//
//  LEDMatrix7219.h
//
//  Class for using an 8x8 LED matrix with a MAX7219 controller
//

#include "LEDMatrix7219.h"

// Bit mask from high-order to low-order bit
byte bitMask[8] = {B10000000, B01000000, B00100000, B00010000, B00001000, B00000100, B00000010, B00000001};

// Default Constructor
LEDMatrix7219::LEDMatrix7219 ()
{
  init (DEFAULT_NUM_DEVICES, DEFAULT_LED_INTENSITY, DEFAULT_PIN_DATA_IN, DEFAULT_PIN_CS, DEFAULT_PIN_CLK);
}

// Default Constructor w/ Number of Matricies
LEDMatrix7219::LEDMatrix7219 (byte n)
{ 
  init (n, DEFAULT_LED_INTENSITY, DEFAULT_PIN_DATA_IN, DEFAULT_PIN_CS, DEFAULT_PIN_CLK);
}

// Default Constructor w/ Number of Matricies
LEDMatrix7219::LEDMatrix7219 (byte n, byte intensity)
{ 
  init (n, intensity, DEFAULT_PIN_DATA_IN, DEFAULT_PIN_CS, DEFAULT_PIN_CLK);
}

// Constructor w/ Params
LEDMatrix7219::LEDMatrix7219 (byte num, byte intensity, byte dataIn, byte cs, byte clk)
{
  init (num, intensity, dataIn, cs, clk);
}

void LEDMatrix7219::init(byte num, byte intensity, byte dataIn, byte cs, byte clk)
{

  // Number of matrices, LED Intensity, data-in pin, cs pin, clock pin
  _numDevices    = num;
  _LEDIntensity  = intensity;
  _pin_dataIn    = dataIn;  
  _pin_cs        = cs;
  _pin_clk       = clk;
  
  // Set pin modes ot output
  pinMode (_pin_dataIn, OUTPUT);
  pinMode (_pin_cs    , OUTPUT);
  pinMode (_pin_clk   , OUTPUT);

  // Initialize all MAX7219 various registers
  writeRegisterAll (MAX7219_REG_SCANLIMIT, 0x07);                        // Scans all eight digits (columns)
  writeRegisterAll (MAX7219_REG_DECODEMODE, 0x00);                       // Matrix (not digits)
  writeRegisterAll (MAX7219_REG_SHUTDOWN, 0x01);                         // not in shutdown mode
  writeRegisterAll (MAX7219_REG_DISPLAYTEST, 0x00);                      // Display test off
  writeRegisterAll (MAX7219_REG_INTENSITY, (_LEDIntensity & B00001111)); // the first 0x0f is the value you can set range: 0x00 to 0x0f

  // Empty all of the display columns for all devices  
  for (int i=0; i<8; i++) {
    writeRegisterAll (MAX7219_REG_COL0+i, 0);
  }

}

void LEDMatrix7219::putByte(byte data) {
  
  // Clock out the bits
  for (int i=0; i<8; i++) {
    digitalWrite (_pin_clk, LOW);
    digitalWrite (_pin_dataIn, (data & bitMask[i]));
    digitalWrite (_pin_clk, HIGH);    
  };

}

// Write a single register/byte value pair to the dataIn bus
void LEDMatrix7219::writeRegister( byte reg, byte value) {
  putByte(reg);
  putByte(value);
}

// Write register value to all MAX7219's
void LEDMatrix7219::writeRegisterAll (byte reg, byte value) {

  // Begin, write register+value pairs, end
  digitalWrite(_pin_cs, LOW);
  for (int i=0; i<_numDevices; i++) writeRegister (reg, value);
  digitalWrite(_pin_cs, LOW);
  digitalWrite(_pin_cs,HIGH);
  
}

// For addressing one out of multiple cascaded MAX7219's
void LEDMatrix7219::writeDeviceRegister (byte deviceNum, byte reg, byte value) {
  
  // Begin
  digitalWrite(_pin_cs, LOW);

  // Write no-op before and after target device. Write register+value to device
  for (int i=0; i<_numDevices; i++) {
    if (i != (deviceNum-1)) {
      writeRegister (MAX7219_REG_NOOP, 0);
    } else {
      writeRegister (reg, value);
    }
  }

  // End
  digitalWrite(_pin_cs, LOW);
  digitalWrite(_pin_cs,HIGH); 
}


