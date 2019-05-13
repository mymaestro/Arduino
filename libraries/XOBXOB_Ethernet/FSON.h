///////////////////////////////////////////////////////////////
//
//  FSON.h
//
//  Used to parse strings returned from xobxob IoT service
//  FSON is JSON, but only one level (i.e. Flat JSON)
//
#ifndef FSON_h
#define FSON_h

#include "Arduino.h"

// CLASS
class FSON
{
  public:

    FSON ();                                   // Constructor
    void    setObject (String s);              // Initializes the object with a string
    String  getObject ();                      // Returns current source object
    String  getProperty (String name);         // Return a property string by name. Escape chars removed
    String	getRawProperty (String name);	   // Returns a property with escape characters intact
    
    void    initStreamScanner();               // Initializes the stream scanner
    boolean setStreamedObject(char c);         // Scans stream for an object delimeted by "{", "}"
    String  decodeURIComponent(String s);      // Decodes an encoded value from string
    String  encodeURIComponent(String s);      // Encodes to URL-safe string
	String	unescape(String s);				   // Simple unescaping of string (removes '\' characters)
	
  private:
  
    String  _FSONString;                       // String storage for JSON
    int     _FSONStringLength;                 // String Length
    
    int     _scanPos;                          // Variables for used in scanning for property names and values
    byte    _bracketLevel,
            _quoteLevel,
            _sqBracketLevel;

    
    #define SCAN_STATE_OBJECT_SEEKING  0
    #define SCAN_STATE_OBJECT_LOOKING  1
    #define SCAN_STATE_OBJECT_FOUND    2
    
    #define SCAN_STATE_NAME_SEEKING    0
    #define SCAN_STATE_NAME_LOOKING    1
    #define SCAN_STATE_NAME_FOUND      3
    
    #define SCAN_STATE_VALUE_SEEKING   0
    #define SCAN_STATE_VALUE_LOOKING   1
    #define SCAN_STATE_VALUE_FOUND     3

    byte    _scanState;                        // Scanner state (waiting, looking, done)
    boolean _scanEscape;                       // Flags an escape character
    int     _scanObjectLevel;                  // For tracking bracket nesting levels

    String  _findName(int*);                   // For property scanning
    String  _findValue(int*);
    boolean _findChar(char, int*);             // Finds a char at current level
    String	_hexChars;						   // For conversion between hex and decimal
};

#endif // FSON
