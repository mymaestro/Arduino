// The TROLL
#include <SPI.h>    // include the SPI communcation library
// Connect pin#13 (SCLK) To S2 on AP23 
// Connect pin#11 (DATOUT) To S3 on AP23 
// Connect pin#13 (SCLK) To S2 on AP23 
// Connect pin#10 (CS) To S1 on AP23
#define SW4 5 // DIP Switches
#define SW3 6
#define SW2 7
#define SW1 8
#define LED 2
#define TRIGGER 9
#define SEL 0
boolean buggerstate = false;
int state = 0;
int program = 0;
int cs=10;          // this is the chip-select pin for SPI
int del=200;        // short 200ms delay
int value1 = 0x98;  // play command - This value never changes
int value2 = 0;  // voice address - when you place a hex value in this integer and call the "readout()" function, that specific word will play
int value3 = 0xA8;  // COUT ramp up - This value never changes
int value4 = 0xB8;   // COUT ramp down - This value never changes
int rampstate = 0;
int buttonstate = 0;
int soundbite = 0;
// ALL OF THE ABOE CODE IS REQUIRED FOR THE OPERATION OF THE CHIP


void setup()  // Set up SPI
{
  pinMode(SW1,INPUT);
  pinMode(SW2,INPUT);
  pinMode(SW3,INPUT);
  pinMode(SW4,INPUT);
  pinMode(SEL,INPUT);
  pinMode(TRIGGER,INPUT);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  SPI.begin();             // Initialize SPI
  SPI.setClockDivider(SPI_CLOCK_DIV32); // low frequency SPI
  pinMode(cs,OUTPUT);    // Chip select pins is an output
  digitalWrite(cs,HIGH); // Set chip select to be HIGH (5v) by default.  The chip on the shield is selected when this line is brought low. 
  SPI.setBitOrder(MSBFIRST);  // OTP requires MSB first
  SPI.setDataMode(SPI_MODE0);  // Use MODE0, as all other modes to not work
  value2 = 0x00; // Start at voice group 0
  switchstate();
  if(rampstate == 1)
  {
    rampup();
  }
  delay(500);   // One second delay
  // ramp up Little Buddy Talker
  //digitalWrite(cs,LOW);
  //SPI.transfer(value3);
  //SPI.transfer(0x00);
  //digitalWrite(cs,HIGH);
   //blink();
}


//////////////////////////////////////////////////////////SELECT PROGRAM
void loop() // Based on the DIP switch settings, this routine will select the program that you enter on power up.
{
//test(); //IF YOU COMMENT THIS FUNCTION BACK IN, YOU'LL PLAY ALL SOUND BITES IN ORDER.  YOU'LL NEED TO COMMENT OUT THE REST OF VOID LOOP THOUGH.
//}  // IF YOU WISH TO USE THE ABOVE TEST ROUTING, COMMENT THIS "}" BACK IN, WHICH WILL END THE VOID LOOP
  if(program == 0)
  {
  lightsout();  //PROGRAM1
  }
  if(program == 1)
  {
    bugger();   //PROGRAM2
  }
  if(program == 2)
  {
    if(value2 == 0)
    {
      value2 = 0x16; // was 1c
    }
    external(); //PROGRAM3
  }
  if(program == 3)
  {
  haunting();   //PROGRAM4
  }
}



//////////////////////////////////////////////////////////RAMP AND READOUT.  

void readout()
{
  if(rampstate == 0) // IF THE SPEAKER IS SELECTED, WE NEED TO RAMP UP, PLAY THE SOUND, THEN RAMP DOWN TO SAVE POWER.  IF THE EXTERNAL JACK IS USED, WE RAMP UP ONCE, AND NEVER NEED TO RAMP DOWN.
  {
  rampup();
  }
  delay(5);
  // Transmit Data
  digitalWrite(cs,LOW);   // Select the Little Buddy Talker
  SPI.transfer(value1);   // Send fixed value1 integer to LBT
  SPI.transfer(value2);   // Send word address and play
  digitalWrite(cs,HIGH);  // Unselect LBT
  readydelay();
  if(rampstate == 0)
  {
  rampdown();
  }
  // Another word can be played.  Play around with this delay if you wish.
  //rampdown();
}


