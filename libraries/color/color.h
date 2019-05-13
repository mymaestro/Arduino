/*

 A class for color for the tft 1.8" sceens
 color is 16 bit so r, g , and b are 5,6 and 5 bits respectively
*/

#ifndef Color_h
#define Color_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class Color
{

  public:
	Color();
    Color(byte r, byte g, byte b);
    void set_color(byte r, byte g, byte b);
    void set_color_hsb(byte h, byte s, byte b);
    int get_color_16bit();
    void convert_rgb_to_hsb(float r, float g, float b);
    void convert_hcl_to_rgb(float h, float c, float l);
    byte red;
    byte green;
    byte blue;
    int my_color;
  private:


};



#endif


