/**********************
Motion Activated Camera
Dan Bridges 2009

For schematics and more documentation see:
http://www.dayofthenewdan.com/projects/motion-camera
**********************/


boolean focus = true;

int shutterPin = 2;
int focusPin = 3;
int PIRPin = 4;
int statusLED = 13;
int shutterPressDelay = 200;
int focusPressDelay = 200;
int interPictureDelay = 500;

void setup(){
  pinMode(shutterPin, OUTPUT);
  pinMode(focusPin, OUTPUT);
  pinMode(statusLED, OUTPUT);
}

void loop(){
    if (digitalRead(PIRPin)) {
       digitalWrite(statusLED, HIGH);
       takePicture();
       delay(interPictureDelay);
       digitalWrite(statusLED, LOW);
    }
}

void takePicture() {
    //If you want the camera to focus first set
    //the focus variable to true.
    if (focus) {
        digitalWrite(focusPin, HIGH);
        delay(focusPressDelay);
        digitalWrite(focusPin, LOW);
        delay(250);
    }
    digitalWrite(shutterPin, HIGH);
    delay(shutterPressDelay);
    digitalWrite(shutterPin, LOW);
}
