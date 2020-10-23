/*
 Standalone Sketch to use with a Arduino Fio and a 
 Sharp Optical Dust Sensor GP2Y1010AU0F
 
 Blog: http://arduinodev.woofex.net/2012/12/01/standalone-sharp-dust-sensor/
 Code: https://github.com/Trefex/arduino-airquality/
 For Pin connections, please check the Blog or the github project page
 Authors: Cyrille Médard de Chardon (serialC), Christophe Trefois (Trefex)
 Changelog:
   2012-Dec-01: Cleaned up code
   2012-Dec-13: Converted mg/m3 to ug/m3 which seems to be the accepted standard
 This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter 
 to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA. 
 */

#define VOLTAGE 5.0 // Arduino supply voltage
 
int measurePin = A0;
int ledPower = 10;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

const int numReadings = 100;    // samples are taken at 100Hz so calculate average over 1sec

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
long int total = 0;             // the running total
int latest_reading = 0;         // the latest reading
int average_reading = 0;        // the average reading

void setup(){
  Serial.begin(115200);
  pinMode(ledPower,OUTPUT);
  // Initialise sample buffer
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
     readings[thisReading] = 0;
  }
}

// Dust sample interrupt service routine
void takeReading() {
   // subtract the last reading:
   total = total - readings[readIndex];
   // read from the sensor:
   latest_reading = analogRead(dustPin);
   readings[readIndex] = latest_reading;
   // add the reading to the total:
   total = total + latest_reading;
   // advance to the next position in the array:
   readIndex = readIndex + 1;

   // if we're at the end of the array...wrap around to the beginning:
   if (readIndex >= numReadings) readIndex = 0;

   // calculate the average:
   average_reading = total / numReadings; // Seems to work OK with integer maths - but total does need to be long int
}

uint32_t timer = millis();

void loop(){
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();
  // approximately every second or so, print out the dust reading
  if (millis() - timer > 1000) {
    timer = millis(); // reset the timer
    digitalWrite(ledPower,LOW); // power on the LED
    delayMicroseconds(samplingTime);

    //voMeasured = analogRead(measurePin); // read the dust value
  
    delayMicroseconds(deltaTime);
    digitalWrite(ledPower,HIGH); // turn the LED off
    delayMicroseconds(sleepTime);

    // 0 - 5.0V mapped to 0 - 1023 integer values 
    calcVoltage = latest_reading * (VOLTAGE / 1023.0); 
  
    // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
    // Chris Nafis (c) 2012
    dustDensity = (0.17 * calcVoltage - 0.1)*1000; 

    Serial.print("delayMu(): ");
    Serial.print("Voltage: ");
    Serial.print(latest_reading);
  
    Serial.print("\t - A0: ");
    Serial.print(calcVoltage);
  
    Serial.print(" - Dust Density [ug/m3]: ");
    Serial.println(dustDensity);
  }
}
