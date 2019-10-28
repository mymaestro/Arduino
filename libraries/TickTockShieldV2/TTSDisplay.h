/*
  TTSDisplay.h

  Author:Loovee
  2013-11-21
 
  The MIT License (MIT)
  Copyright (c) 2013 Seeed Technology Inc.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef __TTSDISPLAY_H__
#define __TTSDISPLAY_H__

#ifndef uchar
#define uchar unsigned char 
#endif

// definitions for TM1636
#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44
#define STARTADDR  0xc0

// definitions for the clock point of the digit tube
#define POINT_ON   1
#define POINT_OFF  0

// definitions for brightness
#define  BRIGHT_DARKEST 0
#define  BRIGHT_TYPICAL 2
#define  BRIGHTEST      7

//Special characters index of tube table
#define INDEX_NEGATIVE_SIGH    16
#define INDEX_BLANK            17


class TTSDisplay{


private:

    uchar Cmd_SetData;
    uchar Cmd_SetAddr;
    uchar Cmd_Dispdisplay;
    uchar _PointFlag;                                   //_PointFlag=1:the clock point on
    uchar _brightness;
    uchar Clkpin;
    uchar Datapin;
    uchar dtaDisplay[4];
    
private:

    void writeByte(uchar wr_data);                      //write 8bit data to tm1637
    void start(void);                                   //send start bits
    void stop(void);                                    //send stop bits
    void set(uchar = BRIGHT_TYPICAL, uchar = 0x40, uchar = 0xc0);       //To take effect the next time it displays. 
    uchar coding(uchar DispData);


//********************************************************************************************************** 
//*************************** USER INTERFACE ***************************************************************
//********************************************************************************************************** 
public:

    TTSDisplay();

    void display(uchar loca, uchar dta);                // display a single num(0-9)
    void num(int dta);                                  // display a num (0-9999)    
    void time(uchar hour, uchar min);                   // display time, such as: 11:26
    void clear();                                       // clear display
    
    void pointOn();                                     // display :
    void pointOff();                                    // undisplay :

};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/