void setup() {
  // initalise digital pins for LEDs as outputs
  pinMode(2, OUTPUT); // LED 1
  pinMode(3, OUTPUT); // LED 2
  pinMode(4, OUTPUT); // LED 3

  // initalise digital pins for buttons as inputs
  // and initialise internal pullups
  pinMode(9, INPUT_PULLUP); // button K1
  pinMode(10, INPUT_PULLUP); // button K2
  pinMode(11, INPUT_PULLUP); // button K3
}

void loop()
{
  if (digitalRead(9) == LOW)
  {
    digitalWrite(2, HIGH);
    delay(10);
    digitalWrite(2, LOW);
  }

  if (digitalRead(10) == LOW)
  {
    digitalWrite(3, HIGH);
    delay(10);
    digitalWrite(3, LOW);
  }

  if (digitalRead(11) == LOW)
  {
    digitalWrite(4, HIGH);
    delay(10);
    digitalWrite(4, LOW);
  }
}
