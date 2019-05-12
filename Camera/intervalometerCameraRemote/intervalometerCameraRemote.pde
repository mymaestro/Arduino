/**********************
Button Activated Camera
Dan Bridges 2009

For schematics and more documentation see:
http://www.dayofthenewdan.com/projects/motion-camera
**********************/


boolean focus = false;

int shutterPin = 2;
int focusPin = 3;
int shutterPressPin = 7;
int focusSwitch = 6;
int focusLED = 12;
int statusLED = 13;
int shutterPressDelay = 200;
int focusPressDelay = 300;
int focusPauseDelay = 250;
int interPictureDelay = 500;
int timeLapseDelay = 5000;

void setup(){
  pinMode(shutterPin, OUTPUT);
  pinMode(focusPin, OUTPUT);
  pinMode(statusLED, OUTPUT);
  pinMode(focusLED, OUTPUT);
  pinMode(focusSwitch, INPUT);
  pinMode(shutterPressPin, INPUT);
  digitalWrite(focusSwitch, HIGH); 
  digitalWrite(shutterPressPin, HIGH); //turn on internal 20 k pullup resistor so the open input state is HIGH.
  digitalWrite(statusLED, LOW);
  // wait for button press before looping
  while(digitalRead(shutterPressPin) == HIGH) {
    delay(500);
  }
}

void loop(){
  focus = (digitalRead(focusSwitch == LOW));
  digitalWrite(statusLED, HIGH);
  takePicture();
  delay(interPictureDelay);
  digitalWrite(statusLED, LOW);
  delay(timeLapseDelay);
}

void takePicture() {
    //If you want the camera to focus first set
    //the focus variable to true.
    if (focus) {
       digitalWrite(focusLED, HIGH);
        digitalWrite(focusPin, HIGH);
        delay(focusPressDelay);
        digitalWrite(focusPin, LOW);
        delay(focusPauseDelay);
    } else {
       digitalWrite(focusLED, LOW);
    }
    digitalWrite(shutterPin, HIGH);
    delay(shutterPressDelay);
    digitalWrite(shutterPin, LOW);
}
