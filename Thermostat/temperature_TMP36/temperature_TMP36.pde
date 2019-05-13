/* Sensor test sketch for more information see http://www.ladyada.net/make/logshield/lighttemp.html
  */

#define LED_PIN 13
boolean isOn = true;
 
//TMP36 Pin Variables
#define aref_voltage 3.28  // TMP36 Vin connected to 3.3V
                           // My Arduino produces 3.28V instead of 3.3
int sensorPin = 1;         //the analog pin the TMP36's Vout (sense) pin is connected to
                           //the resolution is 10 mV / degree centigrade with a
                           //500 mV offset to allow for negative temperatures
int sensorReading;         // the analog reading from the sensor

float smoothingFactor = 93;  // Percentage of influence the last value has over its preceding average
float temperatureSmooth = 25; // Equates to room temperature (about 75F) in analog pin readings as voltage

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup(void) {
  pinMode(LED_PIN, OUTPUT);
  pinMode(sensorPin, INPUT);
  lcd.begin(16, 2);
  Serial.begin(9600);
  analogReference(EXTERNAL);
}

void loop(void) {
  sensorReading = analogRead(sensorPin);
 
  Serial.print("Reading: ");
  Serial.print(sensorReading);     // the raw analog reading
 
  // converting that reading to voltage, which is based off the reference voltage
  float voltage = sensorReading * aref_voltage / 1024 ;

  // print out the voltage
  Serial.print(", ");
  Serial.print(voltage); Serial.print("V, ");
  Serial.print("Temp: ");
 
  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                //to degrees ((volatge - 500mV) times 100)

  temperatureSmooth = temperatureSmooth * (smoothingFactor / 100) + temperatureC * ((100 - smoothingFactor) / 100);

  Serial.print(temperatureC); Serial.print("C, ");
 
  // now convert to Fahrenheight
  float temperatureF = (temperatureSmooth * 9.0 / 5.0) + 32.0;
  
  Serial.print(temperatureSmooth); Serial.print("C, ");
  Serial.print(temperatureF); Serial.println("F");
 
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(temperatureF);
  lcd.print("F");
  delay(500);

}
 
 

