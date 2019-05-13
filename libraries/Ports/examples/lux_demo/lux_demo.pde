// Demo of the Lux Plug, based on the LuxPlug class in the Ports library
// 2010-03-18 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: lux_demo.pde 7689 2011-05-24 14:22:35Z jcw $

#include <Ports.h>
#include <RF12.h> // needed to avoid a linker error :(
    
PortI2C myBus (1);
LuxPlug sensor (myBus, 0x39);
byte highGain;

void setup () {
    Serial.begin(57600);
    Serial.println("\n[lux_demo.2]");
    sensor.begin();
}

void loop () {    
    const word* photoDiodes = sensor.getData();
    Serial.print("LUX ");
    Serial.print(photoDiodes[0]);
    Serial.print(' ');
    Serial.print(photoDiodes[1]);
    Serial.print(' ');
    Serial.print(sensor.calcLux());
    Serial.print(' ');
    Serial.println(highGain);

    // need to wait after changing the gain
    //  see http://talk.jeelabs.net/topic/608
    highGain = ! highGain;
    sensor.setGain(highGain);
    delay(1000);
}
