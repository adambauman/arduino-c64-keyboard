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
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <Arduino.h>
#include "Configuration.h"
#include "CD4051.h"
#include "KeyMatrix.h"

// Size of the keyboard matrix. C64 is technically 9x9 but the RESTORE key lives on its own and is handled seperatly
const uint8_t row_count = 8;
const uint8_t column_count = 8;

// Debounce setup, on an ATmega32U4 @ 16MHz 10ms works pretty well
unsigned long startTime = 0;
unsigned int debounceTime = 10;
  
CD4051 cd4051_column(PIN_CD4051_COLUMN_A0, PIN_CD4051_COLUMN_A1, PIN_CD4051_COLUMN_A2, PIN_CD4051_COLUMN_COMMON);
CD4051 cd4051_row(PIN_CD4051_ROW_A0, PIN_CD4051_ROW_A1, PIN_CD4051_ROW_A2, PIN_CD4051_ROW_COMMON);
KeyMatrix key_matrix;

void setup() {
  if (SYSTEM_DEBUG_ENABLED)
    Serial.begin(115200);

  cd4051_column.SetAsOutput();
  cd4051_row.SetAsInputPullup();

  // All key reading pins use the internal pullup resistors
  pinMode(PIN_SHIFT_LOCK, INPUT_PULLUP);
  pinMode(PIN_COLUMN_I, OUTPUT);
  pinMode(PIN_ROW_8, INPUT_PULLUP);

  // Row drops low when button closed to column. Drop columns LOW so they're ready.
  digitalWrite(PIN_CD4051_COLUMN_COMMON, LOW);
  digitalWrite(PIN_COLUMN_I, LOW);

  key_matrix.StartKeyboard();
}


void loop() {
  if  ((millis() - startTime) > debounceTime) {
      key_matrix.ProcessKeyMatrix(cd4051_row, cd4051_column);
      startTime = millis();
  } 
}


void SetLED(int requestedStatus) {
  if (SYSTEM_RGB_ENABLED) {
    switch (requestedStatus) {
      case 0: // Normal (red)
        analogWrite(PIN_RGB_RED, 255);
        analogWrite(PIN_RGB_GREEN, 0);
        analogWrite(PIN_RGB_BLUE, 0);
        break;
      case 1: // Capslock Enabled (blue)
        analogWrite(PIN_RGB_RED, 0);
        analogWrite(PIN_RGB_GREEN, 0);
        analogWrite(PIN_RGB_BLUE, 255);
        break;
      case 2: // Battery Good (green)
        analogWrite(PIN_RGB_RED, 0);
        analogWrite(PIN_RGB_GREEN, 255);
        analogWrite(PIN_RGB_BLUE, 0);
        break;
      case 3: // Battery Warning (yellow)
        analogWrite(PIN_RGB_RED, 255);
        analogWrite(PIN_RGB_GREEN, 255);
        analogWrite(PIN_RGB_BLUE, 0);
        break;
      default:
        break;
    }
  }
}


/*oid DebugKeys() {
  int shouldSend = 0;
  
  Serial.println("");
  for (uint8_t c = 0; c < column_count; c++) {  
    for (uint8_t r = 0; r < row_count; r++) {
      Serial.print(keyMapHistory[r][c]);
      Serial.print(", ");

      if (keyMapHistory[r][c] == true)
        shouldSend = keyMapUnmodified[r][c]; 
    }
    Serial.println("");
  }

  Serial.println("");
  Serial.println("KeymapUnmodified: ");

  for (uint8_t c = 0; c < column_count; c++) {
    for (uint8_t r = 0; r < row_count; r++) {
      Serial.print(keyMapUnmodified[r][c]);
      Serial.print(", ");
    }
    Serial.println("");
  }
  
  Serial.print("Should send: ");
  Serial.println(shouldSend);
  Serial.println("");
}*/
