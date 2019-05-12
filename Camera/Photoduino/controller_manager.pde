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

// Runs the controller
void controller_run(){
   lcd.clear();
   lcd.print(MSG_PHOTODUINO_V);
   lcd.print(CODE_MAYOR_VERSION,10);
   lcd.print(".");
   lcd.print(CODE_MINOR_VERSION,10);
   lcd.setCursor(0,1);
   lcd.print(MSG_READY);
   buzzer_beep(200);
   keyboard_waitForAnyKey();
   controller_showMainMenu();
}

// Main Menu View
void controller_showMainMenu(){
  
  byte currentOption = 0;
  
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_MAIN_MENU);

    if(currentOption==0) display_printEnumeration(0, MSG_SYSTEM_CONFIG); 
    if(currentOption==1) display_printEnumeration(1, MSG_INTERVAL_MODE); 
    if(currentOption==2) display_printEnumeration(2, MSG_AUDIO_TRIGGER); 
    if(currentOption==3) display_printEnumeration(3, MSG_BARRIER_MODE);  
    if(currentOption==4) display_printEnumeration(4, MSG_SHOCK_TRIGGER); 
    if(currentOption==5) display_printEnumeration(5, MSG_FLASH_SLAVE); 
    
    keyboard_waitForAnyKey();
    
    if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 5, 1);   
    if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 5, 1);        
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_BH) {
                        
          if(currentOption==0) controller_showSystemConfig(); 
          if(currentOption==1) controller_showIntervalMenu(); 
          if(currentOption==2) controller_showAudioTriggerMenu();
          if(currentOption==3) controller_showBarrierMenu();
          if(currentOption==4) controller_showShockTriggerMenu();
          if(currentOption==5) controller_showFlashSlaveMenu();
    }
  }
}

// Interval Menu View
void controller_showIntervalMenu(){
  
  byte currentOption = 0;
 
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_INTERVAL_MODE);

    // Show menu option
    if(currentOption==0) lcd.print(MSG_CONFIG);
    if(currentOption==1) lcd.print(MSG_RUN);
    if(currentOption==2) lcd.print(MSG_RESET_CONFIG);
    
    keyboard_waitForAnyKey();
    
    if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 2, 1);        
    if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 2, 1);   
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_BH) {
                    
         if(currentOption==0)  { controller_showIntervalConfig(); } 
         if(currentOption==1)  { runAs_interval(); }
         if(currentOption==2)  { config_loadDefaults_intervalMode(); config_saveBackup_intervalMode(); display_printResetting(); }
    }
  }
}


// Audio trigger menu view
void controller_showAudioTriggerMenu(){
  
  byte currentOption = 0;
 
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_AUDIO_TRIGGER);

    // Show menu option
    if(currentOption==0) lcd.print(MSG_CONFIG);
    if(currentOption==1) lcd.print(MSG_RUN);
    if(currentOption==2) lcd.print(MSG_RESET_CONFIG);
    
    keyboard_waitForAnyKey();
    
    if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_BH) {
                    
           if(currentOption==0) { controller_showAudioTriggerConfig(); } 
           if(currentOption==1) { runAs_audioTrigger(); }
           if(currentOption==2) { config_loadDefaults_audioTriggerMode(); config_saveBackup_audioTriggerMode(); display_printResetting(); }
    }
  }
}

// Shock trigger menu view
void controller_showShockTriggerMenu(){
  
  byte currentOption = 0;
 
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_SHOCK_TRIGGER);

    // Show menu option
    if(currentOption==0) lcd.print(MSG_CONFIG);
    if(currentOption==1) lcd.print(MSG_RUN);
    if(currentOption==2) lcd.print(MSG_RESET_CONFIG);
    
    keyboard_waitForAnyKey();
    
    if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_BH) {
                    
           if(currentOption==0)  { controller_showShockTriggerConfig(); } 
           if(currentOption==1)  { runAs_shockTrigger(); }
           if(currentOption==2)  { config_loadDefaults_shockTriggerMode(); config_saveBackup_shockTriggerMode(); display_printResetting();}
    }
  }
}

