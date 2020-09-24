#pragma once
#include "secrets.h"
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

typedef struct weewxWeatherCurrentData {
  //  datetime	"1598572800"
  time_t observationTime;
  //  temperature	"95.7"
  float temperature;
  //  temperature2	"100.0" (greenhouse)
  float temperature2;
  //  wind_chill	"95.7"
  float windchill;
  //  humidity	"50"
  uint8_t humidity;
  //  humidity2	"45"
  uint8_t humidity2;
  //  heat_index	"106.9"
  float heatindex;
  //  wind_direction	"N/A"
  String winddirection;
  //  wind_direction_degrees	"   N/A"
  float windeg;
  //  wind_speed	"0"
  float windspeed;
  //  gust_speed	"0"
  float gustspeed;
  //  gust_direction_degrees	"   N/A"
  float gustdeg;
  //  dewpoint	"74.0"
  float dewpoint;
  //  barometer	"29.589"
  float barometer;
  //  baromtrend	"-0.020"
  float baromtrend;
  //  rain_rate	"0.00"
  float rainrate;
  //  daily_rain	"0.00"
  float rainday;
  //  monthly_rain	"0.65"
  float rainmonth;
  //  total_rain	"19.82"
  float rainyear;

} weewxWeatherCurrentData;

typedef struct weewxWeatherHighs {
  // temperature "99.0"
  float temperature;
  // temperature_time	"1598564828"
  time_t temperaturetime;
humidity	"90"
humidity_time	"1598530796"
dewpoint	"77.2"
dewpoint_time	"1598558412"
windspeed	"5"
windspeed_time	"1598519400"
barometer	"29.717"
barometer_time	"1598543943"
rain_rate	"0.00"
rain_rate_time	"1598504401"
heat_index	"113.4"
heat_index_time	"1598559292"
  time_t observationTime;
  // "main":{
  //   "temp":17.35,
  float temperature;
  //   "temp_min":16.89,
  float tempMin;
  //   "temp_max":17.35,
  float tempMax;
  //   "pressure":970.8,
  float pressure;
  //   "sea_level":1030.62,
  float pressureSeaLevel;
  //   "grnd_level":970.8,
  float pressureGroundLevel;
  //   "humidity":97,
  uint8_t humidity;
  //   "temp_kf":0.46
  // },"weather":[{
  //   "id":802,
  uint16_t weatherId;
  //   "main":"Clouds",
  String main;
  //   "description":"scattered clouds",
  String description;
  //   "icon":"03d"
  String icon;
  String iconMeteoCon;
  // }],"clouds":{"all":44},
  uint8_t clouds;
  // "wind":{
  //   "speed":1.77,
  float windSpeed;
  //   "deg":207.501
  float windDeg;
  // rain: {3h: 0.055},
  float rain;
  // },"sys":{"pod":"d"}
  // dt_txt: "2018-05-23 09:00:00"
  String observationTimeText;

} weewxWeatherForecastData;

class AirliftweewxWeather{
  private:
    Stream *Serial;
    String currentKey;
    String currentParent;
    //weewxWeatherCurrentData *data;
    uint8_t weatherItemCounter = 0;
    bool metric = true;
    String language;
    String _error;

  public:
    AirliftweewxWeather(Stream *serial){Serial = serial;};
    String buildUrlCurrent(String appId, String locationParameter);
    String buildUrlForecast(String appId, String locationParameter);
    bool updateCurrent(weewxWeatherCurrentData &data,String json);
    bool updateForecast(weewxWeatherForecastData &data,String json, int day = 0);

    void setMetric(bool metric) {this->metric = metric;}
    bool isMetric() { return metric; }
    void setLanguage(String language) { this->language = language; }
    String getLanguage() { return language; }
    void setError(String error){_error = error;}
    String getError(){return _error;}

    String getMeteoconIcon(String icon);

};
