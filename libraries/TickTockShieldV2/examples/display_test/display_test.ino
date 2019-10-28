// test display tm1636

#include <Streaming.h>
#include <TTSDisplay.h>
#include <Wire.h>

TTSDisplay display;

void setup()
{
    Serial.begin(115200);
    
    unsigned char dta[4] = {5, 5, 2, 3};
    
    //display.point(1);
    display.num(1025);
    
    delay(1000);
    
    while(1)
    {
        display.pointOn();
        delay(500);
        display.pointOff();
        delay(500);
    }
    cout << "hello world" << endl;
}

void loop()
{
    
}