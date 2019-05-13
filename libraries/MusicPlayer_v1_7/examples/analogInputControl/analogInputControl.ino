//  File MusicPlayer.h
//
//  For more details about the product please check http://www.seeedstudio.com/depot/
//  Copyright (c) 2012 seeed technology inc.
//  Author: Frankie.Chu
//  Version: 1.7
//  Time: June 10, 2012
//  Changing records:
//    
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#include <Fat16.h>
#include <Fat16util.h>
#include <NewSPI.h>
#include <Arduino.h>
#include "pins_config.h"
#include "vs10xx.h"
#include "newSDlib.h"
#include "MusicPlayer.h"
MusicPlayer myplayer;
void setup()
{
  Serial.begin(9600);
  myplayer.keyDisable();//keys disable first;
  myplayer.analogControlEnable();//enable to scan the A4/A5 
  myplayer.begin();//will initialize the hardware and set default mode to be normal.
  myplayer.attachAnalogOperation(A4,adjustVolume);//Grove - Rotary Angle Sensor connected to A4,to control the volume
}
void loop()
{
  myplayer.creatPlaylist();//If the current playlist is empty,it will add all the songs in the root directory to the playlist.
                           //Otherwise it will add the current song to the new playlist.
  myplayer.playList();
  while(1);//If the mode is normal, it will stop when it finished playing all the songs in the playlist
}
void adjustVolume()//User-defined function
{
  unsigned int vol_temp = analogRead(A4);
  unsigned char volume = vol_temp/12;
  if(volume == 0x55)volume = SILENT;//SILENT = 0xfe;
  myplayer.setVolume(volume);
}