void rampup() // TURNS ON THE AUDIO CHIP.  ENABLES AUDIO OUTPUT
{
   digitalWrite(cs,LOW);
  SPI.transfer(value3);
  SPI.transfer(0x00);
  digitalWrite(cs,HIGH);
}

void rampdown() // TURNS OFF THE AUDIO CHIP.  DISABLES AUDIO OUTPUT
{
   digitalWrite(cs,LOW);
  SPI.transfer(value4);
  SPI.transfer(0x00);
  digitalWrite(cs,HIGH);
}

//////////////////////////////////////////////////////////DIP SWITCH SETTINGS
void switchstate()
{
  if(digitalRead(SW1) == HIGH) // IF SW1 IS HIGH, WE'LL BE CHOOSING THE AUDIO JACKS.  IF LOW, WE'RE USING A SPEAKER.  THIS IS IMPORTANT FORS POWER SAVING.
  {
    rampstate = 1; // external audio
  }
  if(digitalRead(SW2) == LOW)
  {
    state = state + 1;  // we're not using this DIP switch right now.  int "state" can be used for whatever you like
  }
  if(digitalRead(SW3) == LOW)
  {
    program = program + 1; // determines what program we're going to start in.  
  }
  if(digitalRead(SW4) == LOW)
  {
    program = program + 2; // Same as above
  }
  if(program == 0)
   {
    value2 = 0x09; // If we're starting in program1 (program = 0), point to the 0x09 sound bite ahead of time, so that the program knows to start there.
   }
}

//////////////////////////////////////////////////////////MAIN PROGRAMS
void lightsout() // TURN OFF THE LIGHT TO START THE PRORGRAM.  PLAY A REALLY LOUD SOUND BITE WHEN THE LIGHT TURNS BACK ON.
{
  if(analogRead(0) > 900) // THIS SAMPLES THE LIGHT SENSOR.  IF THE LIGHTS ARE OFF, WAIT TWO SECONDS, THEN TEST AGAIN.
  {
  delay(2000);
  if(analogRead(0) > 900) // AFTER TWO SECONDS TEST AGAIN.  IF THE LIGHT ARE STILL OFF, BLINK THE LED, AND START THE PROGRAM
  {
  blink();                // BLINKING LED FUNCTION
  while(analogRead(0) > 900) { // DO NOTHING UNTIL THE LIGHTS TURN BACK ON
  {}
  }
  readout();                   // PLAY THE SCREAM 0X09 SOUND BITE FIRST
  value2 = value2 + 1;         // INCREMENT THE SOUND BITE TO PLAY NEXT
  if(value2 == 0x0c)           // IF POINTER IS SET TO 0X0C, POINT BACK TO STARTING POINT OF 0X09
  {
    value2 = 0x09;
  }
  }
  }
}

void haunting() // PLAYS HAUNTING SOUND BITES.   PLAY WITH THE DELAYS AS YOU'D LIKE.  YOU CAN USE THE LARGER DELAYS BELOW IF YOU'D LIKE.  I'M JUST USING  20/10 SECOND DELAYS HERE
{
  value2 = 0x00;
  delay(20000);
  readout();
  
  value2 = 0x02;
  delay(10000);
  //thirtyone();
  readout();
  
  value2 = 0x05;
  delay(10000);
  //elevenmin();
  readout();
  
  value2 = 0x07;
  delay(10000);
  //sevenmin();
  readout();
  
  value2 = 0x08;
  delay(10000);
  //twentythree();
  readout();
  
  value2 = 0x04;
  delay(10000);
  //five();
  readout();
  
  value2 = 0x01;
  delay(10000);
  //thirteen();
  readout();
  
  value2 = 0x03;
  delay(10000);
  //thirtyone();
  readout();
  
  value2 = 0x06;
  delay(10000);
  //fortyfive();
  readout();
  
  value2 = 0x0f;
  delay(10000);
  //sevenmin();
  readout();
  elevenmin();
}

