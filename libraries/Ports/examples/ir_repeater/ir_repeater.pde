// Record an IR command using an Infrared Plug, and repeat it 3x.
// 2010-11-11 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: ir_repeater.pde 6173 2010-11-11 17:24:27Z jcw $

#include <Ports.h>
#include <RF12.h> // needed to avoid a linker error :(

InfraredPlug ir (2);
byte outBuf[250];

void setup () {
    Serial.begin(57600);
    Serial.println("\n[ir_repeater]");
}

void loop () {
    ir.poll();
    byte count = ir.done();
    if (count >= 10) {
        const byte* inBuf = ir.buffer();

        // don't decode the packet, just send it out again
        memset(outBuf, 0, sizeof outBuf);
        word pos = 0;
        
        for (byte i = 0; i < count; ++i) {
            // determine how many slots IR was on or off
            byte nibble = (inBuf[i/2] >> (i % 2) * 4) & 0x0F;
            if (nibble > 10)
                nibble += nibble - 10;
            // nibble now contains number of slots
            
            for (byte j = 0; j < nibble; ++j) {
                if (i % 2 == 0) // even nibbles correspond to "on"
                    bitSet(outBuf[pos/8], pos%8);
                ++pos;
            }
        }
            
        Serial.print("Got ");
        Serial.print(pos);
        Serial.println(" bits");
            
        // send out IR codes three times at 1-second intervals
        for (byte n = 0; n < 3; ++n) {
            delay(1000);
            ir.send(outBuf, pos);
            Serial.println(" sent!");
        }
        
        ir.done();
    }
}
