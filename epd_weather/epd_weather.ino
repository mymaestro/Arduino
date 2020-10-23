#include <time.h>
#include <Adafruit_GFX.h>       // Core graphics library
#include <Adafruit_EPD.h>       // eInk
#include <Adafruit_NeoPixel.h>
#include <Adafruit_WiFiNINA.h>
#include <ArduinoJson.h>        //https://github.com/bblanchon/ArduinoJson
#include <SPI.h>
#include "secrets.h"
#include "weewxWeather.h"

#include "Fonts/meteocons48pt7b.h"
#include "Fonts/meteocons24pt7b.h"
#include "Fonts/meteocons20pt7b.h"
#include "Fonts/meteocons16pt7b.h"

#include "Fonts/moon_phases20pt7b.h"
#include "Fonts/moon_phases36pt7b.h"

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

#define ONE_HOUR          3600
#define ONE_DEGREE        3600
#define ONE_DAY          86400
#define UNIX_OFFSET  946684800
#define NTP_OFFSET  3155673600
#define SRAM_CS              8
#define EPD_CS              10
#define EPD_DC               9  
#define EPD_RESET           -1
#define EPD_BUSY            -1
#define NEOPIXELPIN         40
#define WEBSERVICEPORT      80

#define TZ_OFFSET       -21600 // America/Chicago

int                 wifistatus         = WL_IDLE_STATUS;
unsigned long       lastConnectionTime = 0; // last time you connected to the server, in milliseconds
int                 displayMode        = 1; // Button A pressed
String              urlc; // URL of current conditions
String              urlf; // URL of forecast 

