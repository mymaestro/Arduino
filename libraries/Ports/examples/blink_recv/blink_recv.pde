// Ports library demo, this is the receiver, see also the blink_xmit example
// 2009-02-14 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: blink_recv.pde 6395 2010-12-10 19:27:21Z jcw $

#include <Ports.h>
#include "RF12.h"

// the state received from the transmitter
RemoteNode::Data state;

// local ports
Port one (1), two (2), three (3), four (4);

void setup() {
    rf12_initialize(1, RF12_868MHZ);
}

void loop() {
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof state) {
        memcpy(&state, (void*) rf12_data, sizeof state);
        
        if (state.flags & 0x80) {
            one.mode((state.modes & 0x01) != 0);
            two.mode((state.modes & 0x02) != 0);
            three.mode((state.modes & 0x04) != 0);
            four.mode((state.modes & 0x08) != 0);
        }
        
        if (state.flags & 0x40) {
            one.digiWrite(state.digiIO & 0x01);
            four.digiWrite(state.digiIO & 0x08);
        }

        if (state.flags & 0x10)
            two.anaWrite(state.anaOut[0]);

        if (state.flags & 0x20)
            three.anaWrite(state.anaOut[1]);
        
        // not used right now
        //
        // if (state.flags & 0x01)
        //     state.anaIn[0] = one.anaRead();
        // if (state.flags & 0x02)
        //     state.anaIn[1] = two.anaRead();
        // if (state.flags & 0x04)
        //     state.anaIn[2] = three.anaRead();
        // if (state.flags & 0x08)
        //     state.anaIn[3] = four.anaRead();

        state.flags = 0;
        
        if (RF12_WANTS_ACK)
            rf12_sendStart(RF12_ACK_REPLY, 0, 0);
    }
}
