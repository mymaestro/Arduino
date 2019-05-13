#include "OnionLibrary.h"

// constructor
Onion::Onion() 
{
  // Arduino Dock setup
  ArduinoDockSetup();
}

// destructor
Onion::~Onion()
{
  // nothing for now
}


//// regular functions
// Restarts program from beginning but does not reset the peripherals and registers
void _softwareReset() {
  wdt_enable(WDTO_15MS);
} 

// service routine to read 
void _receiveEvent(int arg) {
  int msg   = 0;
  int shift = 0;

  // receive bytes from i2c bus, reconstruct into one int
  while (Wire.available() > 0) {
    shift = 8 * (Wire.available() - 1);
    int   x = Wire.read();

    msg |= (x & 0xff) << shift;    
    shift += 8;
  }

  // check for reset condition
  if (msg == REBOOT_STRING) {
    _softwareReset();
  }
}


//// public functions
// enable I2C interface and setup ISR
void Onion::ArduinoDockSetup(uint8_t i2c_addr) {
  // disable watchdog timer
  wdt_disable();
  
  // create function pointer to _receiveEvent
  void (*ptr)(int);
  ptr = &(_receiveEvent);

  
  // join i2c bus 
  Wire.begin(i2c_addr);    
  // register service routine
  Wire.onReceive(ptr);
}


