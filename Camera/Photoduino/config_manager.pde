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

void config_init(){
  
  // Check if the firmware version is the same of eeprom config
  if (
      EEPROM.read(EE_ADDR_SIGNATURE_CODE1) == SIGNATURE_CODE1 && 
      EEPROM.read(EE_ADDR_SIGNATURE_CODE2) == SIGNATURE_CODE2 && 
      EEPROM.read(EE_ADDR_SIGNATURE_CODE3) == SIGNATURE_CODE3 && 
      EEPROM.read(EE_ADDR_SIGNATURE_CODE4) == SIGNATURE_CODE4 &&
      EEPROM.read(EE_ADDR_CODE_MAYOR_VERSION) == CODE_MAYOR_VERSION &&
      EEPROM.read(EE_ADDR_CODE_MINOR_VERSION) == CODE_MINOR_VERSION
     ) {
       // loads in ram the eeprom config
       config_loadBackup_all();
     
   } else {
      // clear the eeprom
      for (int i = 0; i < EEPROM_SIZE; i++)  EEPROM.write(i, 0xFF);
      
      // writes sign codes
      EEPROM.write(EE_ADDR_SIGNATURE_CODE1,SIGNATURE_CODE1);
      EEPROM.write(EE_ADDR_SIGNATURE_CODE2,SIGNATURE_CODE2);
      EEPROM.write(EE_ADDR_SIGNATURE_CODE3,SIGNATURE_CODE3);
      EEPROM.write(EE_ADDR_SIGNATURE_CODE4,SIGNATURE_CODE4);
      EEPROM.write(EE_ADDR_CODE_MAYOR_VERSION,CODE_MAYOR_VERSION);
      EEPROM.write(EE_ADDR_CODE_MINOR_VERSION,CODE_MINOR_VERSION);
      
      // load defaults in ram and save it on eeprom
      config_loadDefaults_all();
      config_saveBackup_all();
      
      // inform of resetting action
      display_printResetting();
  } 
}

void config_loadDefaults_all(){
  
    config_loadDefaults_system();
    config_loadDefaults_intervalMode();
    config_loadDefaults_audioTriggerMode();
    config_loadDefaults_shockTriggerMode();
    config_loadDefaults_barrierMode();  
    config_loadDefaults_flashSlave();  
}

void config_saveBackup_all(){
  
    config_saveBackup_system();
    config_saveBackup_intervalMode();
    config_saveBackup_audioTriggerMode();
    config_saveBackup_shockTriggerMode();
    config_saveBackup_barrierMode();
    config_saveBackup_flashSlave();
}
 
void config_loadBackup_all(){
    config_loadBackup_system();
    config_loadBackup_intervalMode();
    config_loadBackup_audioTriggerMode();
    config_loadBackup_shockTriggerMode();
    config_loadBackup_barrierMode();
    config_loadBackup_flashSlave();
} 
 
// Load the system config from eeprom to ram
void config_loadBackup_system(){   
  
  system_useBacklight = EEPROM.read(EE_ADDR_system_useBacklight);
  system_useSpeaker = EEPROM.read(EE_ADDR_system_useSpeaker);
  system_interfaceSensorLimit = EEPROM.read(EE_ADDR_system_interfaceSensorLimit);
}

// Load the default system config to ram
void config_loadDefaults_system() {
  
  system_useBacklight = DEFAULT_system_useBacklight;
  system_useSpeaker = DEFAULT_system_useSpeaker;
  system_interfaceSensorLimit = DEFAULT_system_interfaceSensorLimit;
}

// Save the system config from ram to eeprom
void config_saveBackup_system(){ 
  
  EEPROM.write(EE_ADDR_system_useBacklight, system_useBacklight);
  EEPROM.write(EE_ADDR_system_useSpeaker, system_useSpeaker);
  EEPROM.write(EE_ADDR_system_interfaceSensorLimit, system_interfaceSensorLimit);

}

