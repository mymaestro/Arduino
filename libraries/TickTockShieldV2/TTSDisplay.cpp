/*
  TTSDisplay.cpp

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

#include <Arduino.h>
#include <Streaming.h>

#include "TTSDisplay.h"

//0~9,A,b,C,d,E,F,"-"," "
const uchar TubeTab[] = 
{
    0x3f,0x06,0x5b,0x4f,
    0x66,0x6d,0x7d,0x07,
    0x7f,0x6f,0x77,0x7c,
    0x39,0x5e,0x79,0x71,
    0x40,0x00
};
    
#define PINCLK      7                   // pin of clk 
#define PINDTA      8                   // pin of data

/*********************************************************************************************************
 * Function Name: TTSDisplay
 * Description:  init
 * Parameters: None
 * Return: None
*********************************************************************************************************/
TTSDisplay::TTSDisplay()
{
    Clkpin  = PINCLK;
    Datapin = PINDTA;
    
    pinMode(Clkpin,OUTPUT);
    pinMode(Datapin,OUTPUT);
    
    for(int i=0; i<4; i++)
    {
        dtaDisplay[i] = 0x00;
    }
    
    set();
    //clear();
}

/*********************************************************************************************************
 * Function Name: display
 * Description:  display a num in certain location
 * Parameters: loca - location, 3-2-1-0
 *             num - number to display
 * Return: None
*********************************************************************************************************/
void TTSDisplay::display(uchar loca, uchar dta)
{

    if(loca > 3 || loca < 0)return;

    dtaDisplay[loca] = dta;

    loca = 3-loca;

    uchar segData = coding(dta);
    
    start();                            //start signal sent to TM1637 from MCU
    writeByte(ADDR_FIXED);
    stop();
    
    start();
    writeByte(loca|0xc0);
    writeByte(segData);
    stop();
    
    start();
    writeByte(Cmd_Dispdisplay);
    stop();

}

/*********************************************************************************************************
 * Function Name: num
 * Description:  display a num  that 0 - 9999
 * Parameters: num - number to display
 * Return: None
*********************************************************************************************************/
void TTSDisplay::num(int dta)
{
    if(dta < 0 || dta > 9999) return;           // bad data
    
    //clear();
    
    pointOff();
    if(dta < 10)
    {
        display(0, dta);
        display(1, 0x7f);
        display(2, 0x7f);
        display(3, 0x7f);
    }
    else if(dta < 100)
    {
        display(1, dta/10);
        display(0, dta%10);
        display(2, 0x7f);
        display(3, 0x7f);
    }
    else if(dta < 1000)
    {
        display(2, dta/100);
        display(1, (dta/10)%10);
        display(0, dta%10);
        display(3, 0x7f);
    }
    else
    {
        display(3, dta/1000);
        display(2, (dta/100)%10);
        display(1, (dta/10)%10);
        display(0, dta%10);
    }
}

/*********************************************************************************************************
 * Function Name: time
 * Description:  display time
 * Parameters: hour - hour
 *             min - minutes
 * Return: None
*********************************************************************************************************/
void TTSDisplay::time(uchar hour, uchar min)
{
    if(hour > 24 || hour < 0) return;           // bad data
    if(min > 60 || min < 0) return;             // bad data

    display(3, hour/10);
    display(2, hour%10);
    display(1, min/10);
    display(0, min%10);
}
    
/*********************************************************************************************************
 * Function Name: clear
 * Description:  clear all 
 * Parameters: None
 * Return: None
*********************************************************************************************************/
void TTSDisplay::clear()
{
    display(0x00,0x7f);
    display(0x01,0x7f);
    display(0x02,0x7f);
    display(0x03,0x7f);
}

/*********************************************************************************************************
 * Function Name: writeByte
 * Description:  write a byte to tm1636
 * Parameters: wr_data: data to write
 * Return: None
*********************************************************************************************************/
void TTSDisplay::writeByte(uchar wr_data)
{
    uchar i,count1;
    for(i=0;i<8;i++)                                        // sent 8bit data
    {
        digitalWrite(Clkpin,LOW);
        if(wr_data & 0x01)digitalWrite(Datapin,HIGH);       // LSB first
        else digitalWrite(Datapin,LOW);
        wr_data >>= 1;
        digitalWrite(Clkpin,HIGH);
    }
    
    digitalWrite(Clkpin,LOW);                               // wait for the ACK
    digitalWrite(Datapin,HIGH);
    digitalWrite(Clkpin,HIGH);
    pinMode(Datapin,INPUT);
    
    while(digitalRead(Datapin))
    {
        count1 += 1;
        if(200 == count1)
        {
            pinMode(Datapin,OUTPUT);
            digitalWrite(Datapin,LOW);
            count1 =0;
        }
        pinMode(Datapin,INPUT);
    }
    pinMode(Datapin,OUTPUT);

}

/*********************************************************************************************************
 * Function Name: start
 * Description:  send start signal to TTSDisplay
 * Parameters: None
 * Return: None
*********************************************************************************************************/
void TTSDisplay::start(void)
{
    digitalWrite(Clkpin,HIGH);                              //send start signal to TM1637
    digitalWrite(Datapin,HIGH);
    digitalWrite(Datapin,LOW);
    digitalWrite(Clkpin,LOW);
}

/*********************************************************************************************************
 * Function Name: stop
 * Description:  send end signal
 * Parameters: None
 * Return: None
*********************************************************************************************************/
void TTSDisplay::stop(void)
{
    digitalWrite(Clkpin,LOW);
    digitalWrite(Datapin,LOW);
    digitalWrite(Clkpin,HIGH);
    digitalWrite(Datapin,HIGH);
}

/*********************************************************************************************************
 * Function Name: set
 * Description:  init
 * Parameters: brightness - brightness
 *             SetDta - data
 *             SetAddr - address
 * Return: None
*********************************************************************************************************/
void TTSDisplay::set(uchar brightness,uchar SetData,uchar SetAddr)
{
    _brightness = brightness;
    Cmd_SetData = SetData;
    Cmd_SetAddr = SetAddr;
    Cmd_Dispdisplay = 0x88 + brightness;
}

/*********************************************************************************************************
 * Function Name: pointOn
 * Description:  display :
 * Parameters: None
 * Return: None
*********************************************************************************************************/
void TTSDisplay::pointOn()
{
    _PointFlag = 1;
    
    for(int i=0; i<4; i++)
    {
        display(i, dtaDisplay[i]);
    }
}

/*********************************************************************************************************
 * Function Name: pointOff
 * Description:  no :
 * Parameters: None
 * Return: None
*********************************************************************************************************/
void TTSDisplay::pointOff()
{
    _PointFlag = 0;
    
    for(int i=0; i<4; i++)
    {
        display(i, dtaDisplay[i]);
    }
}

/*********************************************************************************************************
 * Function Name: coding
 * Description:  coding
 * Parameters: None
 * Return: None
*********************************************************************************************************/
uchar TTSDisplay::coding(uchar DispData)
{

    uchar PointData = _PointFlag ? 0x80 : 0x00;
    DispData = (0x7f == DispData) ? PointData : (TubeTab[DispData]+PointData);
    return DispData;
}
    
   
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/