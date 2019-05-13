// THIS CODE SHOWS YOU HOW TO CONTROL ALL FEATURES ON THE COMMS SHIELD
#include <SPI.h>    // include the SPI communcation library
#include "LiquidCrystal.h" 
LiquidCrystal lcd(4,5,6,7,8,9); //LCD pins RS/E/D4/D5/D6/D7 connected to UNO pins 4,5,6,7,8,9 Respectively
#define LED1 1  // First LED controlled by GPIO1
#define LCDen 3 // LCB backlight control by GPIO3
#define LED2 2  // Second LED controlled by GPIO2
// Connect pin#13 (SCLK) To S2 on AP23 
// Connect pin#11 (DATOUT) To S3 on AP23 
// Connect pin#13 (SCLK) To S2 on AP23 
// Connect pin#10 (CS) To S1 on AP23
int cs=10;          // this is the chip-select pin for SPI
int del=200;        // short 200ms delay
int value1 = 0x98;  // play command - This value never changes
int value2 = 0;  // voice address - when you place a hex value in this integer and call the "readout()" function, that specific word will play
int value3 = 0xA8;  // COUT ramp up - This value never changes
int value4 = 0xB8;   // COUT ramp down - This value never changes
/*
 * // If you were to play sentences, this is how it would look.  Check out the look up table on the project page for the LBT library
int sentence[] = {0xc5, 0xc7, 0xa5,0xfc}; // hello how are you
int sentenceb[] = {0xb6, 0xf8, 0xbd, 0xb4}; // Crital warning... fatal condition...
int sentencec[] = {0x3d, 0x9c, 0x39, 0x98, 0x40}; // Seven plus three equals ten
int sentenced[] = {0x91, 0x90, 0x0a, 0x92, 0x93}; // Intruder detected activate security system
int sentencee[] = {0xc8, 0xa3, 0x61}; // I am angry
int sentencef[] = {0xf2, 0x8d, 0xcd, 0x4a, 0x3d, 0x84}; // The weight is 27 pounds
int sentenceg[] = {0xf2, 0x33, 0xcd, 0x38, 0x4b, 0x3b, 0x2d}; // The time is two thirty five pm 
int sentenceh[] = {0x0f, 0x55, 0x3c, 0x74, 0x79}; // move north six kilometers 
int sentencei[] = {0xf3, 0x33, 0xcd, 0x42, 0x4c, 0x2d}; // The time is 12:40 pm - Breaks down to "the (0x86)", "time (0x61)", "is (0x83)"...ETC
int sentencej[] = {0xf3, 0x2e, 0xcd, 0x27, 0x19, 0x41, 0x38, 0x53, 0x48}; // The date is friday march 11/2016 || (two, thousand, sixeeen) = Three words
int sentencek[] = {0x91, 0x90, 0x0a, 0x92, 0x8e}; // Intruder detected.  Enable security alarm
*/

void setup()  
{

  lcd.begin(16,02);              // 16 characters/2 lines
  lcd.clear();                   // Clear LCD
  pinMode(LCDen,OUTPUT);         // Declare I/Os
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  digitalWrite(LCDen,HIGH);      // Turn backlight on, and LEDs off
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  delay(10);
  lcd.print(" PAT MITCHELL'S"); //the command to print to the LCD
  lcd.setCursor(0, 1); //sets the cursor to the 0th column in the 2nd row
  lcd.print("ARDUINO SHIELD!");
  delay(100);              // 100ms delay
  SPI.begin();             // Initialize SPI
  SPI.setClockDivider(SPI_CLOCK_DIV32); // low frequency SPI
  pinMode(cs,OUTPUT);    // Chip select pins is an output
  digitalWrite(cs,HIGH); // Set chip select to be HIGH (5v) by default.  The chip on the shield is selected when this line is brought low. 
  SPI.setBitOrder(MSBFIRST);  // OTP requires MSB first
  SPI.setDataMode(SPI_MODE0);  // Use MODE0, as all other modes to not work
  value2 = 0x00; // Start at voice group 0
  delay(500);   // One second delay
  // ramp up Little Buddy Talker chip
  digitalWrite(cs,LOW);
  SPI.transfer(value3);
  SPI.transfer(0x00);
  digitalWrite(cs,HIGH);
  delay(5000);
}

// the loop function runs over and over again forever
void loop() {
  readout();     // SPI command transfer function.  Whenever you call this function, it calls a word to be played. LEDs will blink, LCD written to, and backlight turned on/off
  delay(50);     // 50ms delay after word played
  value2 = value2 + 1;  // Increment voice command group so that the next word int he queue will play next
}

void readout()
{
  
  delay(7); // 7ms delay
  // Transmit Data
  lcd.print("PLAYING WORD#"); //the command to print to the LCD
  lcd.setCursor(0, 1); //sets the cursor to the 0th column in the 1st row
  lcd.print(value2);   //tell the user which word is being played 
  delay(700);          // 700ms delay
  digitalWrite(cs,LOW);   // Select the Little Buddy Talker
  SPI.transfer(value1);   // Send fixed value1 integer to LBT
  SPI.transfer(value2);   // Send word address and play
  digitalWrite(cs,HIGH);  // Unselect LBT
  digitalWrite(LED1,HIGH); // Turn both LEDs on
  digitalWrite(LED2,HIGH);
  delay(500);             // Half second delay, then turn both LEDs off
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  lcd.clear();            // Clear the LCD, and turn the backlight off for 600ms
  digitalWrite(LCDen,LOW);
  delay(600);
  digitalWrite(LCDen,HIGH); // Turn the backlight on again and end the function
}



   

   




   



