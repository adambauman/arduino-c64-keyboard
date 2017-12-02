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

#include "Common.h"

RgbLed status_led(
	PIN_RGB_RED, 
	PIN_RGB_GREEN,
	PIN_RGB_BLUE
);

namespace led_color {
  RgbColor red {255, 0, 0};
  RgbColor green {0, 255, 0};
  RgbColor blue {0, 0, 255};
  RgbColor orange {255, 147, 41};
  RgbColor yellow {255, 255, 0};
  RgbColor white {255, 255, 255};
}

CD4051 row_cd4051(
  PIN_CD4051_ROW_A0, 
  PIN_CD4051_ROW_A1, 
  PIN_CD4051_ROW_A2, 
  PIN_CD4051_ROW_COMMON
);

CD4051 column_cd4051(
  PIN_CD4051_COLUMN_A0, 
  PIN_CD4051_COLUMN_A1, 
  PIN_CD4051_COLUMN_A2, 
  PIN_CD4051_COLUMN_COMMON
);

C64KeyMap key_map_generator;
KeyMap c64_key_map = key_map_generator.GetC64KeyMap();
KeyMatrix usb_key_matrix(true);

void setup() {
  if (SYSTEM_DEBUG_ENABLED) { Serial.begin(115200); }
  //NOTE: (Adam) For dual CD4051 key matrix one chip needs to sink current from the other
  column_cd4051.SetAsMatrixSink();
  //row_cd4051.SetAsMatrixEnergize();

  pinMode(PIN_SHIFT_LOCK, INPUT_PULLUP);  
  //NOTE: (Adam) row_8 and column_i function as a simple button
  pinMode(PIN_ROW_8, INPUT_PULLUP);
  pinMode(PIN_COLUMN_I, OUTPUT);
  digitalWrite(PIN_COLUMN_I, LOW);
}

void loop() {
  unsigned long start_time = 0;
  if  ((millis() - start_time) > SYSTEM_DEBOUNCE_TIME) {
	  usb_key_matrix.ProcessKeyboardMatrix(
		  row_cd4051, 
		  column_cd4051, 
		  PIN_ROW_8,
		  PIN_SHIFT_LOCK,
		  c64_key_map,
		  SYSTEM_DEBUG_ENABLED
	  );
      start_time = millis();
  }
}