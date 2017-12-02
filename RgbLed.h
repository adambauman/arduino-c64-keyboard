#ifndef RGBLED_H
#define RGBLED_H
//#include "CodeLabels.h"
//#include <Arduino.h>
//#include <inttypes.h>

struct RgbColor {
    IN uint8_t red;
    IN uint8_t green;
    IN uint8_t blue;
};

class RgbLed {
    public:
        RgbLed(IN uint8_t pin_red, IN uint8_t pin_green, IN uint8_t pin_blue);

        void SoloChangeColor(IN RgbColor &rgb_color);
        // ChangeMultiLights
        // Disable all lights
        // Enable all lights
        // Pulse and animation functions

    private:
        uint8_t m_pin_red;
        uint8_t m_pin_green;
        uint8_t m_pin_blue;
};

#endif