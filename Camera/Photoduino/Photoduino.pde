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

/* ------------------------------------------------------------------------------------------- *
 *                                                                                             *
 *                                      CONFIGURATION VALUES:                                  *
 *                             You can set the next configuration values                       *
 *                              before compile and upload this sketch                          * 
 *                                                                                             *
 * ------------------------------------------------------------------------------------------- */

//
// Language of menus:
// - Set the language of menus in LCD
// 
// Valid values: 
// - LANGUAGE_ES for (Spanish) 
// - LANGUAGE_EN for (English)
//
#define LANGUAGE_EN  

//
// Default Shutterlag:
// - The default value for shutterlag to load when you reset configs
// 
// Valid values:
// - Number between 0 and 59999 in miliseconds
// - Check your camera model at: http://code.google.com/p/photoduino/wiki/CameraReference
//
#define DEFAULT_SHUTTERLAG               100 

// 
// Default intercicle time:
// - The default value in miliseconds camera needs to process photo and be ready for the next
// 
// Valid values:
// - Number between 0 and 59999 in miliseconds
//
#define DEFAULT_INTERCICLE_TIME          200  

// 
// Pulse width for shooting flashes:
// - Time in microseconds of the pulse to shoot the flashes.
// - Increase this value if you have problems with your flash model (very rare).
// 
// Valid values:
// - Number between 0 and 59999 in microseconds
// 
#define DEVICES_FLASHES_SHOOTING_PULSE     1 

// 
// Camera mirror lock-up time limit:
// - When use the mirror lock-up shooting mode, all cameras has a limit time (about 30 seconds) to 
//   make the photo while the mirror is locked up. After that time, the mirror is automaticaly unlocked
//   by the camera without taking photo. The next value is this time in miliseconds to say to photoduino 
//   when to re-lock-up the mirror if no photo has taken.
// 
// Valid values:
// - Number between 0 and 59999 in miliseconds
//
#define DEVICES_CAMERA_MIRROR_LOCKUP_TIMELIMIT         31000

// 
// Default sensor tunning mode:
// - When you set the limit of a sensor you have two modes available. Visual and numeric. This is the 
//   default value but also you can change this preference through the system config menu.
// 
// Valid values:
// - SENSORLIMIT_VISUAL
// - SENSORLIMIT_NUMERIC
//
#define DEFAULT_system_interfaceSensorLimit           SENSORLIMIT_VISUAL

//
// Arduino board type:
// - Make the firmware compatible with different boards
// 
// Valid values: 
// - BOARD_ARDUINO_DUEMILANOVE (With Atmega328)
// - BOARD_ARDUINO_MEGA
//
#define BOARD_ARDUINO_DUEMILANOVE

/* ------------------------------------------------------------------------------------------- *
 *                                                                                             *
 *                                     IMPORTANT NOTE:                                         *
 *                        You don't need to modify the code below this lines                   * 
 *                                                                                             *
 * ------------------------------------------------------------------------------------------- */


// Import libraries 
#include <LiquidCrystal.h>  
#include <EEPROM.h>

// Signature and version codes 
#define SIGNATURE_CODE1             0x21 // day 
#define SIGNATURE_CODE2             0x08 // month
#define SIGNATURE_CODE3             0x20 // century
#define SIGNATURE_CODE4             0x10 // year of century
#define CODE_MAYOR_VERSION          0x00 // mayor version
#define CODE_MINOR_VERSION          0x09 // minor version

// Default system config
#define DEFAULT_system_useBacklight                   true
#define DEFAULT_system_useSpeaker                     true


// Default interval mode config
#define DEFAULT_intervalMode_autofocusTime            0
#define DEFAULT_intervalMode_shutterLagTime           DEFAULT_SHUTTERLAG
#define DEFAULT_intervalMode_useFlash1                false
#define DEFAULT_intervalMode_preFlash1Time            0
#define DEFAULT_intervalMode_useFlash2                false
#define DEFAULT_intervalMode_preFlash2Time            0
#define DEFAULT_intervalMode_preCloseTime             0
#define DEFAULT_intervalMode_intervalUnits            UNITS_SECS
#define DEFAULT_intervalMode_intervalValue            1
#define DEFAULT_intervalMode_numCicles                0

