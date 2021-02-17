#define USBMIDIPORT 0
#define MIDI1PORT 1
#define MIDI2PORT 2
#define MIDI3PORT 3

byte outputMap = 0;  // Bitmasked map of outputs
char* outputNames[] = { "USBmidi", "MIDI1", "MIDI2", "MIDI3"};
int  notesOn = 0; // How many notes are on
char* theNoteNames[] = { "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};
  /*  bitmask (1 = send, 0 = don't send)
    USBport | MIDI1 | MIDI2 | MIDI3  
  */
int LED = 13;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  Serial.println("--------------------------");
  Serial.println("--                      --");
}

void loop() {
  digitalWrite(LED, HIGH);
  //cycleBits(outputMap);
  testBits(outputMap);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
  addABit(1);
}

void addABit(int v) {
  outputMap = outputMap + v;
  outputMap = (outputMap % 16);
}

void printTheNotes() {
  for (int i = 0; i < 12; i++ ) {
    Serial.print("note :");
    Serial.print(theNoteNames[i]);
    Serial.print(" ");
  }
  Serial.println("---");
}

void testBits(byte outputMap) { 
  Serial.print("Test bits with output map ");
  Serial.println(outputMap); 
  if (outputMap & (1 << USBMIDIPORT) ) Serial.println(outputNames[USBMIDIPORT]);
  if (outputMap & (1 << MIDI1PORT ) ) Serial.println(outputNames[MIDI1PORT]);
  if (outputMap & (1 << MIDI2PORT ) ) Serial.println(outputNames[MIDI2PORT]);
  if (outputMap & (1 << MIDI3PORT ) ) Serial.println(outputNames[MIDI3PORT]);
}

void cycleBits(byte outputMap) {
  for(int i = 0, mask = 1; i < 4; i++, mask = mask << 1) {
    Serial.print("i = ");
    Serial.print(i);
    Serial.print(" mask: ");
    Serial.println(mask);
    if (outputMap & mask) 
    {
      Serial.print(outputNames[i]);
      Serial.println(" is on.");
     // printTheNotes();
        // bit is on
    } else {
      Serial.print(outputNames[i]);
      Serial.println(" is off.");
        // bit is off
    }
}

}
