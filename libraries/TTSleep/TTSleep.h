/************************************************************************************
 * 	
 * 	Name    : Triggertrap TTsleep.h                         
 * 	Author  : Noah Shibley / NoMi Design                        
 * 	Date    : July 10th 2011                                    
 * 	Version : 0.1                                              
 * 	Notes   : Most of this code comes from "Cloudy" on the arduino form
 *			  http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1292898715
 * 
 * 			Code for Triggertrap device (TTv1) and Triggertrap Shield (TT-k)                      
 * 
 * 	Copyright (c) 2012 NoMi Design (http://n0m1.com) All right reserved.
 * 
 * 	This file is part of Triggertrap. See Triggertrap.com for more information.
 * 
 * 		    Triggertrap is free software: you can redistribute it and/or modify
 * 		    it under the terms of the GNU General Public License as published by
 * 		    the Free Software Foundation, either version 3 of the License, or
 * 		    (at your option) any later version.
 * 
 * 		    Triggertrap is distributed in the hope that it will be useful,
 * 		    but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 		    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		    GNU General Public License for more details.
 * 
 * 		    You should have received a copy of the GNU General Public License
 * 		    along with Triggertrap.  If not, see <http://www.gnu.org/licenses/>.
 * 
 ***********************************************************************************/

#ifndef SLEEP_H
#define SLEEP_H



#include <avr/sleep.h>
#include <avr/wdt.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"    // for digitalRead, digitalWrite, pinMode, delayMicroseconds
#else
#include "WProgram.h"
#endif

extern "C" void WDT_vect(void) __attribute__ ((signal));

class Sleep {

public:
	
	friend void WDT_vect(void);

Sleep();
  
  /* modes of sleep
   SLEEP_MODE_IDLE 
   SLEEP_MODE_ADC
   SLEEP_MODE_PWR_SAVE
   SLEEP_MODE_EXT_STANDBY
   SLEEP_MODE_STANDBY
   SLEEP_MODE_PWR_DOWN
   */

	void idleMode() { setSleepMode(SLEEP_MODE_IDLE);}
	void adcMode() {setSleepMode(SLEEP_MODE_ADC);}
	void pwrSaveMode() {setSleepMode(SLEEP_MODE_PWR_SAVE);}
	void extStandbyMode(){setSleepMode(SLEEP_MODE_EXT_STANDBY);}
	void standbyMode(){setSleepMode(SLEEP_MODE_STANDBY);}
	void pwrDownMode(){setSleepMode(SLEEP_MODE_PWR_DOWN);}

	void sleepDelay(boolean &abortTrigger, unsigned long sleepTime,int shotCount);
	void calibrateTime(boolean &abortTrigger, unsigned long sleepTime); //calibrate the time keeping difference between WDT and Timer0
	unsigned long WDTMillis();	//estimated millis based on WDT during sleep. 
	
   
  


private:

  int sleepMode_;              
unsigned long timeSleep;
float calibv;
volatile byte isrcalled; 
static Sleep* pSleep; //static ptr to Sleep class for the ISR

  void setSleepMode(int mode);
  void WDT_Off();
  void WDT_On(byte psMask);
  int sleepNow(boolean &abortTrigger,unsigned long remainTime);

};

#endif