// Audio trigger mode default config
#define DEFAULT_audioTriggerMode_sensorLimit          512
#define DEFAULT_audioTriggerMode_shootingMode         SHOOTINGMODE_NORMAL
#define DEFAULT_audioTriggerMode_autofocusTime        0
#define DEFAULT_audioTriggerMode_shutterLagTime       DEFAULT_SHUTTERLAG
#define DEFAULT_audioTriggerMode_preCloseTime         0
#define DEFAULT_audioTriggerMode_useFlash1            false
#define DEFAULT_audioTriggerMode_preFlash1Time        0
#define DEFAULT_audioTriggerMode_useFlash2            false
#define DEFAULT_audioTriggerMode_preFlash2Time        0
#define DEFAULT_audioTriggerMode_numCicles            0
#define DEFAULT_audioTriggerMode_interCicleTime       DEFAULT_INTERCICLE_TIME

// Shock trigger mode default config
#define DEFAULT_shockTriggerMode_sensorLimit          512
#define DEFAULT_shockTriggerMode_shootingMode         SHOOTINGMODE_NORMAL
#define DEFAULT_shockTriggerMode_autofocusTime        0
#define DEFAULT_shockTriggerMode_shutterLagTime       DEFAULT_SHUTTERLAG
#define DEFAULT_shockTriggerMode_preCloseTime         0
#define DEFAULT_shockTriggerMode_useFlash1            false
#define DEFAULT_shockTriggerMode_preFlash1Time        0
#define DEFAULT_shockTriggerMode_useFlash2            false
#define DEFAULT_shockTriggerMode_preFlash2Time        0
#define DEFAULT_shockTriggerMode_numCicles            0
#define DEFAULT_shockTriggerMode_interCicleTime       DEFAULT_INTERCICLE_TIME

// Barrier mode default config
#define DEFAULT_barrierMode_sensorLimit               512
#define DEFAULT_barrierMode_shootingMode              SHOOTINGMODE_NORMAL
#define DEFAULT_barrierMode_autofocusTime             0
#define DEFAULT_barrierMode_shutterLagTime            DEFAULT_SHUTTERLAG
#define DEFAULT_barrierMode_preCloseTime              0
#define DEFAULT_barrierMode_useFlash1                 false
#define DEFAULT_barrierMode_preFlash1Time             0
#define DEFAULT_barrierMode_useFlash2                 false
#define DEFAULT_barrierMode_preFlash2Time             0
#define DEFAULT_barrierMode_numCicles                 0
#define DEFAULT_barrierMode_interCicleTime            DEFAULT_INTERCICLE_TIME

// Flash slave default config
#define DEFAULT_flashSlave_sensorLimit                512
#define DEFAULT_flashSlave_useFlash1                  true
#define DEFAULT_flashSlave_preFlash1Time              0
#define DEFAULT_flashSlave_useFlash2                  true
#define DEFAULT_flashSlave_preFlash2Time              0

// LCD settings
#define LCD_COLS             16 // columns
#define LCD_ROWS             2  // rows

// Pinout settings
#define PINS_LCD_LED         13 //(digital pin)
#define PINS_LCD_RS          12 //(digital pin)
#define PINS_LCD_ENABLE      11 //(digital pin)
#define PINS_LCD_DB4         10 //(digital pin)
#define PINS_LCD_DB5         9  //(digital pin)
#define PINS_LCD_DB6         8  //(digital pin)
#define PINS_LCD_DB7         7  //(digital pin)
#define PINS_BUZZER          6  //(pwm6 pin)
#define PINS_SHUTTER         5  //(digital pin)
#define PINS_AUTOFOCUS       4  //(digital pin)

#ifdef BOARD_ARDUINO_MEGA // For ArduinoMega compatibility
  #define PINS_FLASH1          56 // 56 = Mega 16 = Duemilanova //(digital pin)
  #define PINS_FLASH2          55 // 55 = Mega 15 = Duemilanova //(digital pin)
  #define PINS_RESERVED        54 // 54 = Mega 14 = Duemilanova //(digital pin)
