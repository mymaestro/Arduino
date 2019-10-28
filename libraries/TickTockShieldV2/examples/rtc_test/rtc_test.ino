// rtc test

#include <Wire.h>
#include <TTSTime.h>
#include <Streaming.h>

TTSTime time;

int hour;
int min;
int sec;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    cout << "hello world" << endl;
    
    
    time.setTime(12, 59, 55);
}

void loop()
{
    cout << time.getHour() << " : " << time.getMin() << " : " << time.getSec() << endl;
    
    delay(1000);
}