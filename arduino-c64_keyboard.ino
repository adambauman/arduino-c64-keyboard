// BauTek C64 PiCase Keyboard Controller
// Version 0.55, August 2017
// Written by Adam Bauman (adam@kungfutreachery.net, https://bitbucket.org/adambauman/arduino-c64_keyboard)
//
// Utilizes an ATmega32U4-based microcontroller and two CD4051 multiplexers to run a Commodore64
// keyboard as a USBHID device. Optionally you can also run a RGB status LED and battery monitoring circuit.
//
// See included wiring diagrams for hardware setup. The C64 keyboard requires one minor modification, you must
// de-solder the leads on the SHIFTLOCK key and run one to the microcontroller ground, the other to pin 9.
//
//
// Copyright (C) 2017, Adam J. Bauman
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

#include "Common.h"
#include "Configuration.h"
#include "CD4051.h"
#include "RgbLed.h"
#include "Battery.h"
#include "KeyMatrix.h"

// Debounce setup, on an ATmega32U4 @ 16MHz 10ms works pretty well
unsigned long startTime = 0;
unsigned int debounceTime = 10;
  
CD4051 cd4051_column(PIN_CD4051_COLUMN_A0, PIN_CD4051_COLUMN_A1, PIN_CD4051_COLUMN_A2, PIN_CD4051_COLUMN_COMMON);
CD4051 cd4051_row(PIN_CD4051_ROW_A0, PIN_CD4051_ROW_A1, PIN_CD4051_ROW_A2, PIN_CD4051_ROW_COMMON);
KeyMatrix key_matrix;

#ifdef _SYSTEM_BATTERY_ENABLED
Battery battery(PIN_BATTERY_SENSOR, PIN_BATTERY_BUTTON);
#endif

#ifdef _RGB_ENABLED
RgbLed power_led(PIN_RGB_RED, PIN_RGB_GREEN, PIN_RGB_BLUE, LED_COLOR_RED, 0.1);
#endif

void setup() {
#ifdef _RGB_ENABLED
	power_led.C64StartupCycle(80, 5);
#endif

#ifdef _DEBUG
	Serial.begin(115200);
	Serial.print("Debug in: ");
	for (uint8_t index = 5; index != 0; Serial.print(index), Serial.print(", "), delay(1000), index--);
	Serial.println("Debug starting");

	//NOTE: (Adam) Un-comment if you want to capture battery calibration data
	//			to the serial terminal. This requires placing a constant load on
	//			the battery and parsing the terminal information, then setting
	//			the threshold levels in Battery.h.
	battery.CalibrateBatteryLevels(20);
#endif
	
	//NOTE: All key reading pins use the internal pullup resistors,
	//	  row drops low when button closed to column. Drop columns LOW so they're ready.
	cd4051_column.SetAsOutput();
	cd4051_row.SetAsInputPullup();
	pinMode(PIN_SHIFT_LOCK, INPUT_PULLUP);
	pinMode(PIN_COLUMN_I, OUTPUT);
	pinMode(PIN_ROW_8, INPUT_PULLUP);
	digitalWrite(PIN_CD4051_COLUMN_COMMON, LOW);
	digitalWrite(PIN_COLUMN_I, LOW);

	key_matrix.StartKeyboard();
}

void loop() {
  if  ((millis() - startTime) > debounceTime) {
      key_matrix.ProcessKeyMatrix(cd4051_row, cd4051_column);
#ifdef _SYSTEM_BATTERY_ENABLED
	  if (battery.UserRequestedLevelCheck()) {
		  battery.FlashLedLevelIndicator(power_led);
	  }
#endif
      startTime = millis();
  } 
}
