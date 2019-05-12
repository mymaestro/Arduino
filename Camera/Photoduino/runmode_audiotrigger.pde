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
  
 // Run audio trigger mode
void runAs_audioTrigger() {    
    
   display_printTitle(MSG_RUN_AUDIO);
   
   keyboard_waitForNokey();
   
   attachInterrupt(0, keyboard_interrupts, CHANGE);
   attachInterrupt(1, keyboard_interrupts, CHANGE);
   
   for(unsigned int ciclesCounter = 0; (cancelFlag==false && !(audioTriggerMode_numCicles>0 && ciclesCounter >= audioTriggerMode_numCicles));ciclesCounter++) { 
     
     // Normal shooting mode
     if (audioTriggerMode_shootingMode == SHOOTINGMODE_NORMAL) {
       
       sensor_waitFor(PINS_SENSOR_MIC, SENSOR_MODE_HIGHER, audioTriggerMode_sensorLimit, 0);
       
       if(!cancelFlag) {
         
         camera_autofocusBegin(audioTriggerMode_autofocusTime); 
         camera_shutterBegin(audioTriggerMode_shutterLagTime); 
          
         if (audioTriggerMode_useFlash1) flash_shoot(audioTriggerMode_preFlash1Time, PINS_FLASH1);
         if (audioTriggerMode_useFlash2) flash_shoot(audioTriggerMode_preFlash2Time, PINS_FLASH2); 

       }
     }  
     
     // Prebulb shooting mode
     if (audioTriggerMode_shootingMode == SHOOTINGMODE_PREBULB) {
      
       // With prebulb
       camera_autofocusBegin(audioTriggerMode_autofocusTime);
       camera_shutterBegin(audioTriggerMode_shutterLagTime); 
       
       sensor_waitFor(PINS_SENSOR_MIC, SENSOR_MODE_HIGHER, audioTriggerMode_sensorLimit, 0);
       
       if(!cancelFlag) {
         if (audioTriggerMode_useFlash1) flash_shoot(audioTriggerMode_preFlash1Time, PINS_FLASH1);
         if (audioTriggerMode_useFlash2) flash_shoot(audioTriggerMode_preFlash2Time, PINS_FLASH2);     
       }          
     }  
     
      // Mirror lock-up shooting mode
     if (audioTriggerMode_shootingMode == SHOOTINGMODE_MIRRORLOCKUP) {
              
       for(boolean result = false; result == false;  ){      
         camera_mirrorLockUp(audioTriggerMode_autofocusTime, audioTriggerMode_shutterLagTime);
         result = sensor_waitFor(PINS_SENSOR_MIC, SENSOR_MODE_HIGHER, audioTriggerMode_sensorLimit, DEVICES_CAMERA_MIRROR_LOCKUP_TIMELIMIT);
       }
       camera_shutterBegin(1); 
      
       if(!cancelFlag) {
         
         if (audioTriggerMode_useFlash1) flash_shoot(audioTriggerMode_preFlash1Time, PINS_FLASH1);
         if (audioTriggerMode_useFlash2) flash_shoot(audioTriggerMode_preFlash2Time, PINS_FLASH2); 
       }          
     }
    
     // Common for all shooting modes
     camera_shutterEnd(audioTriggerMode_preCloseTime); 
     camera_autofocusEnd();
     if(!cancelFlag) delay(audioTriggerMode_interCicleTime);

   } 
     
   display_printAborting();
   keyboard_waitForNokey();
   
   detachInterrupt(0);
   detachInterrupt(1);
}

