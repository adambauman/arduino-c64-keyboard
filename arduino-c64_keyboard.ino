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
#include "KeyMaps.h"
#include "Configuration.h"

// Size of the keyboard matrix. C64 is technically 9x9 but the RESTORE key lives on its own and is handled seperatly
struct {
  const byte rows = 8;
  const byte columns = 8;
} g_key_matrix;

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

void setup() {
  if (SystemOptions::debugEnabled == true)
    Serial.begin(115200);

  // All key reading pins use the internal pullup resistors
  pinMode(PIN_SHIFT_LOCK, INPUT_PULLUP);

  pinMode(PIN_COL_I, OUTPUT);
  pinMode(PIN_ROW_8, INPUT_PULLUP);
  
  pinMode(PIN_COL_Z, OUTPUT);
  pinMode(PIN_COL_A0, OUTPUT);
  pinMode(PIN_COL_A1, OUTPUT);
  pinMode(PIN_COL_A2, OUTPUT);
  
  pinMode(PIN_ROW_Z, INPUT_PULLUP); 
  pinMode(PIN_ROW_A0, OUTPUT);
  pinMode(PIN_ROW_A1, OUTPUT);
  pinMode(PIN_ROW_A2, OUTPUT);

  // Row drops low when button closed to column. Drop columns LOW so they're ready.
  digitalWrite(PIN_COL_Z, LOW);
  digitalWrite(PIN_COL_I, LOW);

  Keyboard.begin(); 
  
}
void loop() {
  unsigned long start_time = 0;
  if  ((millis() - start_time) > SystemOptions::debounce_time) {
      ScanKeys();
      start_time = millis();
  }

  g_key_states.last_state_restore = g_key_states.state_restore;
  for (byte column = 0; column < g_key_matrix.columns; column++) {  
    for (byte row = 0; row < g_key_matrix.rows; row++) {
      g_key_states.last_state_map[row][column] = g_key_states.state_map[row][column];
    }
  }
  
  if (g_key_states.keys_have_changed) { WriteKeys(); }
  if (SystemOptions::debugEnabled && g_key_states.keys_have_changed) { DebugKeys(); } 
  g_key_states.keys_have_changed = false;
}

void ColumnCD4051Select(int column) {
  digitalWrite(Pins::CD4051::Column::a0, bitRead(column, 0));
  digitalWrite(Pins::CD4051::Column::a1, bitRead(column, 1));
  digitalWrite(Pins::CD4051::Column::a2, bitRead(column, 2));
}

void RowCD4015Select(int row) {
  digitalWrite(Pins::CD4051::Row::a0, bitRead(row, 0));
  digitalWrite(Pins::CD4051::Row::a1, bitRead(row, 1));
  digitalWrite(Pins::CD4051::Row::a2, bitRead(row, 2));
}

void ScanKeys() {
    // Make sure columns are running LOW
    //digitalWrite(pin_colI, LOW);
    //digitalWrite(pin_colZ, LOW);
    for (byte column = 0; column < g_key_matrix.columns; column++) {  
      ColumnCD4051Select(column);
      for (byte row = 0; row < g_key_matrix.rows; row++) {
        RowCD4015Select(row);

        if (LOW == digitalRead(Pins::CD4051::Row::common_io)) { 
          g_key_states.state_map[row][column] = true;
        } else {
          g_key_states.state_map[row][column] = false;
        }
        
        if (g_key_states.last_state_map[row][column] != g_key_states.state_map[row][column]) { g_key_states.keys_have_changed = true; }
      }
    }

    // Read RESTORE status
    if (LOW == digitalRead(Pins::row_8)) { 
      g_key_states.state_restore = true; 
    } else {
      g_key_states.state_restore = false;
    }

    // Read SHIFTLOCK status
    if (LOW == digitalRead(Pins::shift_lock)) {
      g_key_states.state_shift_lock = true;
    } else {
      g_key_states.state_shift_lock = false;
    }

    if (g_key_states.last_state_restore != g_key_states.state_restore || g_key_states.last_state_shift_lock != g_key_states.state_shift_lock) { 
      g_key_states.keys_have_changed = true;
    }
}

void WriteKeys()
{
  for (byte column = 0; column < g_key_matrix.columns; column++){
    for (byte row = 0; row < g_key_matrix.rows; row++) {
      if (g_key_states.state_map[row][column] && !g_key_states.last_state_map[row][column]) {
        //TODO: figure out why row and column need to be reversed here
        Keyboard.press(key_maps.unmodified[column][row]); 
      } else if (!g_key_states.state_map[row][column] && g_key_states.last_state_map[row][column]) {
        Keyboard.release(key_maps.unmodified[column][row]);
      }
    }
  }

  if (g_key_states.state_restore && !g_key_states.last_state_restore) {
    Keyboard.press(key_maps.restore_key);
  } else if (!g_key_states.state_restore && g_key_states.last_state_restore) {
    Keyboard.release(key_maps.restore_key);
  }

  // Process the SHIFTLOCK key, use .write() since the key physically locks in place
  if (g_key_states.state_shift_lock && !g_key_states.last_state_map) {
    Keyboard.write(193);
  } else if (!g_key_states.state_shift_lock && g_key_states.last_state_shift_lock) {
    Keyboard.write(193);
  }
}

void SetLED(int requested_status) {
  // Only run if RGB LED is enabled in the setup parameters
  if (SystemOptions::RGBenabled) {
    switch (requested_status) {
      case 0: // Normal (red)
        analogWrite(Pins::RGB::red, 255);
        analogWrite(Pins::RGB::green, 0);
        analogWrite(Pins::RGB::blue, 0);
        break;
      case 1: // Capslock Enabled (blue)
        analogWrite(Pins::RGB::red, 0);
        analogWrite(Pins::RGB::green, 0);
        analogWrite(Pins::RGB::blue, 255);
        break;
      case 2: // Battery Good (green)
        analogWrite(Pins::RGB::red, 0);
        analogWrite(Pins::RGB::green, 255);
        analogWrite(Pins::RGB::blue, 0);
        break;
      case 3: // Battery Warning (yellow)
        analogWrite(Pins::RGB::red, 255);
        analogWrite(Pins::RGB::green, 255);
        analogWrite(Pins::RGB::blue, 0);
        break;
      default:
        break;
    }
  }
}

void DebugKeys() {
  int should_send = 0;
  
  Serial.println("");
  for (byte column = 0; column < g_key_matrix.columns; column++) {  
    for (byte row = 0; row < g_key_matrix.rows; row++) {
      Serial.print(g_key_states.last_state_map[row][column]);
      Serial.print(", ");

      if (g_key_states.last_state_map[row][column])
        should_send = key_maps.unmodified[row][column]; 
    }
    Serial.println("");
  }

  Serial.println("");
  Serial.println("KeymapUnmodified: ");

  for (byte column = 0; column < g_key_matrix.columns; column++) {
    for (byte row = 0; row < g_key_matrix.rows; row++) {
      Serial.print(key_maps.unmodified[row][column]);
      Serial.print(", ");
    }
    Serial.println("");
  }
  
  Serial.print("Should send: ");
  Serial.println(should_send);
  Serial.println("");
}