// This is for the 2.7" tricolor EPD
Adafruit_IL91874 gfx(264, 176 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

AirliftweewxWeather owclient(&Serial);
weewxWeatherCurrentData owcdata;
weewxWeatherForecastData owfdata[3];

Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(1, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

//WiFiSSLClient client;
WiFiClient client;

// bool debug = false;
const char *moonphasenames[29] = {
  "New Moon",
  "Waxing Crescent",
  "Waxing Crescent",
  "Waxing Crescent",
  "Waxing Crescent",
  "Waxing Crescent",
  "Waxing Crescent",
  "Quarter",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Full Moon",
  "Waning Gibbous",
  "Waning Gibbous",
  "Waning Gibbous",
  "Waning Gibbous",
  "Waning Gibbous",
  "Waning Gibbous",
  "Last Quarter",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent"
};

int8_t readButtons(void) {
  uint16_t reading = analogRead(A3);

  if (reading > 600) {
    return 0; // no buttons pressed
  }
  if (reading > 400) {
    return 4; // button D pressed
  }
  if (reading > 250) {
    return 3; // button C pressed
  }
  if (reading > 125) {
    return 2; // button B pressed
  }
  return 1;   // button A pressed
}

bool wifi_connect() {
  Serial.print("Connecting to WiFi... ");
  
  WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI); // Unique to Adafruit_WiFiNINA

  // Check for the WiFi module
  if(WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    displayError("Communication with WiFi module failed!");
    while(true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  neopixel.setPixelColor(0, neopixel.Color(0, 0, 255));  // Set pixel blue while connecting
  neopixel.show(); 

  int wifitimeout = 15;
  if (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) == WL_CONNECT_FAILED) {
    while ((wifistatus = WiFi.status()) != WL_CONNECTED && wifitimeout > 0) {
      delay(1000);
      Serial.print(".");
      wifitimeout--;
      }
  }
  if (wifitimeout == 0) {
    Serial.println("WiFi connection timeout with error " + String(wifistatus));
    displayError("WiFi connection timeout with error " + String(wifistatus));
    neopixel.setPixelColor(0, neopixel.Color(0, 0, 0));
    neopixel.show();
    printWifiStatus;
    return false;
  }
  neopixel.setPixelColor(0, neopixel.Color(0, 0, 0));
  neopixel.show(); 
  Serial.println("Connected");
  printWifiStatus;
  return true;
}

void wget(String &url, char *buff) {
  int pos1 = url.indexOf("/",0);
  int pos2 = url.indexOf("/",8);
  String host = url.substring(pos1+2, pos2);
  String path = url.substring(pos2);
  Serial.println("wget(host=" + host + ", path=" + path + ")");
  httpRequest(host, path, buff);
}

void httpRequest(String &host, String &path, char *buff) {
  //WiFiSSLClient client;
  //WiFiClient client;

  // Set neoPixel blue when going online
  neopixel.setPixelColor(0, neopixel.Color(0, 0, 255));
  neopixel.show();
  
  // Stop the client before going on
  client.stop();
  
  if (client.connect(host.c_str(), WEBSERVICEPORT)) {
    Serial.println("Connecting...");
    // Make a HTTP request:
    client.println(String("GET ") + path + String(" HTTP/1.0"));
    client.println("Host: " + host);
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();

    uint32_t bytes = 0;
    int capturepos = 0;
    bool capture = false;
    int linelength = 0;
    char lastc = '\0';
    while(true) {
      while (client.available()) {
        char c = client.read();
        //Serial.print(c);
        if((c == '\n') && (lastc == '\r')) {
          if(linelength == 0) {
            capture = true;
          }
          linelength = 0;
        } else if(capture) {
          buff[capturepos++] = c;
          //Serial.write(c);
        } else {
          if((c != '\n') && (c != '\r')) linelength++;
        }
        lastc = c;
        bytes++;
      }
      // to get out of the while(true) loop ---
      // if the server's disconnected, stop the client:
      if (!client.connected()) {
        Serial.println("Disconnecting from server");
        client.stop();
        buff[capturepos] = '\0';
        Serial.println("Captured " + String(capturepos) + " bytes");
        break;
      }
    }
  } else {
    Serial.println("Problem connecting to " + host + ":" + String(WEBSERVICEPORT));
    printWifiStatus();
    buff[0] = '\0';
  }
  neopixel.setPixelColor(0, neopixel.Color(0, 0, 0));
  neopixel.show(); 
}

int getStringLength(String s) {
  int16_t  x = 0, y = 0;
  uint16_t w, h;
  gfx.getTextBounds(s, 0, 0, &x, &y, &w, &h);
  return w + x;
}

/* The return value is percent of moon cycle ( from 0.0 to 0.999999), i.e.:
0.0:   New Moon
0.125: Waxing Crescent Moon
0.25:  Quarter Moon
0.375: Waxing Gibbous Moon
0.5:   Full Moon
0.625: Waning Gibbous Moon
0.75:  Last Quarter Moon
0.875: Waning Crescent Moon
*/
float getMoonPhase(time_t tdate) {
  time_t newmoonref = 1263539460; //known new moon date (2010-01-15 07:11)
  // moon phase is 29.5305882 days, which is 2551442.82048 seconds
  float phase = abs( tdate - newmoonref) / (double)2551442.82048;
  phase -= (int)phase; // leave only the remainder
  if(newmoonref > tdate)
  phase = 1 - phase;
  return phase;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void displayError(String str) {
    // show error on display
    neopixel.setPixelColor(0, neopixel.Color(255, 0, 0));
    neopixel.show(); 
    Serial.print("displayError(): ");
    Serial.println(str);

    gfx.setTextColor(EPD_BLACK);
    gfx.powerUp();
    gfx.clearBuffer();
    gfx.setTextWrap(true);
    gfx.setCursor(10,60);
    gfx.setFont(&FreeSans12pt7b);
    gfx.print(str);
    gfx.display();
    neopixel.setPixelColor(0, neopixel.Color(0, 0, 0));
    neopixel.show();
}

void displayHeading(weewxWeatherCurrentData &owcdata) {
  Serial.println("displayHeading()");
  //time_t local = owcdata.observationTime + (uint8_t) owcdata.timezone;
  time_t local = owcdata.observationTime + TZ_OFFSET;
  Serial.print("Weather time: ");
  Serial.println((unsigned long) owcdata.observationTime);
  Serial.print("Location offset: ");
  Serial.println((uint8_t) owcdata.timezone);
  struct tm *timeinfo = gmtime(&local);

  // Format the date into a string
  char datestr[80];
  strftime(datestr, sizeof(datestr),"%a, %b %d", timeinfo);
  Serial.print("Date: ");
  Serial.println(datestr);
  gfx.setFont(&FreeSans18pt7b);
  gfx.setCursor((gfx.width()-getStringLength(datestr))/2,30);
  gfx.print(datestr);
  
  // Print the city name
  strftime(datestr, sizeof(datestr), "%A", timeinfo);
  gfx.setFont(&FreeSansBold12pt7b);
  gfx.setCursor((gfx.width()-getStringLength(owcdata.cityName))/2, 60);
  gfx.print(owcdata.cityName);
  Serial.print("City: ");
  Serial.println(owcdata.cityName);
}

void displayForecastDays(weewxWeatherCurrentData &owcdata, weewxWeatherForecastData owfdata[], int count = 3) {
  Serial.println("displayForecastDays()");
  for(int i=0; i < count; i++) {
    // day
    //time_t local = owfdata[i].observationTime + owcdata.timezone;
    time_t local = owfdata[i].observationTime + TZ_OFFSET;

    struct tm *timeinfo = gmtime(&local);
    char strbuff[80];
    strftime(strbuff, sizeof(strbuff), "%I", timeinfo);
    String datestr = String(atoi(strbuff));
    strftime(strbuff,80,"%p",timeinfo);
    // convert AM/PM to lowercase
    strbuff[0] = tolower(strbuff[0]);
    strbuff[1] = tolower(strbuff[1]);
    datestr = datestr + " " + String(strbuff);
    gfx.setFont(&FreeSans9pt7b);
    gfx.setCursor(i*gfx.width()/3 + (gfx.width()/3-getStringLength(datestr))/2,94);
    gfx.print(datestr);
    
    // weather icon
    String wicon = owclient.getMeteoconIcon(owfdata[i].icon);
    gfx.setFont(&meteocons20pt7b);
    gfx.setCursor(i*gfx.width()/3 + (gfx.width()/3-getStringLength(wicon))/2,134);
    gfx.print(wicon);
  
    // weather main description
    gfx.setFont(&FreeSans9pt7b);
    gfx.setCursor(i*gfx.width()/3 + (gfx.width()/3-getStringLength(owfdata[i].main))/2,154);
    gfx.print(owfdata[i].main);

    // temperature
    int itemp = (int)(owfdata[i].temperature + .5);
    int color = EPD_BLACK;
    if((OWM_METRIC && itemp >= METRIC_HOT)|| (!OWM_METRIC && itemp >= ENGLISH_HOT))
      color = EPD_RED;
    gfx.setTextColor(color);
    gfx.setFont(&FreeSans9pt7b);
    gfx.setCursor(i*gfx.width()/3 + (gfx.width()/3-getStringLength(String(itemp)))/2,172);
    gfx.print(itemp);
    gfx.drawCircle(i*gfx.width()/3 + (gfx.width()/3-getStringLength(String(itemp)))/2 + getStringLength(String(itemp)) + 6,163,3,color);
    gfx.drawCircle(i*gfx.width()/3 + (gfx.width()/3-getStringLength(String(itemp)))/2 + getStringLength(String(itemp)) + 6,163,2,color); 
    gfx.setTextColor(EPD_BLACK);   
  }  
}

void displayForecast(weewxWeatherCurrentData &owcdata, weewxWeatherForecastData owfdata[], int count = 3) {
  Serial.println("displayForecast()");
  gfx.powerUp();
  gfx.clearBuffer();
  neopixel.setPixelColor(0, neopixel.Color(0, 255, 0));
  neopixel.show();  

  gfx.setTextColor(EPD_BLACK);
  displayHeading(owcdata);

  displayForecastDays(owcdata, owfdata, count);
  gfx.display();
  gfx.powerDown();
  neopixel.setPixelColor(0, neopixel.Color(0, 0, 0));
  neopixel.show(); 
}

void displayAllWeather(weewxWeatherCurrentData &owcdata, weewxWeatherForecastData owfdata[], int count = 3) {
  Serial.println("displayAllWeather()");
  gfx.powerUp();
  gfx.clearBuffer();
  neopixel.setPixelColor(0, neopixel.Color(0, 255, 0));
  neopixel.show();  

  gfx.setTextColor(EPD_BLACK);

  // date string
  //time_t local = (unsigned long) owcdata.observationTime + owcdata.timezone;
  time_t local = owcdata.observationTime + TZ_OFFSET;
  struct tm *timeinfo = gmtime(&local);
  char datestr[80];
  // date
  strftime(datestr, sizeof(datestr), "%a, %b %d %Y", timeinfo);
  gfx.setFont(&FreeSans9pt7b);
  gfx.setCursor((gfx.width()-getStringLength(datestr))/2,14);
  gfx.print(datestr);
  
  // weather icon
  String wicon = owclient.getMeteoconIcon(owcdata.icon);
  gfx.setFont(&meteocons24pt7b);
  gfx.setCursor((gfx.width()/3-getStringLength(wicon))/2, 56);
  gfx.print(wicon);

  // weather main description
  gfx.setFont(&FreeSans9pt7b);
  gfx.setCursor((gfx.width()/3-getStringLength(owcdata.main))/2,72);
  gfx.print(owcdata.main);

  // temperature
  gfx.setFont(&FreeSansBold24pt7b);
  int itemp = owcdata.temperature + .5;
  int color = EPD_BLACK;
  if((OWM_METRIC && (int)itemp >= METRIC_HOT)|| (!OWM_METRIC && (int)itemp >= ENGLISH_HOT))
    color = EPD_RED;
  gfx.setTextColor(color);
  gfx.setCursor(gfx.width()/3 + (gfx.width()/3-getStringLength(String(itemp)))/2, 58);
  gfx.print(itemp);
  gfx.setTextColor(EPD_BLACK);

  // draw temperature degree as a circle (not available as font character
  gfx.drawCircle(gfx.width()/3 + (gfx.width()/3 + getStringLength(String(itemp)))/2 + 8, 58-30, 4, color);
  gfx.drawCircle(gfx.width()/3 + (gfx.width()/3 + getStringLength(String(itemp)))/2 + 8, 58-30, 3, color);

  // draw moon
  // draw Moon Phase
  float moonphase = getMoonPhase(owcdata.observationTime);
  int moonage = 29.5305882 * moonphase;
  //Serial.println("moon age: " + String(moonage));
  // convert to appropriate icon
  String moonstr = String((char)((int)'A' + (int)(moonage*25./30)));
  gfx.setFont(&moon_phases20pt7b);
  // font lines look a little thin at this size, drawing it a few times to thicken the lines
  gfx.setCursor(2*gfx.width()/3 + (gfx.width()/3-getStringLength(moonstr))/2, 56);
  gfx.print(moonstr);  
  gfx.setCursor(2*gfx.width()/3 + (gfx.width()/3-getStringLength(moonstr))/2+1, 56);
  gfx.print(moonstr);  
  gfx.setCursor(2*gfx.width()/3 + (gfx.width()/3-getStringLength(moonstr))/2, 56-1);
  gfx.print(moonstr);  

  // draw moon phase name
  int currentphase = moonphase * 28. + .5;
  gfx.setFont(); // system font (smallest available)
  gfx.setCursor(2*gfx.width()/3 + max(0,(gfx.width()/3 - getStringLength(moonphasenames[currentphase]))/2), 62);
  gfx.print(moonphasenames[currentphase]);

  displayForecastDays(owcdata, owfdata, count);
  gfx.display();
  gfx.powerDown();
  neopixel.setPixelColor(0, neopixel.Color(0, 0, 0));
  neopixel.show();
}

void displayCurrentConditions(weewxWeatherCurrentData &owcdata) {
  Serial.println("displayCurrentConditions()");
  gfx.powerUp();
  gfx.clearBuffer();
  neopixel.setPixelColor(0, neopixel.Color(0, 255, 0));
  neopixel.show();  

  gfx.setTextColor(EPD_BLACK);
  displayHeading(owcdata);

  // weather icon
  String wicon = owclient.getMeteoconIcon(owcdata.icon);
  gfx.setFont(&meteocons48pt7b);
  gfx.setCursor((gfx.width()/2-getStringLength(wicon))/2, 156);
  gfx.print(wicon);

  // weather main description
  gfx.setFont(&FreeSans9pt7b);
  gfx.setCursor(gfx.width()/2 + (gfx.width()/2-getStringLength(owcdata.main))/2, 160);
  gfx.print(owcdata.main);

  // temperature
  gfx.setFont(&FreeSansBold24pt7b);
  int itemp = owcdata.temperature + .5;
  Serial.println("Temperature: " + itemp);
  int color = EPD_BLACK;
  if((OWM_METRIC && (int)itemp >= METRIC_HOT)|| (!OWM_METRIC && (int)itemp >= ENGLISH_HOT))
    color = EPD_RED;
  gfx.setTextColor(color);
  gfx.setCursor(gfx.width()/2 + (gfx.width()/2-getStringLength(String(itemp)))/2, 130);
  gfx.print(itemp);
  gfx.setTextColor(EPD_BLACK);
  
  // draw temperature degree as a circle (not available as font character
  gfx.drawCircle(gfx.width()/2 + (gfx.width()/2 + getStringLength(String(itemp)))/2 + 10, 130-26,4, color);
  gfx.drawCircle(gfx.width()/2 + (gfx.width()/2 + getStringLength(String(itemp)))/2 + 10, 130-26,3, color);
  
  gfx.display();
  gfx.powerDown();
  neopixel.setPixelColor(0, neopixel.Color(0, 0, 0));
  neopixel.show(); 
}

void displaySunMoon(weewxWeatherCurrentData &owcdata) {
  Serial.println("displaySunMoon()");
  gfx.powerUp();
  gfx.clearBuffer();
  neopixel.setPixelColor(0, neopixel.Color(0, 255, 0));
  neopixel.show();  

  gfx.setTextColor(EPD_BLACK);
  displayHeading(owcdata);

  // draw Moon Phase
  float moonphase = getMoonPhase(owcdata.observationTime);
  int moonage = 29.5305882 * moonphase;
  // convert to appropriate icon
  String moonstr = String((char)((int)'A' + (int)(moonage*25./30)));
  gfx.setFont(&moon_phases36pt7b);
  gfx.setCursor((gfx.width()/3-getStringLength(moonstr))/2,140);
  gfx.print(moonstr);

  // draw moon phase name
  int currentphase = moonphase * 28. + .5;
  gfx.setFont(&FreeSans9pt7b);
  gfx.setCursor(gfx.width()/3 + max(0,(gfx.width()*2/3 - getStringLength(moonphasenames[currentphase]))/2),110);
  gfx.print(moonphasenames[currentphase]);

  // draw sunrise/sunset
  // sunrise/sunset times
  // sunrise

  //time_t local = (unsigned long) owcdata.sunrise + owcdata.timezone + 30;  // round to nearest minute
  time_t local = owcdata.sunrise + 30 + TZ_OFFSET;
  struct tm *timeinfo = gmtime(&local);
  char strbuff[80];
  strftime(strbuff, sizeof(strbuff), "%I", timeinfo);
  String datestr = String(atoi(strbuff));
  strftime(strbuff,80,":%M %p",timeinfo);
  datestr = datestr + String(strbuff) + " - ";
  // sunset
  //local = owcdata.sunset + owcdata.timezone + 30; // round to nearest minute
  local = owcdata.sunset + 30 + TZ_OFFSET;
  timeinfo = gmtime(&local);
  strftime(strbuff,80,"%I",timeinfo);
  datestr = datestr + String(atoi(strbuff));
  strftime(strbuff,80,":%M %p",timeinfo);
  datestr = datestr + String(strbuff);

  gfx.setFont(&FreeSans9pt7b);
  int datestrlen = getStringLength(datestr);
  int xpos = (gfx.width() - datestrlen)/2;
  gfx.setCursor(xpos,166);
  gfx.print(datestr);

  // draw sunrise icon
  // sun icon is "B"
  String wicon = "B";
  gfx.setFont(&meteocons16pt7b);
  gfx.setCursor(xpos - getStringLength(wicon) - 12,174);
  gfx.print(wicon);

  // draw sunset icon
  // sunset icon is "A"
  wicon = "A";
  gfx.setCursor(xpos + datestrlen + 12,174);
  gfx.print(wicon);

  gfx.display();
  gfx.powerDown();
  neopixel.setPixelColor(0, neopixel.Color(0, 0, 0));
  neopixel.show(); 
}


/* Start neoPixel and graphics display */
void setup() {
  // Setup neoPixel
  neopixel.begin();
  neopixel.show();
  // Setup ePaper display
  gfx.begin();
  Serial.println("ePaper display initialized");
  gfx.setRotation(2);
  gfx.setTextWrap(false);


  // Initialize current and forecasted weather request URLs
  urlc = owclient.buildUrlCurrent(OWM_KEY, OWM_LOCATION);
  Serial.println(urlc);
  urlf = owclient.buildUrlForecast(OWM_KEY, OWM_LOCATION);
  Serial.println(urlf);

  // Connect to wifi access point
  wifi_connect();
}

static uint32_t timer = millis();
static uint8_t lastbutton = 1;
static bool firsttime = true;
char data[4000];

void loop() {
  displayMode = readButtons();
  // update weather data at specified interval or when button 4 is pressed
  if((millis() >= (timer + 1000*60*UPDATE_INTERVAL)) || (displayMode == 4) || firsttime) {
    Serial.println("Getting weather data");
    firsttime = false;
    timer = millis();
    
    // Get current weather into data array
    wget(urlc, data);
      // if (strlen(data) == 0 && retry < 0) {
      //  displayError("Cannot get weather data, press reset to restart");
      //  while(1);      
      //}
      if (strlen(data) == 0 ) delay(60000); // Wait a minute before trying again
    } while(strlen(data) == 0);
    Serial.print("JSON data: ");
    Serial.println(data);
    int retry = 6;
    while(!owclient.updateCurrent(owcdata, data)) {
      retry--;
      //if(retry < 0) {
      //  displayError(owclient.getError());
      //  while(1);
      //}
      delay(5000);
    }
    Serial.print("Returned observation time: "); Serial.println((unsigned long) owcdata.observationTime);
    
    wget(urlf, data);
    Serial.println("Data retrieved:");
    Serial.println(data);
    if(!owclient.updateForecast(owfdata[0], data, 0)) {
      displayError(owclient.getError());
      while(1);
    }
    if(!owclient.updateForecast(owfdata[1],data,2)) {
      displayError(owclient.getError());
      while(1);
    }
    if(!owclient.updateForecast(owfdata[2],data,4)) {
      displayError(owclient.getError());
      while(1);
    }

    switch(lastbutton) {
      case 1:        
        displayAllWeather(owcdata, owfdata, 3);
        break;
      case 2:
        displayCurrentConditions(owcdata);
        break;
      case 3:
        displaySunMoon(owcdata);
        break;
    }
  }

  Serial.print("Button "); Serial.print(displayMode); Serial.println(" pressed");

  switch(displayMode) {
    case 0: // no button pressed
      break;
    case 1:
      displayAllWeather(owcdata, owfdata, 3);
      break;
    case 2:
      displayCurrentConditions(owcdata);
      break;
    case 3:
      displaySunMoon(owcdata);
      break;
    case 4:
      displayForecast(owcdata, owfdata, 3);
      break;
  }
  
  lastbutton = displayMode;
  
}
