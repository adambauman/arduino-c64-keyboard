#include "RgbLed.h"

RgbLed::RgbLed(uint8_t pin_red, uint8_t pin_green, uint8_t pin_blue)
{
    this->m_pin_red = pin_red;
    this->m_pin_green = pin_green;
    this->m_pin_blue = pin_blue;
}

void RgbLed::SoloChangeColor(RgbColor &rgb_color)
{
    
}