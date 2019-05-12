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
 
 // Run barrier mode
void runAs_barrierMode() { 
       
   display_printTitle(MSG_RUN_BARRIER);
   
   keyboard_waitForNokey();
   
   attachInterrupt(0, keyboard_interrupts, CHANGE);
   attachInterrupt(1, keyboard_interrupts, CHANGE);
   
   for(unsigned int ciclesCounter = 0; (cancelFlag==false && !(barrierMode_numCicles>0 && ciclesCounter >= barrierMode_numCicles));ciclesCounter++) { 
     
     // Normal shooting mode
     if (barrierMode_shootingMode == SHOOTINGMODE_NORMAL) {

       laser_turnOn();
       sensor_waitFor(PINS_SENSOR_BARRIER, SENSOR_MODE_LOWER, barrierMode_sensorLimit, 0);
       laser_turnOff();
       
       if(!cancelFlag) {
         
         camera_autofocusBegin(barrierMode_autofocusTime); 
         camera_shutterBegin(barrierMode_shutterLagTime); 
          
         if (barrierMode_useFlash1) flash_shoot(barrierMode_preFlash1Time, PINS_FLASH1);
         if (barrierMode_useFlash2) flash_shoot(barrierMode_preFlash2Time, PINS_FLASH2); 
       }
     }
     
     // Prebulb shooting mode
     if (barrierMode_shootingMode == SHOOTINGMODE_PREBULB) {
      
       camera_autofocusBegin(barrierMode_autofocusTime);
       camera_shutterBegin(barrierMode_shutterLagTime); 
       laser_turnOn();
       sensor_waitFor(PINS_SENSOR_BARRIER, SENSOR_MODE_LOWER, barrierMode_sensorLimit, 0);
       laser_turnOff();
       
       if(!cancelFlag) {
         if (barrierMode_useFlash1) flash_shoot(barrierMode_preFlash1Time, PINS_FLASH1);
         if (barrierMode_useFlash2) flash_shoot(barrierMode_preFlash2Time, PINS_FLASH2); 
       }
     }  
     
     // Mirror lock-up shooting mode
     if (barrierMode_shootingMode == SHOOTINGMODE_MIRRORLOCKUP) {
              
       for(boolean result = false; result == false;  ){      
         camera_mirrorLockUp(barrierMode_autofocusTime, barrierMode_shutterLagTime);
         result = sensor_waitFor(PINS_SENSOR_BARRIER, SENSOR_MODE_LOWER, barrierMode_sensorLimit, DEVICES_CAMERA_MIRROR_LOCKUP_TIMELIMIT);
       }
       camera_shutterBegin(1); 
      
       if(!cancelFlag) {
         
         if (barrierMode_useFlash1) flash_shoot(barrierMode_preFlash1Time, PINS_FLASH1);
         if (barrierMode_useFlash2) flash_shoot(barrierMode_preFlash2Time, PINS_FLASH2); 
       }          
     }
    
     // Common for all shooting modes
     camera_shutterEnd(barrierMode_preCloseTime); 
     camera_autofocusEnd();
     if(!cancelFlag) delay(barrierMode_interCicleTime);
  
   }
   
   laser_turnOn();
   display_printAborting();
   keyboard_waitForNokey();
   
   detachInterrupt(0);
   detachInterrupt(1);
}
