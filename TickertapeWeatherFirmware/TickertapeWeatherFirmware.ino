/* ***************************************************************************************************** *\
**     TickerTape Firmware                                                                               **
**     (Kickstarter Backer Version 0.7)                                                                  **
**     (c) 2019 Darren Dignam                                                                            **
**                                                                                                       **
**                                                                                                       **
**     This project relies on the community of ESP32 developers, and AdaFruit and others.                **
**                                                                                                       **
**     Special shout out to this project:                                                                **
**     https://github.com/zhouhan0126/WIFIMANAGER-ESP32                                                  **
**                                                                                                       **
  \* ***************************************************************************************************** */

#include <Arduino.h>
#include <LED_Display_Wrapper.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <HTTPClient.h>
#include <time.h>

void ResetDevice();

//Defines and constants  (Version might be better as date stting, and used for headers..?)
#define VERSION "TickerTape v0.7"
#define VNUMBER "0.7"
#define DEBUG_BUFFER_SIZE 500
#define LED 5

int DEBUG = 1;

//The state machine for the device. 0 is the default, 1 displays the weather API.
int DeviceMode = 0;

unsigned long p_time_Millis = 0;        // will store last time LED was updated
unsigned long p_weather_Millis = 0;
unsigned long p_weather_interval = 0; //starts at zero - but will be set to an hour after the first time the weather is requested
unsigned long p_delay_Millis = 0;
const long    millis_interval = 1000;

uint16_t UI_Leds = 0b0000000000000000;  //all the additional UI LEDS are off.

LED_Display_Wrapper LEDdisplay = LED_Display_Wrapper();

//Weather API stuff
char apiKey_default[40]  = "10babfc51dce3f77465a5a398695ea98";
char apiLocation_default[40] = "Austin,us";
char weather_payload[700];
bool weather_busy = 0;

// https://openweathermap.org/ API for city weather data
// More info: https://techtutorialsx.com/2018/03/17/esp32-arduino-getting-weather-data-from-api/
// our query: http://api.openweathermap.org/data/2.5/weather?q=London,uk&units=metric&APPID=10babfc51dce3f77465a5a398695ea98


unsigned long api_mtbs = 60000; //mean time between api requests
unsigned long api_lasttime;   //last time api request has been done
unsigned long subs = 0;
WiFiClientSecure client;

// The timezone stuff here is a bit crude, so if you want to make it better, skip down to where these values are used (line 333), and you can implement an actual
// timezone it is a bit more complex, but should be more useful in the long run, accounting for daylight savings time, and odd timezones where the offset is 1.5 hours.

//Time Stuff
short timezone_default = 1;
short timezone;

char displaybuffer[6] = {' ', ' ', ' ', ' ', ' ', ' '};
char _str_buffer[7];  //6 chars and a null char...

//define default values here, if there are different values in config.json, they are overwritten.
char Brightness[40];
char WelcomeText[50];
char apiKey[40];
char apiLocation[40];
char charTimezone[4];

uint8_t _brightness = 8;
uint8_t _brightness_new;

//touch buttons stuff
int touch_threshold = 40;  //Adjust this if touch is not working.

#include "helper_functions.h"

void gotTouch0() {
  if (!weather_busy) {
    DeviceMode = 2;
  }
}
void gotTouch2() {
  //brightness down
  _brightness_new = _brightness - 1;

  if (_brightness_new <= 1) {
    _brightness_new = 1;
  }
}
void gotTouch3() {
  //brightness up
  _brightness_new = _brightness + 1;
  if (_brightness_new >= 14) {
    _brightness_new = 14;
  }
}