// Flash slave menu view
void controller_showFlashSlaveMenu(){
  
  byte currentOption = 0;
 
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_FLASH_SLAVE);

    // Show menu option
    if(currentOption==0) lcd.print(MSG_CONFIG);
    if(currentOption==1) lcd.print(MSG_RUN);
    if(currentOption==2) lcd.print(MSG_RESET_CONFIG);
    
    keyboard_waitForAnyKey();
    
    if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_BH) {
                    
           if(currentOption==0)  { controller_showFlashSlaveConfig(); } 
           if(currentOption==1)  { runAs_flashSlave(); }
           if(currentOption==2)  { config_loadDefaults_flashSlave(); config_saveBackup_flashSlave(); display_printResetting();}
    }
  }
}

// Barrier menu view
void controller_showBarrierMenu(){
  
  byte currentOption = 0;
  
  laser_turnOn();
  
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_BARRIER_MODE);

    // Show menu option
    if(currentOption==0) lcd.print(MSG_CONFIG);
    if(currentOption==1) lcd.print(MSG_RUN);
    if(currentOption==2) lcd.print(MSG_RESET_CONFIG);
    
    keyboard_waitForAnyKey();
    
    if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_BH) {
                    
          if(currentOption==0)  { controller_showBarrierConfig(); } 
          if(currentOption==1)  { runAs_barrierMode(); }
          if(currentOption==2)  { config_loadDefaults_barrierMode(); config_saveBackup_barrierMode(); display_printResetting(); }
    }
  }
  
  laser_turnOff();
}

// Interval Config view
void controller_showIntervalConfig(){
  
  byte currentOption = 0;
  
  for(boolean exit = false;!exit;){
     
      display_printTitle(MSG_INTERVAL_MODE);
     
      // Show menu options
      if(currentOption==0) { lcd.print(MSG_AUTOFOCUS_TIME);  }
      if(currentOption==1) { lcd.print(MSG_SHUTTERLAG_TIME); }     
      if(currentOption==2) { lcd.print(MSG_USE_FLASH1); display_printBoolean(intervalMode_useFlash1); }
      if(currentOption==3) { lcd.print(MSG_FLASH1_PRETIME); }
      if(currentOption==4) { lcd.print(MSG_USE_FLASH2); display_printBoolean(intervalMode_useFlash2); }
      if(currentOption==5) { lcd.print(MSG_FLASH2_PRETIME); }
      if(currentOption==6) { lcd.print(MSG_CLOSE_PRETIME); }
      if(currentOption==7) { lcd.print(MSG_INTERVAL_UNITS); }
      if(currentOption==8) { lcd.print(MSG_INTERVAL_VALUE); }
      if(currentOption==9) { lcd.print(MSG_CICLES); }

      keyboard_waitForAnyKey();
      
      if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 9, 1); 
      if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 9, 1);  
      if(lastKey==KEY_AH) { config_saveBackup_intervalMode(); exit = true; }
      if(lastKey==KEY_BH) {          

             if(currentOption==0)  controller_setNumericParameterValue(&intervalMode_autofocusTime, MSG_AUTOFOCUS_TIME,UNITS_MS);
             if(currentOption==1)  controller_setNumericParameterValue(&intervalMode_shutterLagTime, MSG_SHUTTERLAG_TIME,UNITS_MS);
             if(currentOption==2)  intervalMode_useFlash1 = !intervalMode_useFlash1;
             if(currentOption==3)  controller_setNumericParameterValue(&intervalMode_preFlash1Time, MSG_FLASH1_PRETIME,UNITS_MS);
             if(currentOption==4)  intervalMode_useFlash2 = !intervalMode_useFlash2;
             if(currentOption==5)  controller_setNumericParameterValue(&intervalMode_preFlash2Time, MSG_FLASH2_PRETIME,UNITS_MS);
             if(currentOption==6)  controller_setNumericParameterValue(&intervalMode_preCloseTime, MSG_CLOSE_PRETIME,UNITS_MS);
             if(currentOption==7)  controller_setUnits(&intervalMode_intervalUnits, MSG_INTERVAL_UNITS); 
             if(currentOption==8)  controller_setNumericParameterValue(&intervalMode_intervalValue, MSG_INTERVAL_VALUE,intervalMode_intervalUnits);
             if(currentOption==9)  controller_setNumericParameterValue(&intervalMode_numCicles, MSG_CICLES,UNITS_CICLES);     
      } 
   }
}

