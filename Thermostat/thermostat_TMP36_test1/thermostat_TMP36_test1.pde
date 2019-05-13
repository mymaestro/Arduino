/*
 Thermostat

 TMP36 Vin connected to 3.3V (my Arduino produces 3.28V instead of 3.3)
 The analog pin the TMP36's Vout (sense) pin is connected to A1
 The TMP36's resolution is 10 mV / degree centigrade with a 500 mV offset to allow for negative temperatures

*/

#define fanPin       A5
#define heatPin      A4
#define coolPin      A2
#define normalPin    A3
#define tmp36Pin      1
#define setPin       A0
#define setButton     2

#define aref_voltage 3.28
#define waitStatusTime 3000

boolean fanOn = false;
int sensorReading;
int hvacStatus;

const float temperature_correction = -8;

const int defaultTemp = 72;
int desiredTemp;
int readTemp;
float temperature_range = 5;
float temperature_high = 75;
float temperature_low = 65;

unsigned long nextLogTime;  // How often messages are printed to the serial port
unsigned long nextPrintTime; // How often messages are printed to the LCD
unsigned long lastStatusChangeRequest;

const int COOL = -1;
const int OFF = 0;
const int AUTO = 0;
const int HEAT = 1;


float smoothingFactor = 93;  // Percentage of influence the last value has over its preceding average
float temperatureSmooth = 25; // Equates to room temperature (about 75F) in analog pin readings as voltage

// include the LCD library code
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(3, 5, 6, 7, 8, 9);

void setup(void) {
  Serial.begin(9600);
  pinMode(fanPin, OUTPUT);
  pinMode(heatPin, OUTPUT);
  pinMode(coolPin, OUTPUT);
  pinMode(normalPin, OUTPUT);
  pinMode(setButton, INPUT);
  pinMode(setPin, INPUT);
  pinMode(tmp36Pin, INPUT);
  lcd.begin(16, 1);
  analogReference(EXTERNAL);
  hvacStatus = OFF;
  nextLogTime = 0;
  lastStatusChangeRequest = 0;
  temperature_range = map(analogRead(setPin), 0, 1023, 1, 50); // range;
  desiredTemp = defaultTemp; // later I will write a way to set this
}

void loop(void) {
  float temp_f = currentTemp();
  temperature_range = readDesiredRange();

  temperature_high = desiredTemp + temperature_range;
  temperature_low = desiredTemp - temperature_range;
  
  checkCool(temp_f);
  checkHeat(temp_f);
  
  // eventually I'll put this into an interrupt
  if (digitalRead(setButton)) {
    fanOn = !fanOn;
    if (fanOn) {
      digitalWrite(fanPin, HIGH);
    } else {
      digitalWrite(fanPin, LOW);
    }
    
  }
  
  if (nextLogTime <= millis()) {
    logToSerial(temp_f);
    nextLogTime += 2500;
   }
  if (nextPrintTime <= millis()) {
    logToLCD(temp_f);
    nextPrintTime += 2500;
   }

  delay(500);
}

int readDesiredRange() {
  return map(analogRead(setPin), 0, 1023, 1, 50);
}

void checkCool(float temp_f) {
  Serial.print("checkCool: ");
  Serial.println(temp_f);  
  if (hvacStatus == COOL && shouldTurnOffAc(temp_f)) {
    setHvacStatus(OFF);
  } else if (hvacStatus == OFF && shouldTurnOnAc(temp_f)) {
    setHvacStatus(COOL);
  }
}

void checkHeat(float temp_f) {
  Serial.print("checkHeat: ");
  Serial.println(temp_f);  
  if (hvacStatus == HEAT && shouldTurnOffHeat(temp_f)) {
    setHvacStatus(OFF);
  } else if (hvacStatus == OFF && shouldTurnOnHeat(temp_f)) {
    setHvacStatus(HEAT);
  }
}

float currentTemp() {
  // converting that reading to voltage, which is based off the reference voltage
  float voltage = analogRead(tmp36Pin) * aref_voltage / 1024 ;
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                //to degrees ((voltage - 500mV) times 100)

  temperatureSmooth = temperatureSmooth * (smoothingFactor / 100) + temperatureC * ((100 - smoothingFactor) / 100);

  // return Fahrenheight
  return (temperatureSmooth * 9.0 / 5.0) + 32.0;
}


void logToSerial(float temp_f) {
  Serial.println(" ");
  Serial.print(temp_f);
  Serial.print(" ");

  Serial.print(temperature_low);
  Serial.print("=>");
  Serial.print(temperature_high);
  Serial.print(" ");
  
  if (hvacStatus == HEAT) {
    Serial.println("heat");
  } else if (hvacStatus == COOL) {
    Serial.println("a/c");
  } else {
    Serial.println("off");
  }
  Serial.print("variance: ");
  Serial.println(temperature_range);
}

void logToLCD(float temp_f) {
  lcd.setCursor(0,0);
  lcd.print(int(temperature_low+0.5));
  lcd.print("-");
  lcd.print(int(temperature_high+0.5));
  lcd.print("  ");
  
  lcd.setCursor(8,0);
  switch (hvacStatus) {
    case COOL:
      lcd.print("cool");
      break;
    case HEAT:
      lcd.print("heat");
      break;
    default:
      lcd.print("    ");
      break;
  }

  lcd.setCursor(12,0);  
  if (fanOn) {
    lcd.print("*");
  } else {
    lcd.print(" ");
  }
  
  lcd.setCursor(14,0);
  lcd.print(int(temp_f+0.5));
  lcd.print("  ");

}

void setHvacStatus(int status) {
  Serial.print("setting hvac status: ");
  Serial.println(status);

  if (status == HEAT) {
    digitalWrite(heatPin, HIGH);
    digitalWrite(coolPin, LOW);
    digitalWrite(normalPin, LOW);
  } else if (status == COOL) {
    digitalWrite(coolPin, HIGH);
    digitalWrite(heatPin, LOW);
    digitalWrite(normalPin, LOW);
  } else {
    digitalWrite(heatPin, LOW);
    digitalWrite(coolPin, LOW);
    digitalWrite(normalPin, HIGH);
  }
  hvacStatus = status;
}

bool shouldTurnOffAc(float temp) {
  if (temp < (temperature_high - 1)) {
    if (changeRequestTimelyEnough(lastStatusChangeRequest)) {
      return true;
    }
    lastStatusChangeRequest = millis();
  }
  return false;
}

bool shouldTurnOnAc(float temp) {
  if (temp > temperature_high) {
    if (changeRequestTimelyEnough(lastStatusChangeRequest)) {
      return true;
    }
    lastStatusChangeRequest = millis();
  }
  return false;
}

bool shouldTurnOffHeat(float temp) {
  if (temp > (temperature_low + 1)) {
    if (changeRequestTimelyEnough(lastStatusChangeRequest)) {
      return true;
    }
    lastStatusChangeRequest = millis();
  }
  return false;
}

bool shouldTurnOnHeat(float temp) {
  if (temp < temperature_low) {
    if (changeRequestTimelyEnough(lastStatusChangeRequest)) {
      return true;
    }
    lastStatusChangeRequest = millis();
  }
  return false;
}

bool changeRequestTimelyEnough(unsigned long previousRequest) {
  if (previousRequest > (millis() - waitStatusTime)) {
    Serial.print("Enough time passed.");
    return true;
  } else {
    Serial.print("Not enough time passed.");
    return false;
  }
}


