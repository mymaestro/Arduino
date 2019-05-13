int hvacStatus;
int thermostatMode;

const float temperature_correction = -8;

const int defaultTemp = 68;
int desiredTemp;
int readTemp;

unsigned long nextLogTime;

unsigned long lastStatusChangeRequest;

int HEAT = 13;
int COOL = 12;

const int COOL = -1;
const int OFF = 0;
const int HEAT = 1;

void setup(void) {
  Serial.begin(9600);
  pinMode(HEAT, OUTPUT);
  pinMode(COOL, OUTPUT);
  hvacStatus = OFF;
  nextLogTime = 0;
  lastStatusChangeRequest = 0;
  thermostatMode = COOL;
  desiredTemp = defaultTemp;
}

void loop(void) {
  float temp_f = currentTemp();

  desiredTemp = readDesiredTemp();

  if (thermostatMode == HEAT) {
    checkHeat(temp_f);
  } else if (thermostatMode == COOL) {
    checkCool(temp_f);
  }
  if (nextLogTime <= millis()) {
    logToSerial(temp_f);
    nextLogTime += 15000;
  }
  delay(1000);
}

int readDesiredTemp() {
  if (Serial.available() > 0) {
    readTemp = Serial.read();
    Serial.flush();
    Serial.println(readTemp);
    return readTemp;
  } else {
    return desiredTemp;
  }
}

void checkCool(float temp_f) {
  if (hvacStatus == COOL && shouldTurnOffAc(temp_f)) {
    setHvacStatus(OFF);
  } else if (hvacStatus == OFF && shouldTurnOnAc(temp_f)) {
    setHvacStatus(COOL);
  }
}

void checkHeat(float temp_f) {
  if (hvacStatus == HEAT && shouldTurnOffHeat(temp_f)) {
    setHvacStatus(OFF);
  } else if (hvacStatus == OFF && shouldTurnOnHeat(temp_f)) {
    setHvacStatus(HEAT);
  }
}

float currentTemp() {
  float temp_volt = analogRead(A0) * 5.0 / 1023.0;
  float temp_c = 100 * temp_volt - 50;
  return (9.0 / 5) * temp_c + 32 + temperature_correction;
}

void logToSerial(float temp_f) {
  Serial.print(temp_f);
  Serial.print(" ");

  Serial.print(desiredTemp);
  Serial.print(" ");
  if (hvacStatus == HEAT) {
    Serial.println("heat-on");
  } else if (hvacStatus == COOL) {
    Serial.println("ac-on");
  } else {
    Serial.println("off");
  }
}

void setHvacStatus(int status) {
  Serial.print("setting hvac status: ");
  Serial.println(status);

  if (status == HEAT) {
    digitalWrite(HEAT, HIGH);
    digitalWrite(COOL, LOW);
  } else if (status == COOL) {
    digitalWrite(COOL, HIGH);
    digitalWrite(HEAT, LOW);
  } else {
    digitalWrite(HEAT, LOW);
    digitalWrite(COOL, LOW);
  }
  hvacStatus = status;
  thermostatMode = status;
}

bool shouldTurnOffAc(float temp) {
  if (temp < (desiredTemp - 1)) {
    if (changeRequestTimelyEnough(lastStatusChangeRequest)) {
      return true;
    }
    lastStatusChangeRequest = millis();
  }
  return false;
}

bool shouldTurnOnAc(float temp) {
  if (temp > (desiredTemp + 1)) {
    if (changeRequestTimelyEnough(lastStatusChangeRequest)) {
      return true;
    }
    lastStatusChangeRequest = millis();
  }

  return false;
}

bool shouldTurnOffHeat(float temp) {
  if (temp > (desiredTemp + 1)) {
    if (changeRequestTimelyEnough(lastStatusChangeRequest)) {
      return true;
    }
    lastStatusChangeRequest = millis();
  }

  return false;
}

bool shouldTurnOnHeat(float temp) {
  if (temp < (desiredTemp - 1)) {
    if (changeRequestTimelyEnough(lastStatusChangeRequest)) {
      return true;
    }
    lastStatusChangeRequest = millis();
  }

  return false;
}

bool changeRequestTimelyEnough(unsigned long previousRequest) {
  if (previousRequest > (millis() - 3000)) {
    return true;
  } else {
    return false;
  }
}