// Load the default interval mode config to ram
void config_loadDefaults_intervalMode() {

  intervalMode_autofocusTime =   DEFAULT_intervalMode_autofocusTime;
  intervalMode_shutterLagTime =  DEFAULT_intervalMode_shutterLagTime;
  intervalMode_useFlash1 =       DEFAULT_intervalMode_useFlash1;
  intervalMode_preFlash1Time =   DEFAULT_intervalMode_preFlash1Time;
  intervalMode_useFlash2 =       DEFAULT_intervalMode_useFlash2;
  intervalMode_preFlash2Time =   DEFAULT_intervalMode_preFlash2Time;
  intervalMode_preCloseTime =    DEFAULT_intervalMode_preCloseTime;
  intervalMode_intervalUnits =   DEFAULT_intervalMode_intervalUnits;
  intervalMode_intervalValue =   DEFAULT_intervalMode_intervalValue;
  intervalMode_numCicles =       DEFAULT_intervalMode_numCicles;
  
}

// Save the inverval mode config to eeprom
void config_saveBackup_intervalMode(){

  eeprom_writeInt(EE_ADDR_intervalMode_autofocusTime, intervalMode_autofocusTime);
  eeprom_writeInt(EE_ADDR_intervalMode_shutterLagTime, intervalMode_shutterLagTime);
  EEPROM.write(EE_ADDR_intervalMode_useFlash1, intervalMode_useFlash1);
  eeprom_writeInt(EE_ADDR_intervalMode_preFlash1Time, intervalMode_preFlash1Time);
  EEPROM.write(EE_ADDR_intervalMode_useFlash2, intervalMode_useFlash2);
  eeprom_writeInt(EE_ADDR_intervalMode_preFlash2Time, intervalMode_preFlash2Time);
  eeprom_writeInt(EE_ADDR_intervalMode_preCloseTime, intervalMode_preCloseTime);
  EEPROM.write(EE_ADDR_intervalMode_intervalUnits, intervalMode_intervalUnits);
  eeprom_writeInt(EE_ADDR_intervalMode_intervalValue, intervalMode_intervalValue);
  eeprom_writeInt(EE_ADDR_intervalMode_numCicles, intervalMode_numCicles);  
  
}

// Load the interval mode config from eeprom to ram
void config_loadBackup_intervalMode(){  
  
  intervalMode_autofocusTime = eeprom_readInt(EE_ADDR_intervalMode_autofocusTime);
  intervalMode_shutterLagTime = eeprom_readInt(EE_ADDR_intervalMode_shutterLagTime);
  intervalMode_useFlash1 = EEPROM.read(EE_ADDR_intervalMode_useFlash1);
  intervalMode_preFlash1Time = eeprom_readInt(EE_ADDR_intervalMode_preFlash1Time);
  intervalMode_useFlash2 = EEPROM.read(EE_ADDR_intervalMode_useFlash2);
  intervalMode_preFlash2Time = eeprom_readInt(EE_ADDR_intervalMode_preFlash2Time);
  intervalMode_preCloseTime = eeprom_readInt(EE_ADDR_intervalMode_preCloseTime);
  intervalMode_intervalUnits = EEPROM.read(EE_ADDR_intervalMode_intervalUnits);
  intervalMode_intervalValue = eeprom_readInt(EE_ADDR_intervalMode_intervalValue);
  intervalMode_numCicles = eeprom_readInt(EE_ADDR_intervalMode_numCicles);
}


// Load the audioTriggerMode from eeprom to ram
void config_loadBackup_audioTriggerMode(){   
  
  audioTriggerMode_shootingMode = EEPROM.read(EE_ADDR_audioTriggerMode_shootingMode);
  audioTriggerMode_sensorLimit = eeprom_readInt(EE_ADDR_audioTriggerMode_sensorLimit);
  audioTriggerMode_autofocusTime = eeprom_readInt(EE_ADDR_audioTriggerMode_autofocusTime);
  audioTriggerMode_shutterLagTime = eeprom_readInt(EE_ADDR_audioTriggerMode_shutterLagTime);
  audioTriggerMode_preCloseTime = eeprom_readInt(EE_ADDR_audioTriggerMode_preCloseTime);
  audioTriggerMode_useFlash1 = EEPROM.read(EE_ADDR_audioTriggerMode_useFlash1);
  audioTriggerMode_preFlash1Time = eeprom_readInt(EE_ADDR_audioTriggerMode_preFlash1Time);
  audioTriggerMode_useFlash2 = EEPROM.read(EE_ADDR_audioTriggerMode_useFlash2);
  audioTriggerMode_preFlash2Time = eeprom_readInt(EE_ADDR_audioTriggerMode_preFlash2Time);
  audioTriggerMode_numCicles =  eeprom_readInt(EE_ADDR_audioTriggerMode_numCicles);
  audioTriggerMode_interCicleTime =  eeprom_readInt(EE_ADDR_audioTriggerMode_interCicleTime);
}

