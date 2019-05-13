#include <ICM7218.h>


//pinout: (id0, id1, id2, id3, -id7-, write, mode)
ICM7218 myDisplay(5,6,7,8,9,11,12);  //makes a new ICM7218 called myDisplay
/*if you add a second display, 
 all pins can be shared (including data pins!) 
 except for -id7- (must be unique for each display) */

/*other pins:
ID4 (Shutdown) -> HIGH 5v
ID5 (Decode) -> LOW 0v
ID6 (Code) -> LOW 0v
*/


//useful datasheet (contains chip pinout): http://www.intersil.com/data/fn/fn3159.pdf


/* functions in this library:

writeNum();
writeInt();
writeFloat();
clear();

see below examples for implementation

*/



void setup(){


  
}



void loop(){
  /* writes 12345 to the screen with the decimal 
  located at position #2 */
  myDisplay.writeNum(12345, 2);
  
  delay(2000);
  
  /* same as above, but negative and no decimal point */
  myDisplay.writeNum(-12345, -1);
  
  delay(2000);
  
  /* writes integer 1337 to the screen */
  myDisplay.writeInt(1337);
  
  delay(2000);
  
  /* writes float 22/7 (sucky pi) to the screen with 
  hundredths precision (2 decimal places) */
  float pi = 22/7;
  myDisplay.writeFloat(pi, 2);
  
  delay(2000);(
  
  /* Clears LED display (blank) */
  myDisplay.clear();
  
  delay(2000);
  
  //millis write loop for counter
  while(1){
    myDisplay.writeNum(millis(), 3);
    delay(50);
  }
  
}
