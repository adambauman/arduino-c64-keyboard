#ifndef CONFIGURATION_H
#define CONFIGURATION_H

namespace SystemOptions {
    // Set TRUE to enable serial debugging
    boolean debugEnabled = true;

    // Set TRUE to enable the RGB LED
    boolean RGBenabled = true;

    // Set TRUE to enable battery level reading
    boolean batteryEnabled = true;
    // END SETUP PARAMETERS
};

// RESTORE lives outside the muxed matrix, sends BACKSPACE
#define PIN_ROW_8 1
#define PIN_COL_I 2

// Modified SHIFTLOCK (originally closes SHIFT key in hardware, now runs CAPSLOCK)
#define PIN_SHIFT_LOCK 9

// Pins connected to the column CD4051
#define PIN_COL_Z 21
#define PIN_COL_A0 18
#define PIN_COL_A1 19
#define PIN_COL_A2 20

// Pins connected to the row CD4051
#define PIN_ROW_Z 10
#define PIN_ROW_A0 14
#define PIN_ROW_A1 15
#define PIN_ROW_A2 16

// RGB LED pins (optional, set RGBenabled = TRUE to enable custom LED function)
#define PIN_RGB_R 3
#define PIN_RGB_G 5
#define PIN_RGB_B 6

// Battery level check and sensor pins (optional, set batteryEnabled = TRUE to enable function)
#define PIN_BATTERY_BUTTON 7
#define PIN_BATTERY_SENSOR 8

#endif