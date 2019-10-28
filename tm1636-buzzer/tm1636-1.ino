void setup() {
  // buzzer on digital pin 6
  pinMode(6, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(6, HIGH);   // turn the buzzer on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(6, LOW);    // turn the buzzer off by making the voltage LOW
  delay(1000);                       // wait for a second
}
