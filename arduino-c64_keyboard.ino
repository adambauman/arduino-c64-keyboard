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

#include <Keyboard.h>
#include "Configuration.h"
#include "CD4051.h"

// Size of the keyboard matrix. C64 is technically 9x9 but the RESTORE key lives on its own and is handled seperatly
const uint8_t row_count = 8;
const uint8_t column_count = 8;

// Set if matrix is changed, stops loop from writing unnecessary key commands
boolean keysHaveChanged = false;

// Tracks state of modifier keys (currently unused, running custom OS keymaps instead)
boolean shiftState = false;
boolean altState = false;
boolean ctrlState = false;
boolean capsLockState = false;

// Tracks RESTORE and SHIFTLOCK parameters
boolean historyRestore = false;
boolean historyShiftlock = false;
boolean statusRestore = false;
boolean statusShiftlock = false;

// Debounce setup, on an ATmega32U4 @ 16MHz 10ms works pretty well
unsigned long startTime = 0;
unsigned int debounceTime = 10;
  
// Matrix of un-modified keycodes (direct ASCII values)
uint8_t keyMapUnmodified[row_count][column_count] = {
  { 49,  96, 128, 177,  32, 130, 113,  50},
  { 51, 119,  97, 129, 122, 115, 101,  52},
  { 55, 121, 103, 118,  98, 104, 117,  56},
  { 53, 114, 100, 120,  99, 102, 116,  54},
  { 57, 105, 106, 110, 109, 107, 111,  48},
  { 43, 112, 108,  44,  46,  58,  64,  45},
  { 36,  42,  59,  47, 133,  61,  94, 179},
  {212, 176, 215, 217, 194, 196, 198, 200}
};

// Matrix of SHIFT modified keycodes (direct ASCII values) (currently unused, running custom OS keymaps instead)
uint8_t keyMapShifted[row_count][column_count] = {
  { 33, 126,   0,   0,   0,   0,   0,  34},
  { 35,   0,   0,   0,   0,   0,   0,  36},
  { 37,   0,   0,   0,   0,   0,   0,  38},
  { 39,   0,   0,   0,   0, 104,   0,  40},
  { 41,   0,   0,   0,   0,   0,   0,  94},
  {  0,   0,   0,  60,  62,  91,   0,  95},
  {  0,   0,  92,  63,   0,   0, 214, 210}
};

// Matrix of key status during a single scan loop
boolean keyMapStatus[row_count][column_count] = {
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false}
};

// Matrix of key status during the last scan loop, compared to determine if keys have changed
boolean keyMapHistory[row_count][column_count] = {
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false}
};

CD4051 cd4015_column(PIN_CD4051_COLUMN_A0, PIN_CD4051_COLUMN_A1, PIN_CD4051_COLUMN_A2, PIN_CD4051_COLUMN_COMMON);
CD4051 cd4015_row(PIN_CD4051_ROW_A0, PIN_CD4051_ROW_A1, PIN_CD4051_ROW_A2, PIN_CD4051_ROW_COMMON);


//
// setup()
//
void setup() {
  if (SYSTEM_DEBUG_ENABLED)
    Serial.begin(115200);

  cd4015_column.SetAsOutput();
  cd4015_row.SetAsInputPullup();

  // All key reading pins use the internal pullup resistors
  pinMode(PIN_SHIFT_LOCK, INPUT_PULLUP);
  pinMode(PIN_COLUMN_I, OUTPUT);
  pinMode(PIN_ROW_8, INPUT_PULLUP);

  // Row drops low when button closed to column. Drop columns LOW so they're ready.
  digitalWrite(PIN_CD4051_COLUMN_COMMON, LOW);
  digitalWrite(PIN_COLUMN_I, LOW);

  Keyboard.begin(); 
}


void loop() {
  if  ((millis() - startTime) > debounceTime) {
      ScanKeys();
      startTime = millis();
  }

  // Fire off the keys if key status have changed
  if (keysHaveChanged == true)
    WriteKeys();

  // Mirror the status key map into the history key map
  for (uint8_t c = 0; c < column_count; c++) {  
	  for (uint8_t r = 0; r < row_count; keyMapHistory[r][c] = keyMapStatus[r][c], r++) {}
  }

  // Mirror the status of the special keys
  historyRestore = statusRestore;

  if (SYSTEM_DEBUG_ENABLED && keysHaveChanged == true)
    DebugKeys();

  // Reset tracking variables, go for another loop
  keysHaveChanged = false;
    
} // End loop()


void ScanKeys() {
	for (uint8_t column = 0; column < column_count; column++) {
		cd4015_column.Select(column);
		for (uint8_t row = 0; row < row_count; row++) {
			cd4015_row.Select(row);
			keyMapStatus[row][column] = false;
			if (!digitalRead(PIN_CD4051_ROW_COMMON)) { keyMapStatus[row][column] = true; } //Key active if LOW
			if (keyMapStatus[row][column] != keyMapHistory[row][column]) { keysHaveChanged = true; }
		}
	}

	statusRestore = false;
    if (!digitalRead(PIN_ROW_8)) { statusRestore = true; }
	statusShiftlock = false;
	if (!digitalRead(PIN_SHIFT_LOCK)) { statusShiftlock = true; }

	if (historyRestore != statusRestore || historyShiftlock != statusShiftlock) { keysHaveChanged = true; }
}

void WriteKeys() {
  for (uint8_t column = 0; column < column_count; column++){
    for (uint8_t row = 0; row < row_count; row++) {
      if (keyMapStatus[row][column] && !keyMapHistory[row][column]) {
        Keyboard.press(keyMapUnmodified[column][row]); // column and row need to reversed. Sure I'm derping on something here, but it works
      } else if (!keyMapStatus[row][column] && keyMapHistory[row][column]) {
        Keyboard.release(keyMapUnmodified[column][row]); // column and row need to reversed. Sure I'm derping on something here, but it works
      }
    }
  }

  if (statusRestore && !historyRestore) {
    Keyboard.press(178);
  } else if (!statusRestore && historyRestore) {
    Keyboard.release(178);
  }

  if (statusShiftlock && !historyShiftlock) {
    Keyboard.write(193);
  } else if (!statusShiftlock && historyShiftlock) {
    Keyboard.write(193);
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


void DebugKeys() {
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
}