//flag for saving data
bool shouldSaveConfig = true;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  //ResetDevice();//Uncomment this and upload to have the device in a mode where it will erase the onboard memory (SSID + PASSWORD) on reboot. Comment and reupload to get the normal operation back


  //init strings to empty
  Brightness[0]   = 0;
  WelcomeText[0]  = 0;
  apiKey[0]       = 0;
  apiLocation[0]  = 0;
  charTimezone[0] = 0;

  //ESP LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.begin(115200);
  Serial.println();

  //bind touch inturrupts
  touchAttachInterrupt(T0, gotTouch0, touch_threshold);
  //  touchAttachInterrupt(T1, gotTouch1, touch_threshold);
  touchAttachInterrupt(T2, gotTouch2, touch_threshold);
  touchAttachInterrupt(T3, gotTouch3, touch_threshold);

  //debug..
  dbgprint( "Starting ESP32-TickerTape running on CPU %d at %d MHz.  Version %s.  Free memory %d", xPortGetCoreID(), ESP.getCpuFreqMHz(), VERSION, ESP.getFreeHeap() );
  delay(2000);

  LEDdisplay.BLINK();//LEDdisplay.BLINK();LEDdisplay.BLINK();  //LEDdisplay.BLINK();LEDdisplay.BLINK();

  //read configuration from FS json
  dbgprint("mounting FS...");
  SPIFFS.begin (true);

  if (SPIFFS.begin()) {
    dbgprint("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      dbgprint("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        dbgprint("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(Brightness, json["Brightness"]  );
          strcpy(WelcomeText, json["WelcomeText"]);
          strcpy(apiKey, json["apiKey"]          );
          strcpy(apiLocation, json["apiLocation"]);
          strcpy(charTimezone, json["charTimezone"]);
        } else {
          dbgprint("failed to load json config");
        }
      }
    }
  } else {
    dbgprint("failed to mount FS");
  }
  //end read

  //do something with settings..??
  if ( WelcomeText == "RESET" ) {
    dbgprint("ERASING MEMORY...");
    LEDdisplay.ScrollText("Resetting device...     ");
    ResetDevice();
    LEDdisplay.ScrollText("Device reset. Turn me off and back on again :)       ");
  }
  if ( Brightness != "" ) {
    dbgprint("Setting brightness...");
    dbgprint(Brightness);
    String _tmp_bright = Brightness;
    if (_tmp_bright.toInt() > 0) {
      _brightness = _tmp_bright.toInt();
      _brightness_new = _brightness;
      LEDdisplay.setBrightness( _tmp_bright.toInt() );
      Serial.print("Saved Brightness: ");
      Serial.println(_brightness);
    }
  }
  if ( charTimezone != "" ) {
    dbgprint("Setting timezone...");
    dbgprint(charTimezone);
    String _tmp_timezone = charTimezone;
    if (_tmp_timezone.toInt() >= -12 && _tmp_timezone.toInt() <= 12) {
      timezone = _tmp_timezone.toInt();
      Serial.print("Saved Timezone: ");
      Serial.println(timezone);
    } else {
      timezone = 1;
    }
  }

  String tmpString = "";

  if (strlen(WelcomeText) == 0) {
    tmpString = "TICKERTAPE - KICKSTARTER";
  } else {
    tmpString = WelcomeText;
  }
  String _message = "v0.7 " + tmpString + "      ";
  LEDdisplay.ScrollText( _message );

  if (strlen(apiKey) == 0) {
    strncpy(apiKey, apiKey_default, 40);
  }
  if (strlen(apiLocation) == 0) {
    strncpy(apiLocation, apiLocation_default, 40);
  }



  // // The extra parameters to be configured (can be either global or just in the setup)
  // // After connecting, parameter.getValue() will get you the configured value
  // // id/name placeholder/prompt default length
  WiFiManagerParameter custom_Brightness("Brightness", "Brightness", Brightness, 40);
  WiFiManagerParameter custom_WelcomeText("WelcomeText", "WelcomeText", WelcomeText, 50);
  WiFiManagerParameter custom_apiKey("Weather API Key", "Weather API Key", apiKey, 40);
  WiFiManagerParameter custom_apiLocation("Location", "Location", apiLocation, 40);
  WiFiManagerParameter custom_charTimezone("Time-offset", "Time-offset", charTimezone, 4);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //The custom parameters here
  wifiManager.addParameter(&custom_Brightness);
  wifiManager.addParameter(&custom_WelcomeText);
  wifiManager.addParameter(&custom_apiKey);
  wifiManager.addParameter(&custom_apiLocation);
  wifiManager.addParameter(&custom_charTimezone);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("TICKERTAPE", "G42")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected... great :)");

  //read custom parameters
  strcpy(Brightness, custom_Brightness.getValue());
  strcpy(WelcomeText, custom_WelcomeText.getValue());
  strcpy(apiKey, custom_apiKey.getValue());
  strcpy(apiLocation, custom_apiLocation.getValue());
  strcpy(charTimezone, custom_charTimezone.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

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

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  String _tmp_IP = WiFi.localIP().toString().c_str();
  LEDdisplay.ScrollText("IP " + _tmp_IP + "      ");

  Serial.println();
  Serial.print("Brightness: ");
  Serial.print(Brightness);
  Serial.println();
  Serial.print("WelcomeText: ");
  Serial.print(WelcomeText);
  Serial.println();
  Serial.print("apiKey: ");
  Serial.print(apiKey);
  Serial.println();
  Serial.print("apiLocation: ");
  Serial.print(apiLocation);
  Serial.println();
  Serial.print("charTimezone: ");
  Serial.print(charTimezone);
  Serial.println();

  //WiFi.disconnect(true); //erases store credentially
  //SPIFFS.format();  //erases stored values
  Serial.println("Done");

  // This is the code that uses the timezone value, it's a crude offsetting value that gets used here with configTime function. It has been suggested to me that you can actually use a
  // proper timezone here.

  //setup time stuff (comment this command and use the instructions below to use a timezone instead.)
  configTime(timezone * 3600, 0, "pool.ntp.org", "time.nist.gov");


  //Use theese commands to implement an actual timezone:
  //
  // the standard configTime function:
  //            configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  //
  // now you need to use the setenv command and set the device TZ timezone. the Line below is for GMT. But you will need to replace the "GMT0"
  // with your timezone string.
  // The timezone strings can be found in the following CSV online:
  // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
  // Further reading:
  // https://github.com/G6EJD/ESP32-Time-Services-and-SETENV-variable/blob/master/README.md
  // https://www.timeanddate.com/time/zones/
  //
  //            setenv("TZ", "GMT0", 1);
  //
  // You can add additional parameters to the timezone string, but that is beyond the scope of this comment!


  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }

  //TODO: Fix the line below, a bit of a hack. The WiFi Manager resets the brightness, as it uses it own LED class to write text at 50% brightness, and never restets it to the saved value
  LEDdisplay.setBrightness(_brightness);

  //turn on the wifi LED
  UI_Leds = 0b0000100000000000;
  LEDdisplay.writeDigitRaw(6, UI_Leds);
  LEDdisplay.writeDisplay();
}