// Load the default audioTriggerMode config to ram
void config_loadDefaults_audioTriggerMode() {
  
  audioTriggerMode_shootingMode =     DEFAULT_audioTriggerMode_shootingMode;
  audioTriggerMode_sensorLimit =      DEFAULT_audioTriggerMode_sensorLimit;
  audioTriggerMode_autofocusTime =    DEFAULT_audioTriggerMode_autofocusTime;
  audioTriggerMode_shutterLagTime =   DEFAULT_audioTriggerMode_shutterLagTime;
  audioTriggerMode_preCloseTime =     DEFAULT_audioTriggerMode_preCloseTime;
  audioTriggerMode_useFlash1 =        DEFAULT_audioTriggerMode_useFlash1;
  audioTriggerMode_preFlash1Time =    DEFAULT_audioTriggerMode_preFlash1Time;
  audioTriggerMode_useFlash2 =        DEFAULT_audioTriggerMode_useFlash2;
  audioTriggerMode_preFlash2Time =    DEFAULT_audioTriggerMode_preFlash2Time;
  audioTriggerMode_numCicles =        DEFAULT_audioTriggerMode_numCicles;
  audioTriggerMode_interCicleTime =   DEFAULT_audioTriggerMode_interCicleTime;
}

// Save the audioTriggerMode from ram to eeprom
void config_saveBackup_audioTriggerMode(){ 
  
  EEPROM.write(EE_ADDR_audioTriggerMode_shootingMode, audioTriggerMode_shootingMode);
  eeprom_writeInt(EE_ADDR_audioTriggerMode_sensorLimit, audioTriggerMode_sensorLimit);
  eeprom_writeInt(EE_ADDR_audioTriggerMode_autofocusTime, audioTriggerMode_autofocusTime);
  eeprom_writeInt(EE_ADDR_audioTriggerMode_shutterLagTime, audioTriggerMode_shutterLagTime);
  eeprom_writeInt(EE_ADDR_audioTriggerMode_preCloseTime, audioTriggerMode_preCloseTime);
  EEPROM.write(EE_ADDR_audioTriggerMode_useFlash1, audioTriggerMode_useFlash1);
  eeprom_writeInt(EE_ADDR_audioTriggerMode_preFlash1Time, audioTriggerMode_preFlash1Time);
  EEPROM.write(EE_ADDR_audioTriggerMode_useFlash2, audioTriggerMode_useFlash2);
  eeprom_writeInt(EE_ADDR_audioTriggerMode_preFlash2Time, audioTriggerMode_preFlash2Time);
  eeprom_writeInt(EE_ADDR_audioTriggerMode_numCicles, audioTriggerMode_numCicles);
  eeprom_writeInt(EE_ADDR_audioTriggerMode_interCicleTime, audioTriggerMode_interCicleTime); 
}

// Load the shockTriggerMode from eeprom to ram
void config_loadBackup_shockTriggerMode(){   
  
  shockTriggerMode_shootingMode = EEPROM.read(EE_ADDR_shockTriggerMode_shootingMode);
  shockTriggerMode_sensorLimit = eeprom_readInt(EE_ADDR_shockTriggerMode_sensorLimit);
  shockTriggerMode_autofocusTime = eeprom_readInt(EE_ADDR_shockTriggerMode_autofocusTime);
  shockTriggerMode_shutterLagTime = eeprom_readInt(EE_ADDR_shockTriggerMode_shutterLagTime);
  shockTriggerMode_preCloseTime = eeprom_readInt(EE_ADDR_shockTriggerMode_preCloseTime);
  shockTriggerMode_useFlash1 = EEPROM.read(EE_ADDR_shockTriggerMode_useFlash1);
  shockTriggerMode_preFlash1Time = eeprom_readInt(EE_ADDR_shockTriggerMode_preFlash1Time);
  shockTriggerMode_useFlash2 = EEPROM.read(EE_ADDR_shockTriggerMode_useFlash2);
  shockTriggerMode_preFlash2Time = eeprom_readInt(EE_ADDR_shockTriggerMode_preFlash2Time);
  shockTriggerMode_numCicles =  eeprom_readInt(EE_ADDR_shockTriggerMode_numCicles);
  shockTriggerMode_interCicleTime =  eeprom_readInt(EE_ADDR_shockTriggerMode_interCicleTime);
}

