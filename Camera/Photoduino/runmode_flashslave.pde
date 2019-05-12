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
void runAs_flashSlave() { 
       
   display_printTitle(MSG_RUN_FLASH_SLAVE);
   
   keyboard_waitForNokey();
   
   attachInterrupt(0, keyboard_interrupts, CHANGE);
   attachInterrupt(1, keyboard_interrupts, CHANGE);
   
   for(;(cancelFlag==false);) { 
  
       sensor_waitFor(PINS_SENSOR_BARRIER, SENSOR_MODE_HIGHER, flashSlave_sensorLimit, 0);
       if (flashSlave_useFlash1) flash_shoot(flashSlave_preFlash1Time, PINS_FLASH1);
       if (flashSlave_useFlash2) flash_shoot(flashSlave_preFlash2Time, PINS_FLASH2); 
   } 
     
   display_printAborting();
   keyboard_waitForNokey();
   
   detachInterrupt(0);
   detachInterrupt(1);
}

