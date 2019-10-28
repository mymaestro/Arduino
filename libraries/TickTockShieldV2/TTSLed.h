/*
  TTSLed.h

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

#ifndef __TTSLED_H__
#define __TTSLED_H__

#include <Arduino.h>


#define TTSLED1                 2           // pin of led1
#define TTSLED2                 3           // pin of led1
#define TTSLED3                 4           // pin of led1
#define TTSLED4                 5           // pin of led1


class TTSLed{

private:

    int __pin;
    unsigned char __state;                    // state of state, HIGH: on, LOW: off

public:

    TTSLed(int pin)
    {
        __pin = pin;
        __state = LOW;
        pinMode(__pin, OUTPUT);
        digitalWrite(__pin, LOW);
    }
    
    void on()                               // buzzer on
    {
        __state = HIGH;
        digitalWrite(__pin, __state);
    }
    
    void off()                              // buzzer off
    {
        __state = LOW;
        digitalWrite(__pin, __state);
    }
    
    unsigned char state()                   // return state of buzzer, HIGH: on, LOW: off
    {
        return __state;
    }
      
};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/