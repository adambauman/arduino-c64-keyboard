// BauTek C64 PiCase Keyboard Controller
// Version 0.4, August 2016
// Written by Adam Bauman (adam@kungfutreachery.net, https://bitbucket.org/adambauman/arduino-c64_keyboard)
//
// Utilizes an ATmega32U4-based microcontroller and two CD4051 muxers to run a Commodore64
// keyboard as a USBHID device. Optionally you can also run a RGB status LED and alternate keymaps.
//
// See included wiring diagrams for hardware setup. The C64 keyboard requires one minor modification, you must
// de-solder the leads on the SHIFTLOCK key and run one to the microcontroller ground, the other to pin 9.
//
//
// Copyright (C) 2016, Adam J. Bauman
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

#include <Keyboard.h>
#include "C64KeyMaps.h"
#include "Configuration.h"
#include "RgbLed.h"
#include "CD4051.h"

// Size of the keyboard matrix. C64 is technically 9x9 but the RESTORE key lives on its own and is handled seperatly
struct {
  const byte rows = 8;
  const byte columns = 8;
} g_key_matrix_size;

struct {
  // Set if matrix is changed, stops loop from writing unnecessary key commands
  boolean keys_have_changed = false;
  // Tracks state of modifier keys (currently unused, running custom OS keymaps instead)
  boolean state_shift = false;
  boolean state_alt = false;
  boolean state_ctrl = false;
  boolean state_caps_lock = false;
  // Tracks RESTORE and SHIFTLOCK parameters
  boolean last_state_restore = false;
  boolean last_state_shift_lock = false;
  boolean state_restore = false;
  boolean state_shift_lock = false;

  boolean state_map[8][8] = {
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false}
  };

  boolean last_state_map[8][8] = {
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false}
  };
} g_key_states;

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

void setup() {
  if (SystemOptions::debugEnabled) { Serial.begin(115200); }
  Keyboard.begin();
  //NOTE: (Adam) For dual CD4051 key matrix one chip needs to sink current from the other
  column_cd4051.SetAsMatrixSink();

  pinMode(Pins::shift_lock, INPUT_PULLUP);  
  //NOTE: (Adam) row_8 and column_i function on their own "matrix"
  pinMode(Pins::row_8, INPUT_PULLUP);
  pinMode(Pins::column_i, OUTPUT);
  digitalWrite(Pins::column_i, LOW);
}

void loop() {
  unsigned long start_time = 0;
  if  ((millis() - start_time) > SystemOptions::debounce_time) {
      ScanKeys();
      start_time = millis();
  }

  g_key_states.last_state_restore = g_key_states.state_restore;
  for (byte column = 0; g_key_matrix_size.columns < column; column++) {  
    for (byte row = 0; g_key_matrix_size.rows > row; row++) {
      g_key_states.last_state_map[row][column] = g_key_states.state_map[row][column];
    }
  }
  
  if (g_key_states.keys_have_changed) { WriteKeys(); }
  if (SystemOptions::debugEnabled && g_key_states.keys_have_changed) { DebugKeys(); } 
  g_key_states.keys_have_changed = false;
}

void ScanKeys() {
    // Make sure columns are running LOW
    //digitalWrite(pin_colI, LOW);
    //digitalWrite(pin_colZ, LOW);
    g_key_states.keys_have_changed = false;
    for (byte column = 0; g_key_matrix_size.columns > column; column++) {  
      column_cd4051.Select(column);
      for (byte row = 0; g_key_matrix_size.rows > row; row++) {
        row_cd4051.Select(row);
        g_key_states.state_map[row][column] = !row_cd4051.ReadCommonValue(); // true on LOW

        if (g_key_states.last_state_map[row][column] != g_key_states.state_map[row][column]) { g_key_states.keys_have_changed = true; }
      }
    }

    g_key_states.state_restore = !digitalRead(Pins::row_8); // true on LOW
    g_key_states.state_shift_lock = !digitalRead(Pins::shift_lock); // true on LOW
    // if (LOW == digitalRead(Pins::row_8)) { 
    //   g_key_states.state_restore = true; 
    // } else {
    //   g_key_states.state_restore = false;
    // }
    // Read SHIFTLOCK status
    // if (LOW == digitalRead(Pins::shift_lock)) {
    //   g_key_states.state_shift_lock = true;
    // } else {
    //   g_key_states.state_shift_lock = false;
    // }
    if (g_key_states.last_state_restore != g_key_states.state_restore) { g_key_states.keys_have_changed = true; }
    if (g_key_states.last_state_shift_lock != g_key_states.state_shift_lock) { g_key_states.keys_have_changed = true; }

}

