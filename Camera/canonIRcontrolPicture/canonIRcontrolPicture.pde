/*
Arduino sketch for simulating a Canon RC-1 IR remote control to start and stop video recording on a Canon 5D Mark II or 7D
2010, Martin Koch
http://controlyourcamera.blogspot.com/
Huge thanks go to http://www.doc-diy.net/photo/rc-1_hacked/index.php for figuring out the IR code.
*/

#define irLED 12
#define STATUSLED 13
#define SWITCH 7

unsigned int pulseDuration = 10; //microseconds 
//The required 15 microseconds pulseDuration didn't work since digitalWrite consumes some additional time
//thats adds to pulseDuration value. 10 to 12 microseconds worked for me.

unsigned int photo = 7330; //A 7330 microseconds delay between bursts shoots a photo.
unsigned int video = 5360; //A 5360 microseconds delay between bursts starts/stops video recording. 

void setup() {
  pinMode(irLED, OUTPUT);
  pinMode(STATUSLED, OUTPUT);
  pinMode(SWITCH, INPUT);
  digitalWrite(SWITCH, HIGH); //turn on internal 20 k pullup resistor so the open input state is HIGH.
  digitalWrite(STATUSLED, LOW);
}

void loop() { //run again and again 
  if (digitalRead(SWITCH) == LOW) { //read switch input
    digitalWrite(STATUSLED, HIGH);
    shoot(photo); //start video recording
    delay(1000); //wait one second
    digitalWrite(STATUSLED, LOW);
/*
    delay(660000); //record for 11 min * 60 s * 1000 ms
    shoot(photo); //stop video recording
    delay(1000); //1 s delay between stop and start of next clip
*/
  }
}

void shoot(unsigned int delayBetweenBursts) { //sends the IR signal
//send first 16 bursts
  for(int i=0; i<16; i++) { 
    digitalWrite(irLED, HIGH);
    delayMicroseconds(pulseDuration);
    digitalWrite(irLED, LOW);
    delayMicroseconds(pulseDuration);
  } 

  delayMicroseconds(delayBetweenBursts); 

//send second 16 bursts
  for(int i=0; i<16; i++) { 
    digitalWrite(irLED, HIGH);
    delayMicroseconds(pulseDuration);
    digitalWrite(irLED, LOW);
    delayMicroseconds(pulseDuration);
  } 

  return;
}

