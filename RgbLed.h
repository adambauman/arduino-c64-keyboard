#ifndef RGBLED_H
#define RGBLED_H

#include <Arduino.h>
#include <inttypes.h>

struct RgbColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class RgbLed {
    public:
        RgbLed(uint8_t pin_red, uint8_t pin_green, uint8_t pin_blue);

        void SoloChangeColor(RgbColor &rgb_color);
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