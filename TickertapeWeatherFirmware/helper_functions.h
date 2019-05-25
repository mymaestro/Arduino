////**************************************************************************************************
////                                          D B G P R I N T                                        *
////**************************************************************************************************
//// Send a line of info to serial output.  Works like vsprintf(), but checks the DEBUG flag.        *
//// Print only if DEBUG flag is true.  Always returns the formatted string.                         *
////**************************************************************************************************
char* dbgprint ( const char* format, ... )
{
  static char sbuf[DEBUG_BUFFER_SIZE] ;                // For debug lines
  va_list varArgs ;                                    // For variable number of params

  va_start ( varArgs, format ) ;                       // Prepare parameters
  vsnprintf ( sbuf, sizeof(sbuf), format, varArgs ) ;  // Format the message
  va_end ( varArgs ) ;                                 // End of using parameters
  if ( DEBUG )                                         // DEBUG on?
  {
    Serial.print ( "D: " ) ;                           // Yes, print prefix
    Serial.println ( sbuf ) ;                          // and the info
  }
  return sbuf ;                                        // Return stored string
}

void displayWeather() {
  Serial.println("displayWeather()");

  String payload_data;
  payload_data = String(weather_payload);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject( payload_data );
  json.printTo(Serial);
  if (json.success()) {
    Serial.println("\nparsed json");

    //p.key// is a const char* pointing to the key
    //p.value// is a JsonVariant

    int response_code = json["cod"];
    if (response_code != 200) {
      Serial.println( "API Error!" );
      Serial.println( json["message"].as<char*>() );
      LEDdisplay.ScrollText(  json["message"].as<char*>()  );
    } else {
      LEDdisplay.ScrollText("    weather in ");
      LEDdisplay.ScrollText( json["name"].as<char*>() );
      LEDdisplay.ScrollText(" IS ");
      LEDdisplay.ScrollText( json["weather"][0]["main"].as<char*>() );

      //                    API returns mixed case, that we might want to change something like this:
      //                    String _WeatherType;
      //                    _WeatherType = String( json["name"].as<char*>() );
      //                    _WeatherType.toUpperCase();
      //                    LEDdisplay.ScrollText( _WeatherType );

      LEDdisplay.ScrollText("      Temperature -- ");
      LEDdisplay.ScrollText( json["main"]["temp"].as<char*>() );
      LEDdisplay.ScrollText("F      Humidity -- ");
      LEDdisplay.ScrollText( json["main"]["humidity"].as<char*>() );
      LEDdisplay.ScrollText("percent      ");
    }

  } else {
    dbgprint("failed to load json config");
  }
}

void getWeather() {
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  char url[500];
  strcpy(url, "http://");
  strcat(url, "api.openweathermap.org/data/2.5/weather?q=");
  strcat(url, apiLocation);
  strcat(url, "&units=imperial&APPID="); //change the temp unit here, refer to the API docs
  strcat(url, apiKey);

  http.begin(url); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();

      payload.toCharArray(weather_payload, 700);
      //weather_payload = payload;

      Serial.println(payload);
      //cool - lets display it!!

      displayWeather();

    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  DeviceMode = 0; //Sets the tickertape back to clock mode
  weather_busy = 0;
}

void parseWeather() {
  weather_busy = 1;
  unsigned long currentMillis = millis();

  LEDdisplay.writeDigitRaw(6, 0b0000000000000000); //clear seconds seperators for now
  LEDdisplay.writeDisplay();

  if (currentMillis - p_weather_Millis >= p_weather_interval ) {
    p_weather_interval = 1000 * 60 * 60; //one hour intervals
    p_weather_Millis = currentMillis;

    LEDdisplay.FillTextBuffer(" WAIT ");
    Serial.println("Weather Pressed");
    getWeather();
  } else {
    LEDdisplay.FillTextBuffer("      ");
    displayWeather();

    DeviceMode = 0; //Sets the tickertape back to clock mode
    weather_busy = 0;
  }
}

void SaveConfig() {
  //    Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  String _bright = String(_brightness);
  _bright.toCharArray(Brightness, 40);
  json["Brightness"] = Brightness;
  json["WelcomeText"] = WelcomeText;
  json["apiKey"] = apiKey;
  json["apiLocation"] = apiLocation;
  json["charTimezone"] = charTimezone;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
  //end save
}
