void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(2, OUTPUT); // LED 1
  pinMode(3, OUTPUT); // LED 2
  pinMode(4, OUTPUT); // LED 3
  pinMode(5, OUTPUT); // LED 4
}

int x = 200;

void loop() {
  digitalWrite(2, HIGH);    // turn on LED1
  delay(x);
  digitalWrite(2, LOW);    // turn off LED1. Process repeats for the other three LEDs
  digitalWrite(3, HIGH);
  delay(x);
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  delay(x);
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  delay(x);
  digitalWrite(5, LOW);
}