// Load the default shockTriggerMode config to ram
void config_loadDefaults_shockTriggerMode() {
  
  shockTriggerMode_shootingMode =     DEFAULT_shockTriggerMode_shootingMode;
  shockTriggerMode_sensorLimit =      DEFAULT_shockTriggerMode_sensorLimit;
  shockTriggerMode_autofocusTime =    DEFAULT_shockTriggerMode_autofocusTime;
  shockTriggerMode_shutterLagTime =   DEFAULT_shockTriggerMode_shutterLagTime;
  shockTriggerMode_preCloseTime =     DEFAULT_shockTriggerMode_preCloseTime;
  shockTriggerMode_useFlash1 =        DEFAULT_shockTriggerMode_useFlash1;
  shockTriggerMode_preFlash1Time =    DEFAULT_shockTriggerMode_preFlash1Time;
  shockTriggerMode_useFlash2 =        DEFAULT_shockTriggerMode_useFlash2;
  shockTriggerMode_preFlash2Time =    DEFAULT_shockTriggerMode_preFlash2Time;
  shockTriggerMode_numCicles =        DEFAULT_shockTriggerMode_numCicles;
  shockTriggerMode_interCicleTime =   DEFAULT_shockTriggerMode_interCicleTime;

}

// Save the shockTriggerMode from ram to eeprom
void config_saveBackup_shockTriggerMode(){ 
  
  EEPROM.write(EE_ADDR_shockTriggerMode_shootingMode, shockTriggerMode_shootingMode);
  eeprom_writeInt(EE_ADDR_shockTriggerMode_sensorLimit, shockTriggerMode_sensorLimit);
  eeprom_writeInt(EE_ADDR_shockTriggerMode_autofocusTime, shockTriggerMode_autofocusTime);
  eeprom_writeInt(EE_ADDR_shockTriggerMode_shutterLagTime, shockTriggerMode_shutterLagTime);
  eeprom_writeInt(EE_ADDR_shockTriggerMode_preCloseTime, shockTriggerMode_preCloseTime);
  EEPROM.write(EE_ADDR_shockTriggerMode_useFlash1, shockTriggerMode_useFlash1);
  eeprom_writeInt(EE_ADDR_shockTriggerMode_preFlash1Time, shockTriggerMode_preFlash1Time);
  EEPROM.write(EE_ADDR_shockTriggerMode_useFlash2, shockTriggerMode_useFlash2);
  eeprom_writeInt(EE_ADDR_shockTriggerMode_preFlash2Time, shockTriggerMode_preFlash2Time);
  eeprom_writeInt(EE_ADDR_shockTriggerMode_numCicles, shockTriggerMode_numCicles);
  eeprom_writeInt(EE_ADDR_shockTriggerMode_interCicleTime, shockTriggerMode_interCicleTime); 
}


// Load the barrierMode from eeprom to ram
void config_loadBackup_barrierMode(){   
  
  barrierMode_shootingMode = EEPROM.read(EE_ADDR_barrierMode_shootingMode);
  barrierMode_sensorLimit = eeprom_readInt(EE_ADDR_barrierMode_sensorLimit);
  barrierMode_autofocusTime = eeprom_readInt(EE_ADDR_barrierMode_autofocusTime);
  barrierMode_shutterLagTime = eeprom_readInt(EE_ADDR_barrierMode_shutterLagTime);
  barrierMode_preCloseTime = eeprom_readInt(EE_ADDR_barrierMode_preCloseTime);
  barrierMode_useFlash1 = EEPROM.read(EE_ADDR_barrierMode_useFlash1);
  barrierMode_preFlash1Time = eeprom_readInt(EE_ADDR_barrierMode_preFlash1Time);
  barrierMode_useFlash2 = EEPROM.read(EE_ADDR_barrierMode_useFlash2);
  barrierMode_preFlash2Time = eeprom_readInt(EE_ADDR_barrierMode_preFlash2Time);
  barrierMode_numCicles =  eeprom_readInt(EE_ADDR_barrierMode_numCicles);
  barrierMode_interCicleTime =  eeprom_readInt(EE_ADDR_barrierMode_interCicleTime);
}

