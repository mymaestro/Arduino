#include <TTSDisplay.h>
TTSDisplay rtcshield;

int a = 0;
int b = 0;

void setup() {}

void loop() {
  // display some numbers
  for (a = 4921; a < 5101; a++) {
    rtcshield.num(a);
    delay(10);
  }

  // clear display
  rtcshield.clear();

  // display individual digits
  for (a = 3; a >= 0; --a) {
    rtcshield.display(a, a);
    delay(1000);
    rtcshield.clear();
  }
  for (a = 3; a >= 0; --a) {
    rtcshield.display(a, a);
    delay(1000);
    rtcshield.clear();
  }

  // turn the colon and off
  for (a = 0; a < 5; a++) {
    rtcshield.pointOn();
    delay(500);
    rtcshield.pointOff();
    delay(500);
  }

  // demo the time display function
  rtcshield.pointOn();
  rtcshield.time(11, 57);
  delay(1000);
  rtcshield.time(11, 58);
  delay(1000);
  rtcshield.time(11, 59);
  delay(1000);
  rtcshield.time(12, 00);
  delay(1000);
}