// Audio Trigger Config view
void controller_showAudioTriggerConfig(){
  
   byte currentOption = 0;
  
   for(boolean exit = false;!exit;){
     
      display_printTitle(MSG_AUDIO_CONFIG);
         
      // Show menu options
      if(currentOption==0) { lcd.print(MSG_SENSOR_LIMIT); }
      if(currentOption==1) { lcd.print(MSG_SHOOTING_MODE);  }
      if(currentOption==2) { lcd.print(MSG_AUTOFOCUS_TIME);  }
      if(currentOption==3) { lcd.print(MSG_SHUTTERLAG_TIME); }     
      if(currentOption==4) { lcd.print(MSG_USE_FLASH1); display_printBoolean(audioTriggerMode_useFlash1); }
      if(currentOption==5) { lcd.print(MSG_FLASH1_PRETIME); }
      if(currentOption==6) { lcd.print(MSG_USE_FLASH2); display_printBoolean(audioTriggerMode_useFlash2); }
      if(currentOption==7) { lcd.print(MSG_FLASH2_PRETIME); }
      if(currentOption==8) { lcd.print(MSG_CLOSE_PRETIME); }
      if(currentOption==9) { lcd.print(MSG_CICLES); }
      if(currentOption==10) { lcd.print(MSG_INTERCICLE_TIME); }
  
      keyboard_waitForAnyKey();
      
      if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 10, 1); 
      if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 10, 1); 
      if(lastKey==KEY_AH) { config_saveBackup_audioTriggerMode(); exit = true; }
      if(lastKey==KEY_BH) {          
          
             if(currentOption==0)  controller_setSensorLimit(&audioTriggerMode_sensorLimit, PINS_SENSOR_MIC, SENSOR_MODE_HIGHER);
             if(currentOption==1)  controller_setShootingModes(&audioTriggerMode_shootingMode, MSG_SHOOTING_MODE);
             if(currentOption==2)  controller_setNumericParameterValue(&audioTriggerMode_autofocusTime, MSG_AUTOFOCUS_TIME,UNITS_MS);
             if(currentOption==3)  controller_setNumericParameterValue(&audioTriggerMode_shutterLagTime, MSG_SHUTTERLAG_TIME,UNITS_MS);
             if(currentOption==4)  audioTriggerMode_useFlash1 = !audioTriggerMode_useFlash1;
             if(currentOption==5)  controller_setNumericParameterValue(&audioTriggerMode_preFlash1Time, MSG_FLASH1_PRETIME,UNITS_MS);
             if(currentOption==6)  audioTriggerMode_useFlash2 = !audioTriggerMode_useFlash2;
             if(currentOption==7)  controller_setNumericParameterValue(&audioTriggerMode_preFlash2Time, MSG_FLASH2_PRETIME,UNITS_MS);
             if(currentOption==8)  controller_setNumericParameterValue(&audioTriggerMode_preCloseTime, MSG_CLOSE_PRETIME,UNITS_MS);
             if(currentOption==9)  controller_setNumericParameterValue(&audioTriggerMode_numCicles, MSG_CICLES,UNITS_CICLES);
             if(currentOption==10)  controller_setNumericParameterValue(&audioTriggerMode_interCicleTime, MSG_INTERCICLE_TIME,UNITS_MS);
                      
      } 
   }
}