void bugger()
{
  //blink();
  if(buggerstate == false) // SELECT SOUND BITES ON POWER UP
 {
  selectsound(); // ONCE SELECTED, BUGGERSTATE WILL BE TRUE, AND THE AUDIO BITES WILL PLAY
 }
 else
 {
  delay(1000);   // First delay before first audio bite - one second
  readout();    //
  value2 = value2 + 1;
  setaudioval();// This function checks to see if the sound bite needs to be set back to the first sound bite.
  elevenmin();  // play the second sound bite eleven minutes later 
  readout();
  value2 = value2 + 1;
  setaudioval();
  delay(27000); // Play next sound bite 27 seconds later.  You can use some of my larter displays below, create your own large delays, or use delay() to create your own short delays
  readout();
  value2 = value2 + 1;
  setaudioval();
  five();       // play five minutes later
  readout();
  value2 = value2 + 1;
  setaudioval();
  thirteen();       // play thirteen minutes later
  readout();
  value2 = value2 + 1;
  setaudioval();
  twentythree();       // play 23 minutes later
  readout();
  value2 = value2 + 1;
  setaudioval();
  thirtyone();       // play 31 minutes later
  readout();
  value2 = value2 + 1;
  setaudioval();  // Start over after this bite plays.
} 
}




void external()
{
 if(digitalRead(TRIGGER) == HIGH)
 {
  //16
  readout();
  value2 = value2 + 1;
  if(value2 == 0x1c)
  {
    value2 = 0x16; 
  }
 }
}


//////////////////////////////////////////////////////////EXTRA FUNCTIONS
void test()
{
  readout();
  value2 = value2 + 1;
}

void blink()
{
  for (int i = 0; i <= 10; i++) {
  digitalWrite(LED,HIGH);
  delay(100);
  digitalWrite(LED,LOW);
  delay(100);
}
}

void selectsound()
{
  while(buggerstate == false){
    {
    value2 = 0x1c;
    readout();
    buttonwait();
    if(buttonstate == 1)
    {
    soundbite = 0;
    value2 = 0x1c;
    buggerstate = true;
    break;
    }
    value2 = 0x0c;
    readout();
     buttonwait();
    if(buttonstate == 1)
    {
    soundbite = 1;
    value2 = 0x0c;
    buggerstate = true;
    break;
    }
    value2 = 0x10;
    readout();
     buttonwait();
    if(buttonstate == 1)
    {
    soundbite = 2;
    value2 = 0x10;
    buggerstate = true;
    break;
    }
    value2 = 0x16;
    readout();
     buttonwait();
    if(buttonstate == 1)
    {
    soundbite = 3;
    value2 = 0x16;
    buggerstate = true;
    break;
    }
  }
}
}

void setaudioval()
{
if(soundbite == 0)
{
  if(value2 == 0x22)
  {
    value2 = 0x1c;
  }
}
if(soundbite == 1)
{
  if(value2 == 0x0f)
  {
    value2 = 0x0c;
  }
}
if(soundbite == 2)
{
  if(value2 == 0x16)
  {
    value2 = 0x10;
  }
}
if(soundbite == 3)
{
  if(value2 == 0x1c)
  {
    value2 = 0x16;
  }
}
}


void buttonwait()
{
  buttonstate = 0;
  while(digitalRead(SEL) == HIGH){
    {}
  }
  delay(400);
  if(digitalRead(SEL) == LOW)
  {
   buttonstate = 1;
   while(digitalRead(SEL) == LOW){
      {}
    }
  } 
}

//////////////////////////////////////////////////////////TIMINGS

void minute() // 60 second delay function
{
  delay(30000);
  delay(30000);
}

  void fortyfive()  // 45 second function
{
    delay(30000);
    delay(15000);
}

 void elevenmin() // Just over 11 minutes.  The extra delay(777) just adds a few extra seconds to screw with people
{
  for (int i = 0; i <= 11; i++)
  {
    //minute();
    delay(777);
  }
}


void sevenmin() // This is another weird delay.  
{
  for (int i = 0; i <= 7; i++)
  {
    minute();
    delay(377);
  }
}

void twentythree() // 23 minutes + (100ms x 23) = 23 minutes, 2.3 seconds
{
 for (int i = 0; i <= 23; i++)
  {
    minute();
    delay(100);
  }
}

void thirtyone() // 31 minutes + (550ms x 31) = 31 mintes, 17 seconds
{
 for (int i = 0; i <= 31; i++)
  {
    minute();
    delay(550);
  }
}

