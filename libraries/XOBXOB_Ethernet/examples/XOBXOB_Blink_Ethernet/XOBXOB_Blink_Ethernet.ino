////////////////////////////////////////////////////////////////////////////
//
//  XOBXOB Blink :: Ethernet Shield
// 
//  This sketch connects to the XOBXOB IoT platform using an Arduino Ethernet shield. 
// 
//
//  The MIT License (MIT)
//  
//  Copyright (c) 2013 Robert W. Gallup, XOBXOB
//  
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.// 

// All of these includes are required
#include <XOBXOB_Ethernet.h>
#include <Ethernet.h>
#include <SPI.h>

///////////////////////////////////////////////////////////
//
// Change these for your Ethernet Shield and your APIKey
//
// NOTE: the MAC address for your Ethernet Shield might be
// printed on a label on the bottom of the shield. Your
// APIKey will be found on your account dashboard when you
// login to XOBXOB)
//

byte mac[]    = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
String APIKey = "xxxx-xxxx-xxxx-xxxx-xxxx";

///////////////////////////////////////////////////////////


// Create XOBXOB object (for the Ethernet shield)
XOBXOB_Ethernet XOB (mac, APIKey);

// Variables for request timing
boolean lastResponseReceived = true;
long lastRequestTime = -20000;

void setup() {
  
  // Set the LED pin for output and turn it off
  pinMode (8, OUTPUT);
  digitalWrite (8, LOW);  
  
  // Initialize XOBXOB
  XOB.init();
  
}

void loop()
{
    
  // New XOB request every 4 seconds (if previous response has been received)
  if (lastResponseReceived && (abs(millis() - lastRequestTime) > 4*1000)) {
 
  	// if the connection has dropped, reconnect
    while (!XOB.connected()) XOB.connect();

    // Reset timer and response flags. Then, request "XOB" contents
    lastResponseReceived = false;
    lastRequestTime = millis();
    
    XOB.requestXOB("XOB");

  }

  // Load response a character at a time when it is available.
  // If loadStreamedResponse returns true, a completed response has been received
  // Get the "switch" message from the XOB and turn the LED on/off
  // NOTE: The message contents are returned with quotes. So, include them
  // in the comparison statement.
  if (!lastResponseReceived && XOB.loadStreamedResponse()) {

    lastResponseReceived = true;

    String LED = XOB.getMessage("switch");
    if (LED == "\"ON\"") {
      digitalWrite (8, HIGH);
    } else {
      digitalWrite (8, LOW);
    }
    
  }

}