// Shock Trigger Config view
void controller_showShockTriggerConfig(){
  
   byte currentOption = 0;
  
   for(boolean exit = false;!exit;){
     
      display_printTitle(MSG_SHOCK_CONFIG);
         
      // Show menu options
      if(currentOption==0) { lcd.print(MSG_SENSOR_LIMIT); }
      if(currentOption==1) { lcd.print(MSG_SHOOTING_MODE);  }
      if(currentOption==2) { lcd.print(MSG_AUTOFOCUS_TIME);  }
      if(currentOption==3) { lcd.print(MSG_SHUTTERLAG_TIME); }     
      if(currentOption==4) { lcd.print(MSG_USE_FLASH1); display_printBoolean(shockTriggerMode_useFlash1); }
      if(currentOption==5) { lcd.print(MSG_FLASH1_PRETIME); }
      if(currentOption==6) { lcd.print(MSG_USE_FLASH2); display_printBoolean(shockTriggerMode_useFlash2); }
      if(currentOption==7) { lcd.print(MSG_FLASH2_PRETIME); }
      if(currentOption==8) { lcd.print(MSG_CLOSE_PRETIME); }
      if(currentOption==9) { lcd.print(MSG_CICLES); }
      if(currentOption==10) { lcd.print(MSG_INTERCICLE_TIME); }

      keyboard_waitForAnyKey();
      
      if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 10, 1); 
      if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 10, 1); 
      if(lastKey==KEY_AH) { config_saveBackup_shockTriggerMode(); exit = true; }
      if(lastKey==KEY_BH) {          
          
             if(currentOption==0)  controller_setSensorLimit(&shockTriggerMode_sensorLimit, PINS_SENSOR_SHOCK, SENSOR_MODE_HIGHER);
             if(currentOption==1)  controller_setShootingModes(&shockTriggerMode_shootingMode, MSG_SHOOTING_MODE);
             if(currentOption==2)  controller_setNumericParameterValue(&shockTriggerMode_autofocusTime, MSG_AUTOFOCUS_TIME,UNITS_MS);
             if(currentOption==3)  controller_setNumericParameterValue(&shockTriggerMode_shutterLagTime, MSG_SHUTTERLAG_TIME,UNITS_MS);
             if(currentOption==4)  shockTriggerMode_useFlash1 = !shockTriggerMode_useFlash1;
             if(currentOption==5)  controller_setNumericParameterValue(&shockTriggerMode_preFlash1Time, MSG_FLASH1_PRETIME,UNITS_MS);
             if(currentOption==6)  shockTriggerMode_useFlash2 = !shockTriggerMode_useFlash2;
             if(currentOption==7)  controller_setNumericParameterValue(&shockTriggerMode_preFlash2Time, MSG_FLASH2_PRETIME,UNITS_MS);
             if(currentOption==8)  controller_setNumericParameterValue(&shockTriggerMode_preCloseTime, MSG_CLOSE_PRETIME,UNITS_MS);
             if(currentOption==9)  controller_setNumericParameterValue(&shockTriggerMode_numCicles, MSG_CICLES,UNITS_CICLES);
             if(currentOption==10)  controller_setNumericParameterValue(&shockTriggerMode_interCicleTime, MSG_INTERCICLE_TIME,UNITS_MS);
                      
      } 
   }
}