#else  
  #define PINS_FLASH1          16 //(digital pin)
  #define PINS_FLASH2          15 //(digital pin)
  #define PINS_RESERVED        14 //(digital pin)
#endif

#define PINS_LASER           PINS_RESERVED  // alias(digital pin)
#define PINS_BTN_A           3  //(digital pin)
#define PINS_BTN_B           2  //(digital pin)
#define PINS_SENSOR_SHOCK    5  //(analog pin)
#define PINS_SENSOR_BARRIER  4  //(analog pin)
#define PINS_SENSOR_MIC      3  //(analog pin)

// Measure units
#define UNITS_MS             0  // miliseconds
#define UNITS_SECS           1  // seconds
#define UNITS_MINS           2  // minutes
#define UNITS_HOURS          3  // hours
#define UNITS_DAYS           4  // days
#define UNITS_CICLES         10 // cicles
#define UNITS_US             11 // microseconds

// Sensor limit modes
#define SENSOR_MODE_LOWER    0 // for trigg by lower values
#define SENSOR_MODE_HIGHER   1 // for trigg by higher values

// Shooting modes
#define SHOOTINGMODE_NORMAL           0 // Normal mode 
#define SHOOTINGMODE_PREBULB          1 // Prebulb mode
#define SHOOTINGMODE_MIRRORLOCKUP     2 // Mirror lookup mode

// Interface modes
#define SENSORLIMIT_VISUAL      0 // Visual 
#define SENSORLIMIT_NUMERIC     1 // Visual 

// Keycodes
#define NO_KEY               0 // No keys pressed
#define KEY_A                1 // Button A was pressed
#define KEY_B                2 // Button B was pressed
#define KEY_AH               3 // Button A was pressed and holded (KEY_HOLD_TIME) milisecons
#define KEY_BH               4 // Button B was pressed and holded (KEY_HOLD_TIME) milisecons

// Keyboard times
#define KEY_DEBOUNCE_TIME    30 // debounce time (ms) to prevent flickering when pressing or releasing the button
#define KEY_HOLD_TIME       400 // holding period (ms) how long to wait for press+hold event

// Characters and symbols addreses on lcd eeprom
#define SYMBOL_DOWN         0x00
#define SYMBOL_UP           0x01
#define SYMBOL_BOX          0xff

// EEPROM size in bytes
#define EEPROM_SIZE         512

// EEPROM Addreses for signature code and version of firmware
#define EE_ADDR_SIGNATURE_CODE1                     0x00 // BYTE
#define EE_ADDR_SIGNATURE_CODE2                     0x01 // BYTE
#define EE_ADDR_SIGNATURE_CODE3                     0x02 // BYTE
#define EE_ADDR_SIGNATURE_CODE4                     0x03 // BYTE
#define EE_ADDR_CODE_MAYOR_VERSION                  0x04 // BYTE
#define EE_ADDR_CODE_MINOR_VERSION                  0x05 // BYTE

// EEPROM Addreses for system config
#define EE_ADDR_system_useBacklight                 0x10 // BYTE
#define EE_ADDR_system_useSpeaker                   0x12 // BYTE
#define EE_ADDR_system_interfaceSensorLimit         0x14 // BYTE

// EEPROM Addreses for interval mode config
#define EE_ADDR_intervalMode_autofocusTime          0x20 // WORD
#define EE_ADDR_intervalMode_shutterLagTime         0x22 // WORD
#define EE_ADDR_intervalMode_useFlash1              0x24 // BYTE
#define EE_ADDR_intervalMode_preFlash1Time          0x26 // WORD
#define EE_ADDR_intervalMode_useFlash2              0x28 // BYTE
#define EE_ADDR_intervalMode_preFlash2Time          0x2A // WORD
#define EE_ADDR_intervalMode_preCloseTime           0x2C // WORD
#define EE_ADDR_intervalMode_intervalUnits          0x2E // BYTE
#define EE_ADDR_intervalMode_intervalValue          0x30 // WORD
#define EE_ADDR_intervalMode_numCicles              0x32 // WORD