bool IsShiftActive(){
  bool shift_pressed = false;
  bool left_shift_pressed = g_key_states.state_map[C64KeyMaps::left_shift_position_row][C64KeyMaps::left_shift_position_column];
  bool right_shift_pressed = g_key_states.state_map[C64KeyMaps::right_shift_position_row][C64KeyMaps::right_shift_position_column];
  if (left_shift_pressed || right_shift_pressed) { shift_pressed = true; }
  return(shift_pressed);
}

void KeyPressAction(int row, int column)
{
  if (IsShiftActive()) {
    //TODO: figure out why row and column need to be reversed here
    Keyboard.press(C64KeyMaps::shifted[column][row]); 
  } else {
    Keyboard.press(C64KeyMaps::unmodified[column][row]); 
  }
}

void KeyReleaseAction(int row, int column)
{
  if (IsShiftActive()) {
    //TODO: figure out why row and column need to be reversed here
    Keyboard.release(C64KeyMaps::shifted[column][row]); 
  } else {
    Keyboard.release(C64KeyMaps::unmodified[column][row]); 
  }
}

void WriteKeys()
{
  for (byte column = 0; g_key_matrix_size.columns > column; column++){
    for (byte row = 0; g_key_matrix_size.rows > row; row++) {
      if (g_key_states.state_map[row][column] && !g_key_states.last_state_map[row][column]) {
        KeyPressAction(row, column);
      } else if (!g_key_states.state_map[row][column] && g_key_states.last_state_map[row][column]) {
        KeyReleaseAction(row, column);
      }
    }
  }

  if (g_key_states.state_restore && !g_key_states.last_state_restore) {
    Keyboard.press(C64KeyMaps::restore_key);
  } else if (!g_key_states.state_restore && g_key_states.last_state_restore) {
    Keyboard.release(C64KeyMaps::restore_key);
  }

  // Process the SHIFTLOCK key, use .write() since the key physically locks in place
  if (g_key_states.state_shift_lock && !g_key_states.last_state_map) {
    Keyboard.write(KEYDEC_CLOK);
  } else if (!g_key_states.state_shift_lock && g_key_states.last_state_shift_lock) {
    Keyboard.write(KEYDEC_CLOK);
  }
}

void DebugKeys() {
  int selected_key = 0;
  
  Serial.println("");
  for (byte column = 0; g_key_matrix_size.columns > column; column++) {  
    for (byte row = 0; g_key_matrix_size.rows > row; row++) {
      Serial.print(g_key_states.last_state_map[row][column]);
      Serial.print(", ");

      if (g_key_states.last_state_map[row][column])
        selected_key = C64KeyMaps::unmodified[row][column]; 
    }
    Serial.println("");
  }

  Serial.println("");
  Serial.println("KeymapUnmodified: ");

  for (byte column = 0; g_key_matrix_size.columns > column; column++) {
    for (byte row = 0; g_key_matrix_size.rows > row; row++) {
      Serial.print(C64KeyMaps::unmodified[row][column]);
      Serial.print(", ");
    }
    Serial.println("");
  }
  
  Serial.print("Should send: ");
  Serial.println(selected_key);
  Serial.println("");
}
