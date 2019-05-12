/**
 *
 * - Photoduino
 * - http://code.google.com/p/photoduino/
 *
 * This file is part of Photoduino.
 *
 * Photoduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Photoduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Photoduino.  If not, see <http://www.gnu.org/licenses/>.
 */
 

// Write a unsigned int (two bytes) value to eeprom
void eeprom_writeInt(int address, unsigned int value){
    
  EEPROM.write(address, value/256);
  EEPROM.write(address+1, value % 256);

}

// read a unsigned int (two bytes) value from eeprom
unsigned int eeprom_readInt(int address){
  
  return EEPROM.read(address)*256+EEPROM.read(address+1);
}

/*
// increment the value of a variable using minimun and maximun limits
void circularList_increment(byte *value, byte minimun, byte maximun) {
  
  if (*value == maximun) *value = minimun; 
  else *value = *value+1;
}

// decrement the value of a variable using minimun and maximun limits
void circularList_decrement(byte *value, byte minimun, byte maximun) {
  
   if (*value == minimun) *value = maximun;
   else  *value = *value-1;
}
*/
// increment the value of a variable using minimun and maximun limits
void circularList_incrementBy(byte *value, byte minimun, byte maximun, byte incrementBy) {
  
  if (*value+incrementBy > maximun) *value = minimun; 
  else *value = *value+incrementBy;
}

// decrement the value of a variable using minimun and maximun limits
void circularList_decrementBy(byte *value, byte minimun, byte maximun, byte decrementBy) {
   
   if ((*value < minimun) || (*value < decrementBy)) *value = maximun;
   else *value = *value-decrementBy;
}

// increment the value of a variable using minimun and maximun limits
void circularList_incrementBy(unsigned int *value, unsigned int minimun, unsigned int maximun, byte incrementBy) {
  
  if (*value+incrementBy > maximun) *value = minimun; 
  else *value = *value+incrementBy;
}

// decrement the value of a variable using minimun and maximun limits
void circularList_decrementBy(unsigned int *value, unsigned int minimun, unsigned int maximun, byte decrementBy) {
   
   if ((*value < minimun) || (*value < decrementBy)) *value = maximun;
   else *value = *value-decrementBy;
}



