/* 	Audio camera trigger by Matt Richardson
        Heavily modified by Warren Gill
	This is a basic motion detecting camera & flash trigger for Arduino.
	Use a piezo element for the sensor (see http://www.arduino.cc/en/Tutorial/KnockSensor)
	Use opto isolators (aka optocouplers) for the flash and camera triggers
	Camera must be in BULB mode for shutter release to work
*/
#define WORKLIGHT_RELAY      2 // for turning on the work light
#define FOCUS_TRIGGER_PIN    3 // 1k resistor to pin 1 of a 4N35 to the flash
#define SHUTTER_TRIGGER_PIN  4 // 1k resistor to pin 1 of a 4N35 to the shutter
#define FLASH_TRIGGER_PIN    5 // 1k resistor to pin 1 of a 4N35
#define BUTTON_PIN           6 // Push button
#define LED_PIN             13 // Status LED
#define SENSOR_PIN          A0 // Data pin of PIR sensor

#define STANDBY              0
#define ACTIVE               1
#define SENSOR_THRESHOLD     0

const unsigned int calibrationTime = 10; // The time we give the sensor to calibrate
                                         // (10-60 secs according to the datasheet)
const unsigned int BAUD_RATE = 9600;     // Serial port baud rate

int mode = STANDBY;

// For best results, set flashDelayMS according to what type
// of shot you're doing. 0 seems best for balloon burst while
// 10 seems best for shattering glass. YMMV.
long flashDelayMS = 100;

void setup() {
  pinMode(WORKLIGHT_RELAY, OUTPUT);
  pinMode(FOCUS_TRIGGER_PIN, OUTPUT);
  pinMode(SHUTTER_TRIGGER_PIN, OUTPUT);
  pinMode(FLASH_TRIGGER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(WORKLIGHT_RELAY, HIGH); //turn the lights on
  digitalWrite(FOCUS_TRIGGER_PIN, LOW);
  digitalWrite(SHUTTER_TRIGGER_PIN, LOW);
  digitalWrite(FLASH_TRIGGER_PIN, LOW);
  digitalWrite(BUTTON_PIN, HIGH); //turn on internal 20 k pullup resistor so the open input state is HIGH.
  digitalWrite(LED_PIN, HIGH); //turn the LED on
  Serial.begin(BAUD_RATE);
  Serial.print("Calibrating sensor");
  for(int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    flashLED(1);
    delay(900);
  }
  Serial.println(" done.");
  Serial.println("Sensor active.");
  delay(50);
  flashLED(2);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("Activating"); 
    mode = ACTIVE;
    digitalWrite(WORKLIGHT_RELAY, LOW); // turn the lights off
    // Give time for light to go down and settle after button push
    // flash the led
    flashLED(4);
    digitalWrite(LED_PIN, LOW); // show we're ready
    digitalWrite(FOCUS_TRIGGER_PIN, HIGH); // camera focus
    digitalWrite(SHUTTER_TRIGGER_PIN, HIGH); // open the camera shutter
  }
  if ((mode == ACTIVE) && (digitalRead(SENSOR_PIN))) {
    // If we're in ACTIVE mode and we sense a pop:
    Serial.println("Photo!"); 
    delay(flashDelayMS);
    digitalWrite(FLASH_TRIGGER_PIN, HIGH); // Fire the flash
    delay(50);
    digitalWrite(FLASH_TRIGGER_PIN, LOW);
    digitalWrite(FOCUS_TRIGGER_PIN, LOW); // camera focus off
    digitalWrite(SHUTTER_TRIGGER_PIN, LOW); // close camera shutter
    mode = STANDBY;
    delay(500); // wait before turning lights on
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(WORKLIGHT_RELAY, HIGH); // turn lights back on
    Serial.println("Done.");
  }
}

boolean flashLED(int n) {
  for(int i = 0; i < n; i++) {
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
  }
  return true;
}