// EEPROM Addreses for audio trigger mode config
#define EE_ADDR_audioTriggerMode_sensorLimit        0x40 // WORD
#define EE_ADDR_audioTriggerMode_shootingMode       0x42 // BYTE
#define EE_ADDR_audioTriggerMode_autofocusTime      0x44 // WORD
#define EE_ADDR_audioTriggerMode_shutterLagTime     0x46 // WORD
#define EE_ADDR_audioTriggerMode_preCloseTime       0x48 // WORD
#define EE_ADDR_audioTriggerMode_useFlash1          0x4A // BYTE
#define EE_ADDR_audioTriggerMode_preFlash1Time      0x4C // WORD
#define EE_ADDR_audioTriggerMode_useFlash2          0x4E // BYTE
#define EE_ADDR_audioTriggerMode_preFlash2Time      0x50 // WORD
#define EE_ADDR_audioTriggerMode_numCicles          0x52 // WORD
#define EE_ADDR_audioTriggerMode_interCicleTime     0x54 // WORD

// EEPROM Addreses for shock trigger mode config
#define EE_ADDR_shockTriggerMode_sensorLimit        0x60 // WORD
#define EE_ADDR_shockTriggerMode_shootingMode       0x62 // BYTE
#define EE_ADDR_shockTriggerMode_autofocusTime      0x64 // WORD
#define EE_ADDR_shockTriggerMode_shutterLagTime     0x66 // WORD
#define EE_ADDR_shockTriggerMode_preCloseTime       0x68 // WORD
#define EE_ADDR_shockTriggerMode_useFlash1          0x6A // BYTE
#define EE_ADDR_shockTriggerMode_preFlash1Time      0x6C // WORD
#define EE_ADDR_shockTriggerMode_useFlash2          0x6E // BYTE
#define EE_ADDR_shockTriggerMode_preFlash2Time      0x70 // WORD
#define EE_ADDR_shockTriggerMode_numCicles          0x72 // WORD
#define EE_ADDR_shockTriggerMode_interCicleTime     0x74 // WORD

// EEPROM Addreses for barrier mode config
#define EE_ADDR_barrierMode_sensorLimit             0x80 // WORD
#define EE_ADDR_barrierMode_shootingMode            0x82 // BYTE
#define EE_ADDR_barrierMode_autofocusTime           0x84 // WORD
#define EE_ADDR_barrierMode_shutterLagTime          0x86 // WORD
#define EE_ADDR_barrierMode_preCloseTime            0x88 // WORD
#define EE_ADDR_barrierMode_useFlash1               0x8A // BYTE
#define EE_ADDR_barrierMode_preFlash1Time           0x8C // WORD
#define EE_ADDR_barrierMode_useFlash2               0x8E // BYTE
#define EE_ADDR_barrierMode_preFlash2Time           0x90 // WORD
#define EE_ADDR_barrierMode_numCicles               0x92 // WORD
#define EE_ADDR_barrierMode_interCicleTime          0x94 // WORD

