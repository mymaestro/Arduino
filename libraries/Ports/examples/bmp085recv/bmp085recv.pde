// Receiver for the bmp085 demo sketch
// 2010-05-26 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: bmp085recv.pde 5828 2010-06-17 00:44:09Z jcw $

#include <Ports.h>
#include <RF12.h>

typedef struct { int16_t temp; int32_t pres; } Payload;

void setup () {
    Serial.begin(57600);
    Serial.println("\n[bmp085recv]");
    rf12_initialize(30, RF12_868MHZ, 5); // 868 Mhz, net group 5, node 30
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof (Payload)) {
        Payload* data = (Payload*) rf12_data;
        Serial.print("BMP 0 0 ");
        Serial.print(data->temp);
        Serial.print(' ');
        Serial.println(data->pres);
    }
}
