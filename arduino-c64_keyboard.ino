// BauTek C64 PiCase Keyboard Controller
// Version 0.5, August 2017
// Written by Adam Bauman (adam@kungfutreachery.net, https://bitbucket.org/adambauman/arduino-c64_keyboard)
//
// Utilizes an ATmega32U4-based microcontroller and two CD4051 multiplexers to run a Commodore64
// keyboard as a USBHID device. Optionally you can also run a RGB status LED and alternate keymaps.
//
// See included wiring diagrams for hardware setup. The C64 keyboard requires one minor modification, you must
// de-solder the leads on the SHIFTLOCK key and run one to the microcontroller ground, the other to pin 9.
//
//
// Copyright (C) 2017, Adam J. Bauman
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "C64KeyMaps.h"
#include "Configuration.h"
#include "RgbLed.h"
#include "CD4051.h"
#include "KeyMatrix.h"

RgbLed status_led(Pins::RGB::red, Pins::RGB::green, Pins::RGB::blue);
namespace led_color {
  RgbColor red {255, 0, 0};
  RgbColor green {0, 255, 0};
  RgbColor blue {0, 0, 255};
  RgbColor orange {255, 147, 41};
  RgbColor yellow {255, 255, 0};
  RgbColor white {255, 255, 255};
}

CD4051 row_cd4051(
  Pins::CD4051::Row::a0, 
  Pins::CD4051::Row::a1, 
  Pins::CD4051::Row::a2, 
  Pins::CD4051::Row::common_io
);

CD4051 column_cd4051(
  Pins::CD4051::Column::a0, 
  Pins::CD4051::Column::a1, 
  Pins::CD4051::Column::a2, 
  Pins::CD4051::Column::common_io
);

KeyMaps c64_key_maps;
KeyMatrix usb_key_matrix(true);

void setup() {
  if (SystemOptions::debugEnabled) { Serial.begin(115200); }
  //NOTE: (Adam) For dual CD4051 key matrix one chip needs to sink current from the other
  column_cd4051.SetAsMatrixSink();
  //row_cd4051.SetAsMatrixEnergize();

  pinMode(Pins::shift_lock, INPUT_PULLUP);  
  //NOTE: (Adam) row_8 and column_i function as a simple button
  pinMode(Pins::row_8, INPUT_PULLUP);
  pinMode(Pins::column_i, OUTPUT);
  digitalWrite(Pins::column_i, LOW);
}

void loop() {
  unsigned long start_time = 0;
  if  ((millis() - start_time) > SystemOptions::debounce_time) {
	  usb_key_matrix.ProcessKeyboardMatrix(
		  row_cd4051, 
		  column_cd4051, 
		  Pins::row_8,
		  Pins::shift_lock,
		  c64_key_maps,
		  SystemOptions::debugEnabled
	  );
      start_time = millis();
  }
}