// EEPROM Addreses for flash slave config
#define EE_ADDR_flashSlave_sensorLimit              0xA0 // WORD
#define EE_ADDR_flashSlave_useFlash1                0xA2 // BYTE
#define EE_ADDR_flashSlave_preFlash1Time            0xA4 // WORD
#define EE_ADDR_flashSlave_useFlash2                0xA6 // BYTE
#define EE_ADDR_flashSlave_preFlash2Time            0xA8 // WORD

 
#ifdef LANGUAGE_ES
  // Messages list                      "123456789012345"
  #define MSG_PHOTODUINO_V 	        "Photoduino v"
  #define MSG_READY 		        "Listo!"
  #define MSG_MAIN_MENU 	        "Menu principal"
  #define MSG_SHOCK_TRIGGER 	        "Sensor Choque"
  #define MSG_BARRIER_MODE 	        "Sensor Barrera"
  #define MSG_INTERVAL_MODE 	        "Intervalometro"
  #define MSG_AUDIO_TRIGGER 	        "Sensor Audio"
  #define MSG_FLASH_SLAVE               "Flash esclavo"
  #define MSG_AUDIO_CONFIG 	        "Config Audio"
  #define MSG_SHOCK_CONFIG 	        "Config Choque"
  #define MSG_BARRIER_CONFIG 	        "Config Barrera"
  #define MSG_SYSTEM_CONFIG 	        "Config Sistema"
  #define MSG_CONFIG 		        "Configurar"
  #define MSG_RUN 		        "Ejecutar"
  #define MSG_RUN_AUDIO                 "Ejecuta audio"
  #define MSG_RUN_INTERVAL              "Ejecuta intervalo"
  #define MSG_RUN_BARRIER               "Ejecuta barrera"
  #define MSG_RUN_SHOCK                 "Ejecuta choque"
  #define MSG_RUN_FLASH_SLAVE           "Ejecuta esclavo"
  #define MSG_RESET_CONFIG 	        "Borrar config"
  #define MSG_AUTOFOCUS_TIME 	        "Tiempo enfoque"
  #define MSG_SHUTTERLAG_TIME 	        "Retardo obturador"
  #define MSG_USE_FLASH1 	        "Usar Flash1"
  #define MSG_USE_FLASH2 	        "Usar Flash2"
  #define MSG_FLASH1_PRETIME 	        "Flash1 pretiempo"
  #define MSG_FLASH2_PRETIME 	        "Flash2 pretiempo"
  #define MSG_CLOSE_PRETIME 	        "Cerrar pretiempo"
  #define MSG_INTERVAL_UNITS 	        "Unid. Intervalo"
  #define MSG_INTERVAL_VALUE 	        "Valor Intervalo"
  #define MSG_CICLES 		        "Ciclos"
  #define MSG_SENSOR_LIMIT 	        "Limite sensor"
  #define MSG_SENSOR      	        ">Sensor :"
  #define MSG_LIMIT      	        ">Limite :"
  #define MSG_SHOCK_LIMIT 	        "Limite choque"
  #define MSG_BARRIER_LIMIT 	        "Limite barrera"*/
  #define MSG_SHOOTING_MODE             "Modo de disparo"
  #define MSG_INTERCICLE_TIME 	        "Tiempo ciclos"
  #define MSG_BACKLIGHT 	        "Luz LCD"
  #define MSG_SPEAKER 		        "Pitidos"
  #define MSG_RESETTING 	        "Reiniciando.."
  #define MSG_ABORTING 		        "Cancelando.."
  #define MSG_YES 		        "SI"
  #define MSG_NO		        "NO"
  #define MSG_UNITS_US		        "us"
  #define MSG_UNITS_MS		        "ms"
  #define MSG_UNITS_SECS	        "segundos"
  #define MSG_UNITS_MINS                "minutos"
  #define MSG_UNITS_HOURS	        "horas"
  #define MSG_UNITS_DAYS	        "dias"
  #define MSG_UNITS_CICLES	        "ciclos"
  #define MSG_SHOOTINGMODE_NORMAL       "Normal"
  #define MSG_SHOOTINGMODE_PREBULB      "Bulb previo"     
  #define MSG_SHOOTINGMODE_MIRRORLOCKUP "Levanta espejo"
  #define MSG_VISUAL_MODE               "Visual"
  #define MSG_NUMERIC_MODE              "Numerico"
  #define MSG_INTERFACE_SENSOR_MODE     "Ajuste Sensores"
  
