// WG: Mod for Micro (TIMER2) and timeout delay for LED
volatile int rate[10];                    // used to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find the inter beat interval
volatile unsigned long lastLEDtime = 0;            // measures the minimum time for lighting the LED
volatile int P =512;                      // used to find peak in pulse wave
volatile int T = 512;                     // used to find trough in pulse wave
volatile int thresh = 512;                // used to find instant moment of heart beat
volatile int amp = 100;                   // used to hold amplitude of pulse waveform
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = true;       // used to seed rate array so we startup with reasonable BPM

void interruptSetup(){  // Initializes Timer1 to throw an interrupt every 2mS.
  TCCR1A = 0x02; // Disable PWM on pins D9 and D10, go into CTC mode
  TCCR1B = 0x05; // Don't force compare, 64 prescalar
  OCR1A = 0X7C;  // Set the top of the count to 124 for 500Hz sample rate
  TIMSK1 = 0x02; // Enable interrupt on match between TIMER1 and OCR1A
  sei();         // Enable global interrupts
} 

// Service TIMER1 interrupts
// Timer1 makes sure that we take a reading every 2 milliseconds

ISR(TIMER1_COMPA_vect){                         // triggered when Timer1 counts to 124
  cli();                                      // disable interrupts while we do this
  Signal = analogRead(pulsePin);              // read the Pulse Sensor 
  sampleCounter = millis();                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

  // Find the peak and trough of the pulse wave
  if (Signal < thresh && N > (IBI/5)*3) {     // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < T) {                         // T is the trough
      T = Signal;                             // keep track of lowest point in pulse wave 
      }
    }
      
    if (Signal > thresh && Signal > P) {      // thresh condition helps avoid noise
      P = Signal;                             // P is the peak
    }                                         // keep track of highest point in pulse wave
    
  // Look for the heartbeat
  // Signal surges up in value every time there is a pulse
  if (N > 250) {                              // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ) {
      Pulse = true;                           // set the Pulse flag when we think there is a pulse
      digitalWrite(blinkPin, HIGH);           // turn on pin 13 LED
      lastLEDtime = sampleCounter;            // remember when the LED was turned on
      IBI = sampleCounter - lastBeatTime;     // measure time between beats in mS
      lastBeatTime = sampleCounter;           // keep track of time for next pulse
         
      if (firstBeat) {                        // if it's the first time we found a beat, firstBeat == TRUE
        firstBeat = false;                    // clear firstBeat flag
        return;                               // IBI value is unreliable so discard it
      }   
      if (secondBeat) {                       // if this is the second beat, secondBeat == TRUE
        secondBeat = false;                   // clear secondBeat flag
        for (int i=0; i<=9; i++) {            // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;
        }
      }

    // keep a running total of the last 10 IBI values
    word runningTotal = 0;                   // clear the runningTotal variable    

    for (int i=0; i<=8; i++) {               // shift data in the rate array
      rate[i] = rate[i+1];                   // and drop the oldest IBI value 
      runningTotal += rate[i];               // add up the 9 oldest IBI values
    }
        
    rate[9] = IBI;                          // add the latest IBI to the rate array
    runningTotal += rate[9];                // add the latest IBI to runningTotal
    runningTotal /= 10;                     // average the last 10 IBI values 
    BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
    QS = true;                              // set Quantified Self flag 
    // NOTE: QS flag is not cleared inside the ISR
    }                       
  }

  if (Signal < thresh && Pulse == true) {  // when the values are going down, the beat is over
    if ( sampleCounter > (lastLEDtime + 100) ) { // the LED should be on at least a quarter second so you can see it
      digitalWrite(blinkPin, LOW);               // turn off pin 13 LED
    }
    Pulse = false;                         // reset the Pulse flag so we can do it again
    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }
  
  if (N > 2500) {                            // if 2.5 seconds go by without a beat
    thresh = 512;                            // set thresh default
    P = 512;                                 // set P default
    T = 512;                                 // set T default
    lastBeatTime = sampleCounter;            // bring the lastBeatTime up to date        
    firstBeat = true;                        // set these to avoid noise
    secondBeat = true;                       // when we get the heartbeat back
  }
  
  sei();                                     // enable interrupts when you're done!
  
}// end ISR



