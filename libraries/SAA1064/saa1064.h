/*
 * saa1064.h
 * 
 * Copyright 2012 Warren Gill <mymaestro@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * Thanks to John Boxall at tronixstuff
 * NXP SAA1064 I2C LED Driver IC Demo
 * http://tronixstuff.wordpress.com/tutorials
 * John Boxall July 2011 | CC by-sa-nc
 */

#ifndef _SAA1064_H__
#define _SAA1064_H__
#define byte uint8_t

class SAA1064 {
  public:
    void begin(uint8_t ioaddr, int brightness);
    void begin(uint8_t ioaddr);
    void begin(void);

	void setBrightness(int brightness);
    void clear();
    void displayInt(int num, int zero);

  private:
    static int _digits[];
    static uint8_t _brightness[];
    uint8_t _addr;
};

#endif
