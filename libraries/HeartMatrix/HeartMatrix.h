/* LucidTronix write messages on the Heart Matrix.
 * For instructions details and schematic, See:
 * http://www.lucidtronix.com/tutorials/18
 */

#ifndef HeartMatrix_h
#define HeartMatrix_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class HeartMatrix
{
  public:
    HeartMatrix(int dataPin, int latchPin, int clockPin);
    void  set_message(String text);
    void  set_scroll_wait(int wait);
	void  no_scroll();
	void  animate();
	void  animate_demo();
    void  on();
    void  shift_3_bytes();
    void  write_character(int index);
    void  write_byte(int col, unsigned char c);
    void  set_row(int r, int val);
    void  all_low();
    void  all_high();
    void  all_on();
    void  all_opposite();
    void  set_column(int c, int val);
	void  set_pixel(int x, int y, bool state);
    void  displayer();
	String get_user_message();
	void  select_letters();
	void  select_letter(int l);
	void  add_letter();
    void  shiftOut(int myDataPin, int myClockPin, byte myDataOut);
    void  set_user_message(String s);
  private:
	void text_update();
	void user_text_update();
    int latchPin,dataPin,clockPin  ; 
    int cur_col,cur_index,col_shift, user_cur_index ;
    int char_index, char_index_2, char_index_3, user_char_index ;
    //holders for infromation you're going to pass to shifting function
    byte data1, data2, data3;
    String message;
	String user_message;
    unsigned int length;
	unsigned int user_length;
    int cur_wait;
	int cur_mode; // the modes are 0 for scrolling text 1 for selecting letters and 2 for animating
	int selected_index;

};

#endif




