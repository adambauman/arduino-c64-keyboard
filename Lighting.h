#ifndef LIGHTING_H
#define LIGHTING_H

struct RgbLed{
    int pin_red;
    int pin_green;
    int pin_blue;
    int value_red;
    int value_green;
    int value_blue;
};

class Lighting {
    public:
    void ChangeColor(RgbLed &rgb_led);
    // Disable all lights
    // Enable all lights
    // Pulse and animation functions
};

#endif