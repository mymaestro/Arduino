/* LucidTronix display audio waveforms
 * on the Heart Matrix LED Display.
 * For instructions details and schematic, See:
 * http://www.lucidtronix.com/tutorials/35
 * Connect the data, latch, and clock pins
 * to the corresponding pins on the heart matrix
 */

#include <FrequencyTimer2.h>
#include <HeartMatrix.h>

// dataPin is on 5, Latch is on 6, and clock is on 7
HeartMatrix hm = HeartMatrix(5,6,7);

const int buffer_size = 10;
int noises[buffer_size];

const int num_cols = 12;
int volumes[num_cols+1];
int cur_index = 0;
unsigned int last_shift = 0;
int mic_pin =2;

void setup() {
   FrequencyTimer2::disable();
   // Set refresh rate (interrupt timeout period)
   // if things break try making this number bigger
   FrequencyTimer2::setPeriod(2000);
    // Set interrupt routine to be called
   FrequencyTimer2::setOnOverflow(displayer2); 
   
   for (int i = 0 ; i < num_cols+1; i++) volumes[i] = 7;
   
   hm.animate();

}

void loop() {
   hm.on();   
   
   // read the value from the sensor:
   cur_index = (cur_index + 1) % buffer_size;
   noises[cur_index] = analogRead(mic_pin) ;
   int var = variance(noises, buffer_size); 
   if (millis() - last_shift > 40){
     last_shift = millis();
     for (int i = 1 ; i < num_cols+1; i++){
      volumes[i-1] = volumes[i];
     }
   }
   volumes[10] = constrain((var / 40), 0, 8);
   for (int i = 0 ; i < num_cols; i++){
     for (int j = 0 ; j < volumes[i]; j++){
       hm.set_pixel(i, j, true);  
     }
     for (int j = volumes[i] ; j < 8; j++){
       hm.set_pixel(i, j, false);  
     }
   }   
   delay(40);
}

void displayer2(){
  hm.displayer();
}

int average(int* array, int length){
  int sum = 0;
  int i;
  for(i = 0; i < length ; i++){
    sum += array[i];
  }
  int avg = sum / length ;
  return avg;
}
int variance(int* array, int length){
  int dif_sum = 0;
  int i;
  int mean = average(array, length);
  for(i = 0; i < length ; i++){
    dif_sum += (abs(array[i] - mean));
  }
  int var = dif_sum / length ;
  return var;	
}
