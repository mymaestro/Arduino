#include <TTSDisplay.h>
#include <TTSTemp.h>

TTSTemp temp;
TTSDisplay rtcshield;

int a = 0;

void setup() {}

void loop() {

  a = temp.get();
  rtcshield.num(a);
  delay(500);
}
