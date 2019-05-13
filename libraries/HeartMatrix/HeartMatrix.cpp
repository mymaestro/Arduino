/* LucidTronix write messages on the Heart Matrix.
 * For instructions details and schematic, See:
 * http://www.lucidtronix.com/tutorials/18
 */
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "HeartMatrix.h"

// defines 5x7 ascii characters 0x20-0x7F (32-127)

byte dot_matrix[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

int font_5x7[96][5] = {
        {0x00,0x00,0x00,0x00,0x00}, //
        {0x00,0x00,0xfa,0x00,0x00}, // !
        {0x00,0xe0,0x00,0xe0,0x00}, // "
        {0x28,0xfe,0x28,0xfe,0x28}, // #
        {0x24,0x54,0xfe,0x54,0x48}, // $
        {0xc4,0xc8,0x10,0x26,0x46}, // %
        {0x6c,0x92,0xaa,0x44,0x0a}, // &
        {0x00,0xa0,0xc0,0x00,0x00}, // '
        {0x00,0x38,0x44,0x82,0x00}, // (
        {0x00,0x82,0x44,0x38,0x00}, // )
        {0x10,0x54,0x38,0x54,0x10}, // *
        {0x10,0x10,0x7c,0x10,0x10}, // +
        {0x00,0x0a,0x0c,0x00,0x00}, // ,
        {0x10,0x10,0x10,0x10,0x10}, // -
        {0x00,0x06,0x06,0x00,0x00}, // .
        {0x04,0x08,0x10,0x20,0x40}, // /
        {0x7c,0x8a,0x92,0xa2,0x7c}, // 0
        {0x00,0x42,0xfe,0x02,0x00}, // 1
        {0x42,0x86,0x8a,0x92,0x62}, // 2
        {0x84,0x82,0xa2,0xd2,0x8c}, // 3
        {0x18,0x28,0x48,0xfe,0x08}, // 4
        {0xe4,0xa2,0xa2,0xa2,0x9c}, // 5
        {0x3c,0x52,0x92,0x92,0x0c}, // 6
        {0x80,0x8e,0x90,0xa0,0xc0}, // 7
        {0x6c,0x92,0x92,0x92,0x6c}, // 8
        {0x60,0x92,0x92,0x94,0x78}, // 9
        {0x00,0x6c,0x6c,0x00,0x00}, // :
        {0x00,0x6a,0x6c,0x00,0x00}, // ;
        {0x00,0x10,0x28,0x44,0x82}, // <
        {0x28,0x28,0x28,0x28,0x28}, // =
        {0x82,0x44,0x28,0x10,0x00}, // >
        {0x40,0x80,0x8a,0x90,0x60}, // ?
        {0x4c,0x92,0x9e,0x82,0x7c}, // @
        {0x7e,0x88,0x88,0x88,0x7e}, // A
        {0xfe,0x92,0x92,0x92,0x6c}, // B
        {0x7c,0x82,0x82,0x82,0x44}, // C
        {0xfe,0x82,0x82,0x44,0x38}, // D
        {0xfe,0x92,0x92,0x92,0x82}, // E
        {0xfe,0x90,0x90,0x80,0x80}, // F
        {0x7c,0x82,0x82,0x8a,0x4c}, // G
        {0xfe,0x10,0x10,0x10,0xfe}, // H
        {0x00,0x82,0xfe,0x82,0x00}, // I
        {0x04,0x02,0x82,0xfc,0x80}, // J
        {0xfe,0x10,0x28,0x44,0x82}, // K
        {0xfe,0x02,0x02,0x02,0x02}, // L
        {0xfe,0x40,0x20,0x40,0xfe}, // M
        {0xfe,0x20,0x10,0x08,0xfe}, // N
        {0x7c,0x82,0x82,0x82,0x7c}, // O
        {0xfe,0x90,0x90,0x90,0x60}, // P
        {0x7c,0x82,0x8a,0x84,0x7a}, // Q
        {0xfe,0x90,0x98,0x94,0x62}, // R
        {0x62,0x92,0x92,0x92,0x8c}, // S
        {0x80,0x80,0xfe,0x80,0x80}, // T
        {0xfc,0x02,0x02,0x02,0xfc}, // U
        {0xf8,0x04,0x02,0x04,0xf8}, // V
        {0xfe,0x04,0x18,0x04,0xfe}, // W
        {0xc6,0x28,0x10,0x28,0xc6}, // X
        {0xc0,0x20,0x1e,0x20,0xc0}, // Y
        {0x86,0x8a,0x92,0xa2,0xc2}, // Z
        {0x00,0x00,0xfe,0x82,0x82}, // [
        {0x40,0x20,0x10,0x08,0x04}, // "\"
        {0x82,0x82,0xfe,0x00,0x00}, // ]
        {0x20,0x40,0x80,0x40,0x20}, // ^
        {0x02,0x02,0x02,0x02,0x02}, // _
        {0x00,0x80,0x40,0x20,0x00}, // `
        {0x04,0x2a,0x2a,0x2a,0x1e}, // a
        {0xfe,0x12,0x22,0x22,0x1c}, // b
        {0x1c,0x22,0x22,0x22,0x04}, // c
        {0x1c,0x22,0x22,0x12,0xfe}, // d
        {0x1c,0x2a,0x2a,0x2a,0x18}, // e
        {0x10,0x7e,0x90,0x80,0x40}, // f
        {0x10,0x28,0x2a,0x2a,0x3c}, // g
        {0xfe,0x10,0x20,0x20,0x1e}, // h
        {0x00,0x22,0xbe,0x02,0x00}, // i
        {0x04,0x02,0x22,0xbc,0x00}, // j
        {0x00,0xfe,0x08,0x14,0x22}, // k
        {0x00,0x82,0xfe,0x02,0x00}, // l
        {0x3e,0x20,0x18,0x20,0x1e}, // m
        {0x3e,0x10,0x20,0x20,0x1e}, // n
        {0x1c,0x22,0x22,0x22,0x1c}, // o
        {0x3e,0x28,0x28,0x28,0x10}, // p
        {0x10,0x28,0x28,0x18,0x3e}, // q
        {0x3e,0x10,0x20,0x20,0x10}, // r
        {0x12,0x2a,0x2a,0x2a,0x04}, // s
        {0x20,0xfc,0x22,0x02,0x04}, // t
        {0x3c,0x02,0x02,0x04,0x3e}, // u
        {0x38,0x04,0x02,0x04,0x38}, // v
        {0x3c,0x02,0x0c,0x02,0x3c}, // w
        {0x22,0x14,0x08,0x14,0x22}, // x
        {0x30,0x0a,0x0a,0x0a,0x3c}, // y
        {0x22,0x26,0x2a,0x32,0x22}, // z
        {0x00,0x10,0x6c,0x82,0x00}, // {
        {0x00,0x00,0xfe,0x00,0x00}, // |
        {0x00,0x82,0x6c,0x10,0x00}, // }
        {0x40,0x80,0xc0,0x40,0x80}, // ~
        {0x00,0x00,0x00,0x00,0x00}, //
    };
HeartMatrix :: HeartMatrix(int _dataPin , int _latchPin, int _clockPin){
    dataPin = _dataPin;
    latchPin = _latchPin;   
    clockPin = _clockPin;
    
    pinMode(dataPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
    cur_col = 0;
    col_shift = 10;
    //holders for information you're going to pass to shifting function
    data1 =  0xFF;
    data2 = ~0x03;
    data3 =  0xFF;
    cur_index = 0;
    message = String("LucidTronix  ");
	user_message = String("");
    length = 11;
	user_length = 0;
    char_index = message[cur_index]-32;
    char_index_2 = message[cur_index+1]-32;
    char_index_3 = message[cur_index+2]-32;
	user_char_index = 0;
	user_cur_index = 0;
    cur_wait = 100;
	cur_mode = 0;
	selected_index = 35;
}

void HeartMatrix :: set_message(String text){
  cur_mode = 0;	
  message = text;
  length = text.length()-1;
}

void HeartMatrix :: set_scroll_wait(int wait){
  cur_mode = 0;
  cur_wait = wait;
}
void HeartMatrix :: animate(){
	cur_mode = 2;
}
String HeartMatrix :: get_user_message(){
	return user_message;
}
void HeartMatrix :: set_user_message(String s){
	user_message = s;
	user_length = s.length();
}
void HeartMatrix :: select_letters(){
	cur_mode = 1;
}
void HeartMatrix :: select_letter(int l){
	selected_index = l;
}
void HeartMatrix :: add_letter(){
	user_message += (char)(selected_index+32);
	user_length++;
}
void HeartMatrix :: on(){
	if (cur_mode == 0 ) text_update();
	else if ( cur_mode == 1) user_text_update();
}
void HeartMatrix :: text_update(){
	col_shift--;
	if (col_shift == -5){
		cur_index++;
		col_shift = 1; 
	}
	if ( cur_index == length ) {
		cur_index = 0;
		col_shift = 10;
	}
	char_index = message[cur_index]-32;
	char_index_2 = message[cur_index+1]-32;
	char_index_3 = message[cur_index+2]-32;
	delay(cur_wait);	
}
void HeartMatrix :: user_text_update(){
	col_shift--;
	if (col_shift == -5){
	  user_cur_index++;
      col_shift = 1; 
	}
    if ( user_cur_index == user_length ) {
       user_cur_index = 0;
       col_shift = 5;
    }
	user_char_index = user_message[user_cur_index]-32;
    delay(cur_wait);	
}								   
void HeartMatrix :: set_pixel(int x, int y, bool state){
	cur_mode = 2;
	if (state) dot_matrix[x] |= ( 1 << y );
	else dot_matrix[x] &= ~( 1 << y );
}
void HeartMatrix :: animate_demo(){
	for (int i = 0; i < 10; i++){
		dot_matrix[i] = ~dot_matrix[i];
	}
	delay(cur_wait);
	for (int i = 0; i < 10; i++){
		dot_matrix[i] = 0x00;
		delay(cur_wait);
	}
	for (int i = 0; i < 10; i++){
		dot_matrix[i] = 0xFF;
		delay(cur_wait);
	}

	for (int i = 0; i < 10; i++){
		dot_matrix[i] = 0xAA;
		delay(cur_wait);
	}
	for (int i = 0; i < 10; i++){
		dot_matrix[i] = 0x55;
		delay(cur_wait);
	}
	for (int i = 0; i < 10; i++){
		dot_matrix[i] = 1<<(i%8);
		delay(cur_wait);
	}
	for (int i = 0; i < 10; i++){
		dot_matrix[i] = 0x80>>(i%8);
		delay(cur_wait);
	}
	for (int i = 0; i < 5; i++){
		for (int i = 0; i < 10; i++){
			if (i%2 == 0) dot_matrix[i] = 0xAA;
			else dot_matrix[i] = 0x55;
		}
		delay(cur_wait);
		for (int i = 0; i < 10; i++){
			if (i%2 == 0) dot_matrix[i] = 0x55;
			else dot_matrix[i] = 0xAA;
			
		}
		delay(cur_wait);
	}
	for (int i = 0; i < 10; i++){
		if (i%2 == 0) dot_matrix[i] = 0xAA;
		else dot_matrix[i] = 0x55;
		delay(cur_wait);
	}
	for (int i = 0; i < 10; i++){
		if (i%2 == 0) dot_matrix[i] = 0x55;
		else dot_matrix[i] = 0xAA;
		delay(cur_wait);
	}
	for (int i = 0; i < 10; i++){
		dot_matrix[i] = i*25;
		delay(cur_wait);
	}
	for (int i = 0; i < 10; i++){
		dot_matrix[i] = random(0,255);
		delay(cur_wait);
	}
	
}
void HeartMatrix :: write_byte(int col, unsigned char c){
  set_column(col, 1);
  for (int i = 1; i < 8; i++){
    if (col > 4){
      if ((c >> i) & 0x01 == 0x01) set_row(14-i,1);
      else set_row(14-i, 0);
    }else{
      if ((c >> i) & 0x01 == 0x01) set_row(7-i,1);
      else set_row(7-i, 0);
    }
  }  
}
void HeartMatrix :: set_row(int r, int val){
 if ( r > 5 ){
  r -= 6;
  if (val == 1 )  HeartMatrix ::data3 |= (1 << r);
  else HeartMatrix :: data3 &= ~(1 << r);
 } else {
  r+= 2;
  if (val == 1 ) data2 |= (1 << r);
  else data2 &= ~(1 << r);
 }
}
void HeartMatrix :: all_low(){
 data1 = 0x00; 
 data2 = 0x00;
 data3 = 0x00; 
}
void HeartMatrix :: all_high(){
 data3 = 0xFF; 
 data2 = 0xFF; 
 data1 = 0xFF; 
}
void HeartMatrix :: all_opposite(){
 data3 = 0x00; 
 data2 = 0x03; 
 data1 = 0xFF; 
}
void HeartMatrix :: all_on(){
 all_opposite();
 data1 = !data1; 
 data2 = !data2; 
 data3 = !data3; 
}
void HeartMatrix :: set_column(int c, int val){
 if ( c > 7 ){
  c -= 8;
  if (val == 0 ) data2 |= (1 << c);
  else data2 &= ~(1 << c);
 } else {
  if (val == 0 ) data1 |= (1 << c);
  else data1 &= ~(1 << c);
 }
}
// Interrupt routine
void HeartMatrix :: displayer() {
	all_opposite();
	if(cur_mode == 2){
		cur_col++;
		if (cur_col >= 10) {
			cur_col = 0;
		}
		write_byte(cur_col, dot_matrix[cur_col]);
	} else if (cur_mode == 0 ){			
	  cur_col++;
	  if (cur_col >= 5) {
		cur_col = 0;
	  }
	  if(cur_col+col_shift < 10 && cur_col+col_shift >-1) write_byte(cur_col+col_shift, font_5x7[char_index][cur_col]);
	  if(cur_col+6+col_shift < 10) write_byte(cur_col+6+col_shift, font_5x7[char_index_2][cur_col]);
	  if(cur_col+12+col_shift < 10) write_byte(cur_col+12+col_shift, font_5x7[char_index_3][cur_col]);
	} else if (cur_mode == 1){
	  cur_col++;
	  if (cur_col >= 5) {
		  cur_col = 0;
	  }	   
	  write_byte(cur_col, font_5x7[selected_index][cur_col]);
      if(cur_col+6+col_shift > 4 && cur_col+6+col_shift < 10  && user_length > 1)
		  write_byte(cur_col+6+col_shift, font_5x7[user_char_index][cur_col]);
	  else if (user_length <2) write_byte(cur_col+5, font_5x7[user_char_index][cur_col]);
	}
	shift_3_bytes();
  
}

void HeartMatrix :: shift_3_bytes(){
  //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, 0);
    //move 'em out  

    shiftOut(dataPin, clockPin, data3);  
    shiftOut(dataPin, clockPin, data2);   
    shiftOut(dataPin, clockPin, data1);
    //return the latch pin high to signal chip that it 
    //no longer needs to listen for information
    digitalWrite(latchPin, 1);
}
// the heart of the program
void HeartMatrix :: shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

	
  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {	
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}