#else
  // Messages list                      "123456789012345"
  #define MSG_PHOTODUINO_V 	        "Photoduino v"
  #define MSG_READY 		        "Ready!"
  #define MSG_MAIN_MENU 	        "Main menu"
  #define MSG_SHOCK_TRIGGER 	        "Shock trigger"
  #define MSG_BARRIER_MODE 	        "Barrier mode"
  #define MSG_INTERVAL_MODE 	        "Interval mode"
  #define MSG_AUDIO_TRIGGER 	        "Audio trigger"
  #define MSG_FLASH_SLAVE               "Flash slave"
  #define MSG_AUDIO_CONFIG 	        "Audio Config"
  #define MSG_SHOCK_CONFIG 	        "Shock Config"
  #define MSG_BARRIER_CONFIG 	        "Barrier Config"
  #define MSG_SYSTEM_CONFIG 	        "System config"
  #define MSG_CONFIG 		        "Config"
  #define MSG_RUN 		        "Run"
  #define MSG_RUN_AUDIO                 "Run audio"
  #define MSG_RUN_INTERVAL              "Run interval"
  #define MSG_RUN_BARRIER               "Run barrier"
  #define MSG_RUN_SHOCK                 "Run shock"
  #define MSG_RUN_FLASH_SLAVE           "Run flashslave"
  #define MSG_RESET_CONFIG 	        "Reset config"
  #define MSG_AUTOFOCUS_TIME 	        "Autofocus time"
  #define MSG_SHUTTERLAG_TIME 	        "Shutterlag time"
  #define MSG_USE_FLASH1 	        "Use Flash1"
  #define MSG_USE_FLASH2 	        "Use Flash2"
  #define MSG_FLASH1_PRETIME 	        "Flash1 pretime"
  #define MSG_FLASH2_PRETIME 	        "Flash2 pretime"
  #define MSG_CLOSE_PRETIME 	        "Close pretime"
  #define MSG_INTERVAL_UNITS 	        "Interval units"
  #define MSG_INTERVAL_VALUE 	        "Interval value"
  #define MSG_CICLES 		        "Cicles"
  #define MSG_SENSOR_LIMIT 	        "Sensor limit"
  #define MSG_SENSOR      	        ">Sensor :"
  #define MSG_LIMIT      	        ">Limit  :"
  #define MSG_SHOOTING_MODE	        "Shooting mode"
  #define MSG_INTERCICLE_TIME 	        "Intercicle time"
  #define MSG_BACKLIGHT 	        "Backlight"
  #define MSG_SPEAKER 		        "Speaker"
  #define MSG_RESETTING 	        "Resetting..."
  #define MSG_ABORTING 		        "Aborting..."
  #define MSG_YES 		        "YES"
  #define MSG_NO		        "NO"
  #define MSG_UNITS_US		        "us"
  #define MSG_UNITS_MS		        "ms"
  #define MSG_UNITS_SECS	        "seconds"
  #define MSG_UNITS_MINS	        "minutes"
  #define MSG_UNITS_HOURS	        "hours"
  #define MSG_UNITS_DAYS	        "days"
  #define MSG_UNITS_CICLES	        "cicles"
  #define MSG_SHOOTINGMODE_NORMAL       "Normal"
  #define MSG_SHOOTINGMODE_PREBULB      "Pre-Bulb Mode"     
  #define MSG_SHOOTINGMODE_MIRRORLOCKUP "Mirror Lock-up"
  #define MSG_VISUAL_MODE               "Visual"
  #define MSG_NUMERIC_MODE              "Numeric"
  #define MSG_INTERFACE_SENSOR_MODE     "Sensor tunning"
#endif



// LiquidCrystal LCD control object instance
LiquidCrystal lcd(PINS_LCD_RS, PINS_LCD_ENABLE, PINS_LCD_DB4, PINS_LCD_DB5, PINS_LCD_DB6, PINS_LCD_DB7);

// Variables used on interrupt mode
volatile boolean cancelFlag = false;    // Flag used to abort interrupt mode

// Global variables
byte         lastKey = NO_KEY;          // Last key pressed
boolean      flagHoldKey = false;       // Flag to ignore keys after a hold key

// System config in ram
boolean      system_useBacklight;    
boolean      system_useSpeaker;
byte         system_interfaceSensorLimit;

// Interval mode config in ram
unsigned int intervalMode_autofocusTime;
unsigned int intervalMode_shutterLagTime;
boolean      intervalMode_useFlash1;
unsigned int intervalMode_preFlash1Time;
boolean      intervalMode_useFlash2;
unsigned int intervalMode_preFlash2Time;
unsigned int intervalMode_preCloseTime;
byte         intervalMode_intervalUnits;
unsigned int intervalMode_intervalValue;
unsigned int intervalMode_numCicles;

