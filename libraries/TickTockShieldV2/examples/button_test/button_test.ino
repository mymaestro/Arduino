// test button

#include <Wire.h>
#include <Streaming.h>

#include <TTSButton.h>
#include <TTSBuzzer.h>
#include <TTSLight.h>
#include <TTSTemp.h>
#include <TTSLed.h>


TTSButton button1(TTSK1);
TTSButton button2(TTSK2);
TTSButton button3(TTSK3);

TTSBuzzer buz;

TTSLight light;

TTSTemp temp;

TTSLed led1(TTSLED1);
TTSLed led2(TTSLED2);
TTSLed led3(TTSLED3);
TTSLed led4(TTSLED4);

int state = 0;

void setup()
{

    Serial.begin(115200);
    cout << "hello world" << endl;
    
    cout << light.get() << endl;

    
}

void loop()
{
    if(button1.pressed())
    {
        delay(10);
        if(button1.pressed())
        {
            cout << "button1 pressed" << endl;
            
            state = 1-state;
            
            if(state)led1.on();
            else led1.off();
            
            cout << light.get() << endl;
        }
        
        while(button1.pressed());
        
    }
}