// Barrier Config view
void controller_showBarrierConfig(){
  
   byte currentOption = 0;
   
   for(boolean exit = false;!exit;){
     
      display_printTitle(MSG_BARRIER_CONFIG);
         
      // Show menu options
      if(currentOption==0) { lcd.print(MSG_SENSOR_LIMIT); }
      if(currentOption==1) { lcd.print(MSG_SHOOTING_MODE);  }
      if(currentOption==2) { lcd.print(MSG_AUTOFOCUS_TIME);  }
      if(currentOption==3) { lcd.print(MSG_SHUTTERLAG_TIME); }     
      if(currentOption==4) { lcd.print(MSG_USE_FLASH1); display_printBoolean(barrierMode_useFlash1); }
      if(currentOption==5) { lcd.print(MSG_FLASH1_PRETIME); }
      if(currentOption==6) { lcd.print(MSG_USE_FLASH2); display_printBoolean(barrierMode_useFlash2); }
      if(currentOption==7) { lcd.print(MSG_FLASH2_PRETIME); }
      if(currentOption==8) { lcd.print(MSG_CLOSE_PRETIME); }
      if(currentOption==9) { lcd.print(MSG_CICLES); }
      if(currentOption==10) { lcd.print(MSG_INTERCICLE_TIME); }

      keyboard_waitForAnyKey();
      
      if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 10, 1); 
      if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 10, 1);  
      if(lastKey==KEY_AH) { config_saveBackup_barrierMode(); exit = true; }
      if(lastKey==KEY_BH) {          
          
             if(currentOption==0) controller_setSensorLimit(&barrierMode_sensorLimit, PINS_SENSOR_BARRIER, SENSOR_MODE_LOWER);
             if(currentOption==1) controller_setShootingModes(&barrierMode_shootingMode, MSG_SHOOTING_MODE);
             if(currentOption==2) controller_setNumericParameterValue(&barrierMode_autofocusTime, MSG_AUTOFOCUS_TIME,UNITS_MS);
             if(currentOption==3) controller_setNumericParameterValue(&barrierMode_shutterLagTime, MSG_SHUTTERLAG_TIME,UNITS_MS);
             if(currentOption==4) barrierMode_useFlash1 = !barrierMode_useFlash1;
             if(currentOption==5) controller_setNumericParameterValue(&barrierMode_preFlash1Time, MSG_FLASH1_PRETIME,UNITS_MS);
             if(currentOption==6) barrierMode_useFlash2 = !barrierMode_useFlash2;
             if(currentOption==7) controller_setNumericParameterValue(&barrierMode_preFlash2Time, MSG_FLASH2_PRETIME,UNITS_MS);
             if(currentOption==8) controller_setNumericParameterValue(&barrierMode_preCloseTime, MSG_CLOSE_PRETIME,UNITS_MS);
             if(currentOption==9) controller_setNumericParameterValue(&barrierMode_numCicles, MSG_CICLES,UNITS_CICLES);     
             if(currentOption==10) controller_setNumericParameterValue(&barrierMode_interCicleTime, MSG_INTERCICLE_TIME,UNITS_MS);     
      } 
   }
}

// Flash slave Config view
void controller_showFlashSlaveConfig(){
  
  byte currentOption = 0;
  
  for(boolean exit = false;!exit;){
     
      display_printTitle(MSG_FLASH_SLAVE);
     
      // Show menu options
      if(currentOption==0) { lcd.print(MSG_SENSOR_LIMIT); }
      if(currentOption==1) { lcd.print(MSG_USE_FLASH1); display_printBoolean(flashSlave_useFlash1); }
      if(currentOption==2) { lcd.print(MSG_FLASH1_PRETIME); }
      if(currentOption==3) { lcd.print(MSG_USE_FLASH2); display_printBoolean(flashSlave_useFlash2); }
      if(currentOption==4) { lcd.print(MSG_FLASH2_PRETIME); }
 
      keyboard_waitForAnyKey();
      
      if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 4, 1); 
      if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 4, 1);  
      if(lastKey==KEY_AH) { config_saveBackup_flashSlave(); exit = true; }
      if(lastKey==KEY_BH) {          

             if(currentOption==0) controller_setSensorLimit(&flashSlave_sensorLimit, PINS_SENSOR_BARRIER, SENSOR_MODE_HIGHER);
             if(currentOption==1) flashSlave_useFlash1 = !flashSlave_useFlash1;
             if(currentOption==2) controller_setNumericParameterValue(&flashSlave_preFlash1Time, MSG_FLASH1_PRETIME,UNITS_MS);
             if(currentOption==3) flashSlave_useFlash2 = !flashSlave_useFlash2;
             if(currentOption==4) controller_setNumericParameterValue(&flashSlave_preFlash2Time, MSG_FLASH2_PRETIME,UNITS_MS);
      } 
   }
}