void loop() {
  unsigned long currentMillis = millis();
  if (_brightness_new != _brightness) {
    _brightness = _brightness_new;
    LEDdisplay.setBrightness(_brightness);
    SaveConfig();
  }
  if (currentMillis > p_delay_Millis) {
    if (DeviceMode == 0 || DeviceMode == 1) { //time stuff
      //p_time_Millis
      //if (currentMillis - p_time_Millis >= (millis_interval/3) )
      if (currentMillis - p_time_Millis >= (millis_interval / 5) ) {
        //UI_Leds &= ~(0b0000000011000000);  //toggle the seconds seperator bits
        UI_Leds &= ~(0b0000000011001100);  //toggle the all the seperator bits
        LEDdisplay.writeDigitRaw(6, UI_Leds);
        LEDdisplay.writeDisplay();
        //      p_time_Millis = currentMillis;  // this is only the 1/2 way point, reset this below
      }
      if (currentMillis - p_time_Millis >= millis_interval ) {
        time_t now = time(nullptr); //get time
        struct tm * timeinfo;
        timeinfo = localtime(&now);  //parse time
        //display time
        sprintf(_str_buffer, "%02d%02d%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        LEDdisplay.FillTextBuffer(_str_buffer);

        UI_Leds |= 0b0000000011001100;  //Turn on the time seperator bits;
        LEDdisplay.writeDigitRaw(6, UI_Leds);
        LEDdisplay.writeDisplay();

        p_time_Millis = currentMillis;
      }
    } else if (DeviceMode == 2) {
      parseWeather();
    }
  }


}    // **  END LOOP  **




// ** ****************************************************************************************************  **
// **                                                                                                       **
// **  This function is useful to erase the saved settings, including any known WiFi Access Points          **
// **                                                                                                       **
// ** ****************************************************************************************************  **
void ResetDevice() {
  WiFi.disconnect(true); //erases store credentially
  SPIFFS.format();  //erases stored values
  ESP.restart();
}
