/* StopWatch
 * Paul Badger 2008
 * Demonstrates using millis(), pullup resistors, 
 * making two things happen at once, printing fractions
 *
 * Physical setup: momentary switch connected to pin 4, other side connected to ground
 * LED with series resistor between pin 13 and ground
 */


#define ledPin  13                  // LED connected to digital pin 13
#define buttonPin 4                 // button on pin 4

int value = LOW;                    // previous value of the LED
int buttonState;                    // variable to store button state
int lastButtonState;                // variable to store last button state
int blinking;                       // condition for blinking - timer is timing
long interval = 100;                // blink interval - change to suit
long previousMillis = 0;            // variable to store last time LED was updated
long startTime ;                    // start time for stop watch
long elapsedTime ;                  // elapsed time for stop watch
int fractional;                     // variable used to store fractional part of time



void setup()
{
   Serial.begin(9600);

   pinMode(ledPin, OUTPUT);         // sets the digital pin as output

   pinMode(buttonPin, INPUT);       // not really necessary, pins default to INPUT anyway
   digitalWrite(buttonPin, HIGH);   // turn on pullup resistors. Wire button so that press shorts pin to ground.

}

void loop()
{
    // check for button press
   buttonState = digitalRead(buttonPin);                   // read the button state and store

   if (buttonState == LOW && lastButtonState == HIGH  &&  blinking == false){     // check for a high to low transition
      // if true then found a new button press while clock is not running - start the clock

      startTime = millis();                                   // store the start time
      blinking = true;                                     // turn on blinking while timing
      delay(5);                                               // short delay to debounce switch
      lastButtonState = buttonState;                          // store buttonState in lastButtonState, to compare next time

   }

   else if (buttonState == LOW && lastButtonState == HIGH && blinking == true){     // check for a high to low transition
      // if true then found a new button press while clock is running - stop the clock and report

        elapsedTime =   millis() - startTime;              // store elapsed time
        blinking = false;                                  // turn off blinking, all done timing
        lastButtonState = buttonState;                     // store buttonState in lastButtonState, to compare next time

       // routine to report elapsed time 
        Serial.print( (int)(elapsedTime / 1000L));         // divide by 1000 to convert to seconds - then cast to an int to print

        Serial.print(".");                             // print decimal point

        // use modulo operator to get fractional part of time 
       fractional = (int)(elapsedTime % 1000L);

       // pad in leading zeros - wouldn't it be nice if 
       // Arduino language had a flag for this? :)
       if (fractional == 0)
          Serial.print("000");      // add three zero's
       else if (fractional < 10)    // if fractional < 10 the 0 is ignored giving a wrong time, so add the zeros
          Serial.print("00");       // add two zeros
       else if (fractional < 100)
          Serial.print("0");        // add one zero

       Serial.println(fractional);  // print fractional part of time 

   }

   else{
      lastButtonState = buttonState;                         // store buttonState in lastButtonState, to compare next time
   }

   // blink routine - blink the LED while timing
   // check to see if it's time to blink the LED; that is, the difference
   // between the current time and last time we blinked the LED is larger than
   // the interval at which we want to blink the LED.

   if ( (millis() - previousMillis > interval) ) {

      if (blinking == true){
         previousMillis = millis();                         // remember the last time we blinked the LED

         // if the LED is off turn it on and vice-versa.
         if (value == LOW)
            value = HIGH;
         else
            value = LOW;
         digitalWrite(ledPin, value);
      }
      else{
         digitalWrite(ledPin, LOW);                         // turn off LED when not blinking
      }
   }

}
