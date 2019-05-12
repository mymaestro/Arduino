/**
 *
 * - Photoduino
 * - http://code.google.com/p/photoduino/
 *
 * This file is part of Photoduino.
 *
 * Photoduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Photoduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Photoduino.  If not, see <http://www.gnu.org/licenses/>.
 */ 
 
 // Run interval mode
void runAs_interval() { 
   
   display_printTitle(MSG_RUN_INTERVAL);

   keyboard_waitForNokey();
   
   attachInterrupt(0, keyboard_interrupts, CHANGE);
   attachInterrupt(1, keyboard_interrupts, CHANGE);
   
   for(unsigned int ciclesCounter = 0; (cancelFlag==false && !(intervalMode_numCicles>0 && ciclesCounter >= intervalMode_numCicles));ciclesCounter++) { 
     
     camera_autofocusBegin(intervalMode_autofocusTime); 
     camera_shutterBegin(intervalMode_shutterLagTime); 
  
     if (intervalMode_useFlash1) flash_shoot(intervalMode_preFlash1Time, PINS_FLASH1);
     if (intervalMode_useFlash2) flash_shoot(intervalMode_preFlash2Time, PINS_FLASH2); 
     
     camera_shutterEnd(intervalMode_preCloseTime); 
     camera_autofocusEnd(); 
     
     waitInterval();
     
   }
   
   display_printAborting();
   keyboard_waitForNokey();
      
   detachInterrupt(0);
   detachInterrupt(1);

}

// Wait interval time
void waitInterval(){
  
       if(intervalMode_intervalUnits == UNITS_MS)     delay(intervalMode_intervalValue); 
  else if(intervalMode_intervalUnits == UNITS_SECS)   delaySeconds(intervalMode_intervalValue);  
  else if(intervalMode_intervalUnits == UNITS_MINS)   delayMinutes(intervalMode_intervalValue);  
  else if(intervalMode_intervalUnits == UNITS_HOURS)  delayHours(intervalMode_intervalValue);  
  else if(intervalMode_intervalUnits == UNITS_DAYS)   delayDays(intervalMode_intervalValue);  
}

// Wait a time in seconds
void delaySeconds(unsigned int value) {
  for (unsigned int i = 0; i<value && !cancelFlag; i++) delay(1000); 
}

// Wait a time in minutes
void delayMinutes(unsigned int value) {
  for (unsigned int i = 0; i<value && !cancelFlag; i++) delaySeconds(60); 
}

// Wait a time in hours
void delayHours(unsigned int value){
  for (unsigned int i = 0; i<value && !cancelFlag; i++) delayMinutes(60); 
}

// Wait a time in days
void delayDays(unsigned int value){
  for (unsigned int i = 0; i<value && !cancelFlag; i++) delayHours(24); 
}




