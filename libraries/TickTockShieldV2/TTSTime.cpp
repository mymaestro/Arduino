/*
  TTSTime.cpp

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

#include <Wire.h>
#include "TTSTime.h"

#define YEAR        13
#define MONTH       11
#define DAY         21
#define WEEK        4

#define I2CADDR     0x68

/*********************************************************************************************************
 * Function Name: decToBcd
 * Description:  dec to bcd
 * Parameters: val - data input
 * Return: return bcd of val
*********************************************************************************************************/
uchar TTSTime::decToBcd(uchar val)
{
    return ( (val/10*16) + (val%10) );
}

/*********************************************************************************************************
 * Function Name: bcdToDec
 * Description:  bcd to dec
 * Parameters: val - data input
 * Return: return dec of val
*********************************************************************************************************/
uchar TTSTime::bcdToDec(uchar val)
{
    return ( (val/16*10) + (val%16) );
}

/*********************************************************************************************************
 * Function Name: setTime
 * Description:  set rtc time
 * Parameters: hour - hour, min - minutes, sec - second
 * Return: None
*********************************************************************************************************/
void TTSTime::setTime(uchar hour, uchar min, uchar sec)
{

    if(hour > 24 || hour < 0)return;
    if(min > 59 || min < 0)return;
    if(sec > 59 || min < 0)return;
    
    Wire.beginTransmission(I2CADDR);
    Wire.write((uchar)0x00);
    Wire.write(decToBcd(sec));                              // 0 to bit 7 starts the clock
    Wire.write(decToBcd(min));
    Wire.write(decToBcd(hour));                             // If you want 12 hour am/pm you need to set
                                                            // bit 6 (also need to change readDateDs1307)
    Wire.write(decToBcd(WEEK));
    Wire.write(decToBcd(DAY));
    Wire.write(decToBcd(MONTH));
    Wire.write(decToBcd(YEAR));
    Wire.endTransmission();
}

/*********************************************************************************************************
 * Function Name: getTime
 * Description:  get time
 * Parameters: *hour - hour, *min - minutes, *sec - second
 * Return: None
*********************************************************************************************************/
void TTSTime::getTime(uchar *hour, uchar *min, uchar *sec)
{
    Wire.beginTransmission(I2CADDR);
    Wire.write((uchar)0x00);
    Wire.endTransmission();
    Wire.requestFrom(I2CADDR, 7);
    
    uchar tmp = 0;
    
    *sec  = bcdToDec(Wire.read() & 0x7f);
    *min  = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);                   // Need to change this if 12 hour am/pm
    tmp   = bcdToDec(Wire.read());
    tmp   = bcdToDec(Wire.read());
    tmp   = bcdToDec(Wire.read());
    tmp   = bcdToDec(Wire.read());
}

/*********************************************************************************************************
 * Function Name: getHour
 * Description:  get hour
 * Parameters: None
 * Return: hour
*********************************************************************************************************/
uchar TTSTime::getHour()
{
    uchar hour, min, sec;
    getTime(&hour, &min, &sec);
    return hour;
}

/*********************************************************************************************************
 * Function Name: getMin
 * Description:  get minutes
 * Parameters: None
 * Return: minutes
*********************************************************************************************************/
uchar TTSTime::getMin()
{
    uchar hour, min, sec;
    getTime(&hour, &min, &sec);
    return min;
}

/*********************************************************************************************************
 * Function Name: getSec
 * Description:  get second
 * Parameters: None
 * Return: second
*********************************************************************************************************/
uchar TTSTime::getSec()
{
    uchar hour, min, sec;
    getTime(&hour, &min, &sec);
    return sec;
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/