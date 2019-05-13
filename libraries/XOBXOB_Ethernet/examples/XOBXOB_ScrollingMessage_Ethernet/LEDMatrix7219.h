///////////////////////////////////////////////////////////////
//
//  LEDMatrix7219.h
//
//  Class for running an 8x8 LED matrix using a MAX7219 controller
//

#ifndef LED_MARTIX_7219_H
#define LED_MARTIX_7219_H

// Define max7219 registers
#define MAX7219_REG_NOOP         0x00
#define MAX7219_REG_COL0         0x01
#define MAX7219_REG_COL1         0x02
#define MAX7219_REG_COL2         0x03
#define MAX7219_REG_COL3         0x04
#define MAX7219_REG_COL4         0x05
#define MAX7219_REG_COL5         0x06
#define MAX7219_REG_COL6         0x07
#define MAX7219_REG_COL7         0x08
#define MAX7219_REG_DECODEMODE   0x09
#define MAX7219_REG_INTENSITY    0x0A
#define MAX7219_REG_SCANLIMIT    0x0B
#define MAX7219_REG_SHUTDOWN     0x0C
#define MAX7219_REG_DISPLAYTEST  0x0F

// Default number of displays and LED intensity
#define DEFAULT_NUM_DEVICES        byte(1)
#define DEFAULT_LED_INTENSITY      byte(15)

// Default data and control pins
#define DEFAULT_PIN_DATA_IN        byte(5)
#define DEFAULT_PIN_CS             byte(6)
#define DEFAULT_PIN_CLK            byte(7)

#include "Arduino.h"

// CLASS
class LEDMatrix7219
{
  public:

    // Constructors
    LEDMatrix7219();
    LEDMatrix7219(byte);
    LEDMatrix7219(byte, byte);
    LEDMatrix7219(byte, byte, byte, byte, byte);
    
    // Methods
    void writeRegister       (byte, byte);
    void writeRegisterAll    (byte, byte);
    void writeDeviceRegister (byte, byte, byte);
    
  private:
  
    byte _pin_dataIn;  
    byte _pin_cs;
    byte _pin_clk;
    byte _numDevices;
    byte _LEDIntensity;
    
    void init (byte, byte, byte, byte, byte);
    void putByte(byte);
    
};

#endif