// System config View
void controller_showSystemConfig(){
  
  byte currentOption = 0;
 
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_SYSTEM_CONFIG);

    // Show menu option
    if (currentOption==0) { lcd.print(MSG_BACKLIGHT); display_printBoolean(system_useBacklight); }
    if (currentOption==1) { lcd.print(MSG_SPEAKER); display_printBoolean(system_useSpeaker); }
    if (currentOption==2) { lcd.print(MSG_INTERFACE_SENSOR_MODE); }
    
    keyboard_waitForAnyKey();
    
    if(lastKey==KEY_A) circularList_decrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_AH) { config_saveBackup_system(); exit = true; } 
    if(lastKey==KEY_BH) {

          if(currentOption==0) backlight_toggle(); 
          if(currentOption==1) system_useSpeaker = !system_useSpeaker;
          if(currentOption==2) controller_setInterfaceSensorModes(&system_interfaceSensorLimit, MSG_INTERFACE_SENSOR_MODE);
    }
  }
}

// set of sensorlimit
void controller_setSensorLimit(unsigned int *value, byte readPin, byte mode){
 
    if(system_interfaceSensorLimit==SENSORLIMIT_VISUAL) controller_setSensorLimitVisual(value, readPin, mode);
    if(system_interfaceSensorLimit==SENSORLIMIT_NUMERIC) controller_setSensorLimitNumeric(value, readPin, mode);
}


// set of sensorlimit in visual mode
void controller_setSensorLimitVisual(unsigned int *value, byte readPin, byte mode){

     unsigned int sensorValue;
     
     display_printTitle(MSG_SENSOR_LIMIT);

     do { 
          sensorValue = sensor_read(readPin);
                    
          lcd.setCursor(0,1);
          
          for (byte i=0; i<16; i++) {
              if(i==*value>>6){
                 if (mode==SENSOR_MODE_HIGHER){
                     if ((sensorValue>>6)>=(*value>>6)) buzzer_beep(100); 
                     lcd.write(SYMBOL_UP);
                 } 
                 else                  
                 if (mode==SENSOR_MODE_LOWER){
                     if ((sensorValue>>6)<=(*value>>6)) buzzer_beep(100); 
                     lcd.write(SYMBOL_DOWN);
                 }
              }
              if(i<(sensorValue>>6)) lcd.write(SYMBOL_BOX); else lcd.print(" ");
          }
          
          keyboard_scan();
          
          if (lastKey==KEY_A) circularList_incrementBy(value, 0, 999, 64);
          if (lastKey==KEY_B) circularList_decrementBy(value, 0, 999, 64);
          
     } while (lastKey!=KEY_AH);
}

// set of sensorlimit in numeric mode
void controller_setSensorLimitNumeric(unsigned int *value, byte readPin, byte mode){

     unsigned int sensorValue = 0;
     unsigned int previousSensorValue = 0;
     byte currentPosition = 2;
     
     sensorValue = sensor_read(readPin);
     
     lcd.clear();
     lcd.print(">Sensor :");
     lcd.setCursor(0,1);
     lcd.print(">Limit  :");
     
     if (mode==SENSOR_MODE_HIGHER) {
             lcd.setCursor(15,1);
             lcd.write(SYMBOL_UP);
     }
     if (mode==SENSOR_MODE_LOWER) {
             lcd.setCursor(15,1);
             lcd.write(SYMBOL_DOWN);
     }       
                    
     lcd.setCursor(9,0);
     display_leadingZeroNumber(sensorValue, 3);
          
     lcd.setCursor(9,1);
     display_leadingZeroNumber(*value, 3);
     
     keyboard_waitForNokey();

     do { 
         
           sensorValue = sensor_read(readPin);
                    
           if (sensorValue != previousSensorValue) {
             lcd.noBlink();
             lcd.setCursor(9,0);
             display_leadingZeroNumber(sensorValue, 3);
             lcd.setCursor(9+currentPosition,1);
             lcd.blink();
           }
        
           keyboard_scan();
            
           if (lastKey!=NO_KEY) controller_setUnsignedIntValue(value, &currentPosition, 3, 9, 1);     
          
           if (((mode==SENSOR_MODE_HIGHER && sensorValue >= *value) || (mode==SENSOR_MODE_LOWER  && sensorValue <= *value)) && lastKey==NO_KEY) buzzer_beep(100); 
           else delay(20);
          
           previousSensorValue = sensorValue;
          
     } while (lastKey!=KEY_AH);
     
     lcd.noBlink();
}

