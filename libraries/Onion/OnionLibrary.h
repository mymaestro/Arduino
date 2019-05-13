/*
 * OnionLibrary
 * Version 0.1 November, 2015
 * Copyright 2015 Onion Corporation

*/

#ifndef _ONION_LIBRARY_H_
#define _ONION_LIBRARY_H_


#include <Wire.h>
#include <avr/wdt.h>

#define   REBOOT_STRING                  (0xdead)
#define   ARDUINO_DOCK_I2C_DEVICE_ADDR   (0x08)

class Onion {
public:
	Onion();
	~Onion();

	void ArduinoDockSetup	(uint8_t i2c_addr = ARDUINO_DOCK_I2C_DEVICE_ADDR);
	//void _receiveEvent    (int arg);

private:
	void (Onion::*ptr)(int);

	//void _softwareReset   ();
	//void _receiveEvent    (int arg);
};

#endif // _ONION_LIBRARY_H_