/*
  TTSTemp.h

  Author:Loovee
  2013-11-22
 
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

#ifndef __TTSTEMP_H__
#define __TTSTEMP_H__

#include <Arduino.h>

#define PINTEMP                 A0               // pin of buzzer


class TTSTemp{

private:

    int __pin;

    
public:

    TTSTemp()
    {
        __pin = PINTEMP;
    }
    
    int get()                                   // get temperature
    {
        int a = 0;

        for(int i=0; i<32; i++)
        {
            a+=analogRead(__pin);
        }
        
        a >>= 5;
        
        float resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
        float temperature=1/(log(resistance/10000)/3975+1/298.15)-273.15;//convert to temperature via datasheet ;
       
        return temperature;
    }
      
};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/