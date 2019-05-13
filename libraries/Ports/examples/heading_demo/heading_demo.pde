// Demo of the Heading Board based on the HDPM01 (with barometer and compass)
// 2010-03-22 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: heading_demo.pde 6089 2010-10-25 00:12:23Z jcw $

#include <Ports.h>
#include <RF12.h> // needed to avoid a linker error :(

HeadingBoard sensor (4);
MilliTimer measureTimer;

void setup () {
    Serial.begin(57600);
    Serial.println("\n[heading_demo]");
    rf12_initialize(7, RF12_868MHZ, 5);
    sensor.begin();
}

void loop () {
    if (measureTimer.poll(1000)) {
        struct { int temp, pres, xaxis, yaxis; } payload;

        sensor.pressure(payload.temp, payload.pres);
        sensor.heading(payload.xaxis, payload.yaxis);
        
        while (!rf12_canSend())
            rf12_recvDone();
        rf12_sendStart(0, &payload, sizeof payload, 2);

        Serial.print("HDPM ");
        Serial.print(payload.temp);
        Serial.print(' ');
        Serial.print(payload.pres);
        Serial.print(' ');
        Serial.print(payload.xaxis);
        Serial.print(' ');
        Serial.println(payload.yaxis);
    }
}