void five() // You see how this works at this point.  Roughly 5 minutes and on second, which is kind of redundant.  Play with your delays!
{
 for (int i = 0; i <= 5; i++)
  {
    minute();
    delay(220);
  }
}

void thirteen() // If you want to add your own delays here, just copy and paste this function, rename it, and change the (i >= 13) to say(i >= 41) to change to 41 minutes + 41 x 2.6 seconds
{
 for (int i = 0; i <= 13; i++)
  {
    minute();
    delay(260);  // This extra delay is optional.  It just adds a few seconds to the overall delay
  }
}



void readydelay() // This function calls a specific delay for a specific sound bite.  For instance, a very short delay after the belch sound bite/a long delay afte a long evill augh
{
if(value2 == 0x00)
{
  delay(4250);
}
if(value2 == 0x01)
{
  delay(4500);
}
if(value2 == 0x02)
{
  delay(3000);
}
if(value2 == 0x03)
{
  delay(10000);
}
if(value2 == 0x04)
{
  delay(13500);
}
if(value2 == 0x05)
{
  delay(5000);
}
if(value2 == 0x06)
{
  delay(3000);
}
if(value2 == 0x07)
{
  delay(8500);
}
if(value2 == 0x08)
{
  delay(6000);
}
if(value2 == 0x09)
{
  delay(4000);
}
if(value2 == 0x0a)
{
  delay(3000);
}
if(value2 == 0x0b)
{
  delay(2000);
}
if(value2 == 0x0c)
{
  delay(1000);
}
if(value2 == 0x0d)
{
  delay(1000);
}
if(value2 == 0x0e)
{
  delay(1000);
}
if(value2 == 0x0f)
{
  delay(1500);
}
if(value2 == 0x10)
{
  delay(1000);
}
if(value2 == 0x11)
{
  delay(1000);
}
if(value2 == 0x12)
{
  delay(1000);
}
if(value2 == 0x13)
{
  delay(1000);
}
if(value2 == 0x14)
{
  delay(1000);
}
if(value2 == 0x15)
{
  delay(1000);
}
if(value2 == 0x16)
{
  delay(1000);
}
if(value2 == 0x17)
{
  delay(1000);
}
if(value2 == 0x18)
{
  delay(1000);
}
if(value2 == 0x19)
{
  delay(1000);
}
if(value2 == 0x1a)
{
  delay(1000);
}
if(value2 == 0x1b)
{
  delay(1000);
}
if(value2 == 0x1c)
{
  delay(1000);
}
if(value2 == 0x1d)
{
  delay(1000);
}
if(value2 == 0x1e)
{
  delay(1000);
}
if(value2 == 0x1f)
{
  delay(2000);
}
if(value2 == 0x20)
{
  delay(1500);
}
if(value2 == 0x21)
{
  delay(2000);
}
if(value2 == 0x22)
{
  delay(1000);
}
if(value2 == 0x23)
{
  delay(1500);
}
if(value2 == 0x24)
{
  delay(1500);
}
if(value2 == 0x25)
{
  delay(1000);
}
if(value2 == 0x26)
{
  delay(2000);
}
if(value2 == 0x27)
{
  delay(1000);
}
if(value2 == 0x28)
{
  delay(6500);
}
if(value2 == 0x29)
{
  delay(4000);
}
if(value2 == 0x2a)
{
  delay(3500);
}
if(value2 == 0x2b)
{
  delay(1500);
}
if(value2 == 0x2c)
{
  delay(3500);
}
if(value2 == 0x2d)
{
  delay(13000);
}
if(value2 == 0x2e)
{
  delay(4000);
}
if(value2 == 0x2f)
{
  delay(4000);
}
if(value2 == 0x30)
{
  delay(11000);
}
if(value2 == 0x31)
{
  delay(500);
}
if(value2 == 0x32)
{
  delay(10500);
}
if(value2 == 0x33)
{
  delay(6000);
}
if(value2 == 0x34)
{
  delay(2500);
}
if(value2 == 0x35)
{
  delay(5500);
}
if(value2 == 0x36)
{
  delay(8000);
}
if(value2 == 0x37)
{
  delay(10000);
}
if(value2 == 0x38)
{
  delay(3750);
}
if(value2 == 0x39)
{
  delay(10000);
}
}
