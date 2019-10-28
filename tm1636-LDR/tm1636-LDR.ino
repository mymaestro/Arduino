#include <TTSDisplay.h>
TTSDisplay rtcshield;

int a = 0;

void setup() {}
void loop()
{
  // read value of analog input
  a = analogRead(A1);
  // show value on display
  rtcshield.num(a);
  delay(100);
}
