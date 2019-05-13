///////////////////////////////////////////////////////////////
//
//  FSON.cpp
//
//  Light-weight method for dealing with JSON strings
//  When object string is set, it is parsed and token locations are noted
//  When properties and values are required, they are returned as strings that can be further parsed
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

#include "FSON.h"

// Constructor
FSON::FSON () {
	_hexChars = "0123456789ABCDEF";
}

// Initialize the string object
void FSON::setObject(String s) {

  // Save the string and it's length
  _FSONString = s;
  _FSONStringLength = s.length();

}

// Returns the FSON string belonging to this object
String FSON::getObject() {
  
  return _FSONString;
  
}

// Initialize streaming scanner
void FSON::initStreamScanner () {
  
  _scanState = SCAN_STATE_OBJECT_SEEKING;
  _scanEscape = false;
  _scanObjectLevel = 0;
  setObject("");

}

// Looks for a streamed object, one character at a time (delimited by "{", "}" )
boolean FSON::setStreamedObject(char c) {
  
  // Run the character through the state machine
  switch (_scanState) {
 
    // SEEKING for first character of an object
    case SCAN_STATE_OBJECT_SEEKING:
      
      // If this char is escaped, skip it
      if (_scanEscape) break;
      
      if ('{' == c) {
        _FSONString += c;
        _scanState = SCAN_STATE_OBJECT_LOOKING;
        _scanObjectLevel += 1;
      }
      break;  // _scanState
   
    // Looking at object contents
    case SCAN_STATE_OBJECT_LOOKING:
      _FSONString += c;
      if (_scanEscape) break;
      
      switch (c) {
        case '{': 
          _scanObjectLevel += 1;
          break;
        
        case '}':
          _scanObjectLevel -= 1;
          if (0 == _scanObjectLevel) _scanState = SCAN_STATE_OBJECT_FOUND;
          break;
      }
      break;  // _scanState
      
    default:
      break;  // _scanState
  }
  
  // Deal with escape characters
  _scanEscape = ('\\' == c) && (!_scanEscape);
    
  // Return true if the object has been found
  _FSONStringLength = _FSONString.length();
  return (SCAN_STATE_OBJECT_FOUND == _scanState);
    

}

// URI Decodes a string
String FSON::decodeURIComponent(String s) {

	String result = "";
	int i = 0;
	while (i < s.length()) {
		if(s[i] != '%') {
			result += s[i];
		} else {
			byte charNum = (_hexChars.indexOf(s[i+1]) << 4) + _hexChars.indexOf(s[i+2]);
			result += char(charNum);
			i += 2;
		}
		i += 1;
	}
	return (result);

}

// URI Encodes a string
String FSON::encodeURIComponent(String s) {

	char esc[4] = "%  ";
	byte c;
	String result = "";
	for (int i=0; i < s.length(); i++) {
		c = byte(s[i]);
		if((c>47&&c<58) || (c>64&&c<91) || (c>96&&c<123)) {
			result += char(c);
		} else {
			esc[1] = _hexChars.charAt(c>>4);
			esc[2] = _hexChars.charAt(c&0x0F);
			result += esc;
		}
	}
	return (result);

}

// Finds an FSON property with name and returns it unescaped
String FSON::getProperty(String name) {
  
  return (unescape(getRawProperty(name)));

}

// Finds an FSON property with name
String FSON::getRawProperty(String name) {
  
  _scanPos = 1;
  
  // Look for name
  _scanState = SCAN_STATE_NAME_SEEKING;
  while ((_findName(&_scanPos) != name) && (_scanPos < _FSONStringLength));
  if (_FSONStringLength <= _scanPos) return (NULL);
  
  // Get value
  _scanPos += 1;
  return (_findValue(&_scanPos));

}

// Remove escape characters from string
String FSON::unescape(String s) {
	String newS = "";
	boolean escape = false;
	for(int i = 0; i<s.length(); i++) {  
		char c = s.charAt(i);
                escape = (!escape && ('\\' == c));
                if (!escape) newS += c;
      	}
	return newS;
}


//////////////////////////////////////////////////////
// Private methods

String FSON::_findName (int* i) {
  
  if (!_findChar ('\"', i) || (_FSONStringLength <= *i)) return (NULL);
  *i += 1;
 
  int theStart = *i;
  if (!_findChar ('\"', i)) return (NULL);
  
  return (_FSONString.substring(theStart, *i));

}

String FSON::_findValue (int* i) {
  
  if (!_findChar (':', i) || (_FSONStringLength <= *i)) return (NULL);
  *i += 1;
  
  // Skip any spaces
  while (_FSONString[*i] == ' ') *i += 1;

  char _endDelimeter;
  switch (_FSONString[*i]) {
    case '{' : _endDelimeter = '}'; break;
    case '[' : _endDelimeter = ']'; break;
    case '\"' : _endDelimeter = '\"'; break;
    default  : _endDelimeter = ',';
  }
  
  int theStart = *i;
  *i += 1;
  _findChar(_endDelimeter, i);
  
  if (_endDelimeter != ',') *i += 1;
  return (_FSONString.substring(theStart, *i));

}

// Find a character in an FSON string. NOTE: won't find escaped chars (though it will find 'the' escape char '\\')
boolean FSON::_findChar (char c, int* i) {
  
  // For tracking bracket and quote levels
  byte _theBracketLevel = 0, _theSquareBracketLevel = 0;
  boolean _quoteOpen = false;

  while (*i < _FSONStringLength) {
    
    // Get the next character
    char theChar = _FSONString[*i];

    // If this is the correct char, AND it's not encumbered, we're done!
    if ((c == theChar)  && !_quoteOpen && ((_theBracketLevel + _theSquareBracketLevel) <= 0)) return (true);
    
    // if the char is an escape, skip forward (but watch for the end)
    if ('\\' == theChar) {
      *i += 2;
      if (_FSONStringLength < *i) return (false);
    }
    
    // Alas, not done, so process bracket and quote levels
    switch (theChar) {
      case '{' : _theBracketLevel++; break;
      case '}' : _theBracketLevel--; break;
      case '[' : _theSquareBracketLevel++; break;
      case ']' : _theSquareBracketLevel--; break;
      case '\"': _quoteOpen = !_quoteOpen; break;
      default: break;
    }
    
    // Increment the character pointer
    *i += 1;
  }
  return (false);
}
