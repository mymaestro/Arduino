// Demo of the Gravity Plug, based on the GravityPlug class in the Ports library
// 2010-03-19 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: gravity_demo.pde 7637 2011-04-27 23:05:26Z jcw $

#include <Ports.h>
#include <RF12.h>

PortI2C myBus (1);
GravityPlug sensor (myBus);
MilliTimer measureTimer;

void setup () {
    Serial.begin(57600);
    Serial.println("\n[gravity_demo]");
    rf12_initialize(7, RF12_868MHZ, 5);
    sensor.begin();
}

void loop () {
    if (measureTimer.poll(1000)) {
        const int* p = sensor.getAxes();

        while (!rf12_canSend())
            rf12_recvDone();
        rf12_sendStart(0, p, 3 * sizeof *p, 2);

        Serial.print("GRAV ");
        Serial.print(p[0]);
        Serial.print(' ');
        Serial.print(p[1]);
        Serial.print(' ');
        Serial.println(p[2]);
    }
}
