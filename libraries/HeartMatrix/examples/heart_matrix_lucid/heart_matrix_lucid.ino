/* LucidTronix write messages on the Heart Matrix.
 * For instructions details and schematic, See:
 * http://www.lucidtronix.com/tutorials/18
 * Connect the data, latch, and clock pins
 * to the corresponding pins on the heart matrix
 */

#include <FrequencyTimer2.h>
#include <HeartMatrix.h>

// dataPin is on 2, Latch is on 3, and clock is on 4
HeartMatrix hm = HeartMatrix(2,3,4);
void displayer2(){
  hm.displayer();
}

void setup() {
   //FrequencyTimer2::disable();
   // Set refresh rate (interrupt timeout period)
   // if things break try making this number bigger
   FrequencyTimer2::setPeriod(2000);
    // Set interrupt routine to be called
   FrequencyTimer2::setOnOverflow(displayer2); 
}

void loop() {
   hm.on();  
   if ( millis() > 5000) hm.set_scroll_wait(50);
   if ( millis() > 10000) hm.set_message("I Love LucidTronix");
}