// Load the default barrierMode config to ram
void config_loadDefaults_barrierMode() {
  
  barrierMode_shootingMode =     DEFAULT_barrierMode_shootingMode;
  barrierMode_sensorLimit =      DEFAULT_barrierMode_sensorLimit;
  barrierMode_autofocusTime =    DEFAULT_barrierMode_autofocusTime;
  barrierMode_shutterLagTime =   DEFAULT_barrierMode_shutterLagTime;
  barrierMode_preCloseTime =     DEFAULT_barrierMode_preCloseTime;
  barrierMode_useFlash1 =        DEFAULT_barrierMode_useFlash1;
  barrierMode_preFlash1Time =    DEFAULT_barrierMode_preFlash1Time;
  barrierMode_useFlash2 =        DEFAULT_barrierMode_useFlash2;
  barrierMode_preFlash2Time =    DEFAULT_barrierMode_preFlash2Time;
  barrierMode_numCicles =        DEFAULT_barrierMode_numCicles;
  barrierMode_interCicleTime =   DEFAULT_barrierMode_interCicleTime;
}

// Save the barrierModee config from ram to eeprom
void config_saveBackup_barrierMode(){ 
  
  EEPROM.write(EE_ADDR_barrierMode_shootingMode, barrierMode_shootingMode);
  eeprom_writeInt(EE_ADDR_barrierMode_sensorLimit, barrierMode_sensorLimit);
  eeprom_writeInt(EE_ADDR_barrierMode_autofocusTime, barrierMode_autofocusTime);
  eeprom_writeInt(EE_ADDR_barrierMode_shutterLagTime, barrierMode_shutterLagTime);
  eeprom_writeInt(EE_ADDR_barrierMode_preCloseTime, barrierMode_preCloseTime);
  EEPROM.write(EE_ADDR_barrierMode_useFlash1, barrierMode_useFlash1);
  eeprom_writeInt(EE_ADDR_barrierMode_preFlash1Time, barrierMode_preFlash1Time);
  EEPROM.write(EE_ADDR_barrierMode_useFlash2, barrierMode_useFlash2);
  eeprom_writeInt(EE_ADDR_barrierMode_preFlash2Time, barrierMode_preFlash2Time);
  eeprom_writeInt(EE_ADDR_barrierMode_numCicles, barrierMode_numCicles);
  eeprom_writeInt(EE_ADDR_barrierMode_interCicleTime, barrierMode_interCicleTime); 
}



// Load the default flash slave config to ram
void config_loadDefaults_flashSlave() {

  flashSlave_sensorLimit =     DEFAULT_flashSlave_sensorLimit;
  flashSlave_useFlash1   =     DEFAULT_flashSlave_useFlash1;
  flashSlave_preFlash1Time =   DEFAULT_flashSlave_preFlash1Time;
  flashSlave_useFlash2 =       DEFAULT_flashSlave_useFlash2;
  flashSlave_preFlash2Time =   DEFAULT_flashSlave_preFlash2Time;
}

// Save the flash slave config to eeprom
void config_saveBackup_flashSlave(){
  
  eeprom_writeInt(EE_ADDR_flashSlave_sensorLimit, flashSlave_sensorLimit);
  EEPROM.write(EE_ADDR_flashSlave_useFlash1, flashSlave_useFlash1);
  eeprom_writeInt(EE_ADDR_flashSlave_preFlash1Time, flashSlave_preFlash1Time);
  EEPROM.write(EE_ADDR_flashSlave_useFlash2, flashSlave_useFlash2);
  eeprom_writeInt(EE_ADDR_flashSlave_preFlash2Time, flashSlave_preFlash2Time);
 
}

// Load the flash slave config from eeprom to ram
void config_loadBackup_flashSlave(){  
  
  flashSlave_sensorLimit = eeprom_readInt(EE_ADDR_flashSlave_sensorLimit);
  flashSlave_useFlash1 = EEPROM.read(EE_ADDR_flashSlave_useFlash1);
  flashSlave_preFlash1Time = eeprom_readInt(EE_ADDR_flashSlave_preFlash1Time);
  flashSlave_useFlash2 = EEPROM.read(EE_ADDR_flashSlave_useFlash2);
  flashSlave_preFlash2Time = eeprom_readInt(EE_ADDR_flashSlave_preFlash2Time);
  
}
