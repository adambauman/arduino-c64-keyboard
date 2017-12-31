#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// Enable RGB lighting, battery monitoring
#define _RGB_ENABLED
#define _SYSTEM_BATTERY_ENABLED

// Debounce setup, on an ATmega32U4 @ 16MHz 10ms works pretty well
#define SYSTEM_DEBOUNCE_TIME		10

// RESTORE lives outside the muxed matrix, sends BACKSPACE
#define PIN_ROW_8					1
#define PIN_COLUMN_I				2
// Modified SHIFTLOCK (originally closes SHIFT key in hardware, now runs CAPSLOCK)
#define PIN_SHIFT_LOCK				9
// Battery level check and sensor pins (optional, set batteryEnabled = TRUE to enable function)
#define PIN_BATTERY_BUTTON			7
#define PIN_BATTERY_SENSOR			8

// CD4051 multiplexer pins. Combination of A0-A2 to select, A0=1, A1=2, A2=4. Common in/out pin is Z/
// Running 2x CD4051s, one for rows, one for columns
#define PIN_CD4051_COLUMN_A0		18
#define PIN_CD4051_COLUMN_A1		19
#define PIN_CD4051_COLUMN_A2		20
#define PIN_CD4051_COLUMN_COMMON	21

#define PIN_CD4051_ROW_A0			14
#define PIN_CD4051_ROW_A1			15
#define PIN_CD4051_ROW_A2			16
#define PIN_CD4051_ROW_COMMON		10

// Optional RGB LED pins
#define PIN_RGB_RED					3
#define PIN_RGB_GREEN				5
#define PIN_RGB_BLUE				6

#endif