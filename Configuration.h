#ifndef CONFIGURATION_H
#define CONFIGURATION_H

namespace SystemOptions {
    // Set TRUE to enable serial debugging
    boolean debugEnabled = true;
    // Set TRUE to enable the RGB LED
    boolean RGBenabled = true;
    // Set TRUE to enable battery level reading
    boolean batteryEnabled = true;
    // Debounce setup, on an ATmega32U4 @ 16MHz 10ms works pretty well
    unsigned int debounce_time = 10;
};

namespace Pins {
    // RESTORE lives outside the muxed matrix, sends BACKSPACE
    int row_8 = 1;
    int column_i = 2;
    // Modified SHIFTLOCK (originally closes SHIFT key in hardware, now runs CAPSLOCK)
    int shift_lock = 9;
    // Battery level check and sensor pins (optional, set batteryEnabled = TRUE to enable function)
    int battery_button = 7;
    int battery_sensor = 8;

    // CD4051 multiplexer pins. Combination of A0-A2 to select, A0=1, A1=2, A2=4. Common in/out pin is Z/
    // Running 2x CD4051s, one for rows, one for columns
    namespace CD4051 {
        namespace Column {
            int a0 = 18;
            int a1 = 19;
            int a2 = 20;
            int common_io = 21; // Z pin
        };
        namespace Row {
            int a0 = 14;
            int a1 = 15;
            int a2 = 16;
            int common_io = 10; // Z pin
        };
    };

    // Optional RGB LED pins
    namespace RGB {
        int red =   3;
        int green = 5;
        int blue =  6;
    };
};

#endif