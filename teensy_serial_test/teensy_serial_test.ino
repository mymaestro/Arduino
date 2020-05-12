void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(31250);
  Serial3.begin(31250);
  pinMode(13, OUTPUT); // LED pin

    digitalWriteFast(13, HIGH); // LED on
    delay(500);
    digitalWriteFast(13, LOW);
}
void loop() {
  // put your main code here, to run repeatedly:
digitalWriteFast(13, HIGH); // LED on
    delay(500);
    digitalWriteFast(13, LOW);
 delay(500);
}