// Audio trigger mode config in ram
unsigned int audioTriggerMode_sensorLimit;
byte         audioTriggerMode_shootingMode;
unsigned int audioTriggerMode_autofocusTime;
unsigned int audioTriggerMode_shutterLagTime;
unsigned int audioTriggerMode_preCloseTime;
boolean      audioTriggerMode_useFlash1;
unsigned int audioTriggerMode_preFlash1Time;
boolean      audioTriggerMode_useFlash2;
unsigned int audioTriggerMode_preFlash2Time;
unsigned int audioTriggerMode_numCicles;
unsigned int audioTriggerMode_interCicleTime;

// Shock trigger mode config in ram
unsigned int shockTriggerMode_sensorLimit;
byte         shockTriggerMode_shootingMode;
unsigned int shockTriggerMode_autofocusTime;
unsigned int shockTriggerMode_shutterLagTime;
unsigned int shockTriggerMode_preCloseTime;
boolean      shockTriggerMode_useFlash1;
unsigned int shockTriggerMode_preFlash1Time;
boolean      shockTriggerMode_useFlash2;
unsigned int shockTriggerMode_preFlash2Time;
unsigned int shockTriggerMode_numCicles;
unsigned int shockTriggerMode_interCicleTime;

// Barrier mode config in ram
unsigned int barrierMode_sensorLimit;
byte         barrierMode_shootingMode;
unsigned int barrierMode_autofocusTime;
unsigned int barrierMode_shutterLagTime;
unsigned int barrierMode_preCloseTime;
boolean      barrierMode_useFlash1;
unsigned int barrierMode_preFlash1Time;
boolean      barrierMode_useFlash2;
unsigned int barrierMode_preFlash2Time;
unsigned int barrierMode_numCicles;
unsigned int barrierMode_interCicleTime;

// Flash slave config in ram
unsigned int flashSlave_sensorLimit;
boolean      flashSlave_useFlash1;
unsigned int flashSlave_preFlash1Time;
boolean      flashSlave_useFlash2;
unsigned int flashSlave_preFlash2Time;


// Setups at startup
void setup() 
{   
  // Create custom LCD symbols
  byte char_arrow_down[8] = { B00000, B00100, B00100, B00100, B00100, B10101, B01110, B00100 };
  byte char_arrow_up[8]   = { B00100, B01110, B10101, B00100, B00100, B00100, B00100, B00000 };
   
  // LCD initialize
  lcd.begin( LCD_COLS,  LCD_ROWS);
  lcd.createChar(SYMBOL_DOWN, char_arrow_down);
  lcd.createChar(SYMBOL_UP, char_arrow_up);

  // Pinmode inputs
  pinMode(PINS_BTN_A, INPUT);     
  pinMode(PINS_BTN_B, INPUT);    
  pinMode(PINS_SENSOR_SHOCK, INPUT); 
  pinMode(PINS_SENSOR_BARRIER, INPUT);
  pinMode(PINS_SENSOR_MIC, INPUT);
  
  // Pinmode outputs
  pinMode(PINS_SHUTTER, OUTPUT);
  pinMode(PINS_AUTOFOCUS, OUTPUT);
  pinMode(PINS_FLASH1, OUTPUT);
  pinMode(PINS_FLASH2, OUTPUT);
  pinMode(PINS_LCD_LED, OUTPUT);
  pinMode(PINS_LCD_RS, OUTPUT);
  pinMode(PINS_LCD_ENABLE, OUTPUT);
  pinMode(PINS_LCD_DB4, OUTPUT);
  pinMode(PINS_LCD_DB5, OUTPUT);
  pinMode(PINS_LCD_DB6, OUTPUT);
  pinMode(PINS_LCD_DB7, OUTPUT);
  pinMode(PINS_BUZZER, OUTPUT);
  pinMode(PINS_RESERVED, OUTPUT);
  
  // Initialize config
  config_init(); 
  
  // Init backlight
  digitalWrite(PINS_LCD_LED, system_useBacklight); 
  
  // Turn off laser
  laser_turnOff();

}

// Run manager
void loop(){ 
   controller_run();
}