// set of interval units
void controller_setUnits(byte *variable, char *msg){
  
     do { 
       
      display_printTitle(msg);
      display_printUnits(*variable);
      
      keyboard_waitForAnyKey();
      
      if (lastKey==KEY_A) circularList_decrementBy(variable, 0, 4, 1);
      if (lastKey==KEY_B) circularList_incrementBy(variable, 0, 4, 1);
   
     } while (lastKey!=KEY_AH);     
}

// set of shooting mode
void controller_setShootingModes(byte *variable, char *msg){
  
     do { 
       
      display_printTitle(msg);
      display_printShootingModes(*variable);
      
      keyboard_waitForAnyKey();
      
      if (lastKey==KEY_B) circularList_incrementBy(variable, 0, 2, 1);
   
     } while (lastKey!=KEY_AH);     
}

// set of shooting mode
void controller_setInterfaceSensorModes(byte *variable, char *msg){
  
     do { 
       
      display_printTitle(msg);
      display_printInterfaceSensorModes(*variable);
      
      keyboard_waitForAnyKey();
      
      if (lastKey==KEY_A) circularList_decrementBy(variable, 0, 1, 1);
      if (lastKey==KEY_B) circularList_incrementBy(variable, 0, 1, 1);
   
     } while (lastKey!=KEY_AH);     
}



// set of numeric parameter value
void controller_setNumericParameterValue(unsigned int *value, char *msg, byte units){
     
     lcd.clear();
     lcd.print(">");
     lcd.print(msg);
     
     lcd.setCursor(6,1);
     display_printUnits(units);
     
     lcd.setCursor(0,1);     
     display_leadingZeroNumber(*value, 5);
     
     byte currentPosition = 4;
     lcd.setCursor(currentPosition,1);
     lcd.blink();
     
     do{
       keyboard_scan();
       if (lastKey!=NO_KEY) controller_setUnsignedIntValue(value, &currentPosition, 5, 0, 1);     
     
     } while(lastKey!=KEY_AH);
     
     lcd.noBlink();
}

// set of unsigned integer value assisted by other function.
void controller_setUnsignedIntValue(unsigned int *value, byte *currentPos, byte positions, byte col, byte row){
     
     if (positions>5) positions = 5;
     byte values[positions];

     byte counter = 0;
       
     if (positions>4) { values[counter] = *value/10000; counter++; }
     if (positions>3) { values[counter] = (*value%10000)/1000; counter++; }
     if (positions>2) { values[counter] = (*value%1000)/100; counter++; }
     if (positions>1) { values[counter] = (*value%100)/10; counter++; }
     if (positions>0) { values[counter] = *value%10; counter++; }
    
      if (lastKey==KEY_A) {
             if (currentPos == 0) circularList_incrementBy(&values[*currentPos],0,5, 1);
             else circularList_incrementBy(&values[*currentPos],0,9, 1); 
      }
      if (lastKey==KEY_B) {
             if (currentPos == 0) circularList_decrementBy(&values[*currentPos],0,5, 1);
             else circularList_decrementBy(&values[*currentPos],0,9, 1);
      }
      if (lastKey==KEY_BH) circularList_decrementBy(currentPos,0, positions-1, 1);             
     
     counter = positions-1;
     *value = 0;
     
     if (positions>0) { *value = *value + values[counter]; counter--; }
     if (positions>1) { *value = *value + values[counter]*10; counter--; }
     if (positions>2) { *value = *value + values[counter]*100; counter--; }
     if (positions>3) { *value = *value + values[counter]*1000; counter--; }
     if (positions>4) { *value = *value + values[counter]*10000; }     
     
     lcd.noBlink();
     lcd.setCursor(col+*currentPos,row);
     lcd.print(values[*currentPos],10);
     lcd.setCursor(col+*currentPos,row);
     lcd.blink();
     
}

