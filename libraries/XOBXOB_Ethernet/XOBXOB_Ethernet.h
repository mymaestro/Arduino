///////////////////////////////////////////////////////////////
//
//  XOBXOB_Ethernet
//
//  Arduino interface to XOBXOB IoT platform
//	For use with the Arduino Ethernet shield
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

#ifndef _xobxob_ethernet_h
#define _xobxob_ethernet_h

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include "FSON.h"
#include "XOBXOB_Info.h"

// CLASS
class XOBXOB_Ethernet
{
  
  public:

    XOBXOB_Ethernet   	(uint8_t*, String);               		// Constructor
    void     			init();									// Constructor helper routine
    boolean  			connect ();								// To connect to xobxob
    boolean  			connected ();							// True/False for connected
    void     			stop ();								// Stops connection
    void     			requestXOB (String);					// Requests the contents of a XOB
    void     			updateXOB (String, int, String [][2]);	// Updates contents of XOB with one or more messages
    void     			initResponse();							// Initializes streaming buffer before sending request
    boolean  			loadStreamedResponse();    				// Load response one char at a time (test for end of object)
    String   			getMessage(String);               		// Returns named message contents from FSON response

  private:
  
    uint8_t* _mac;							   					// MAC Address for Ethernet card
    FSON _FSON;                                					// FSON utilities
    String _REQUEST_HEADER;                    					// XOBXOB request header (set in Constructor)

};

#endif // XOBXOB_Ethernet

