#include "Common.h"
#include "RgbLed.h"

RgbLed::RgbLed(IN uint8_t pin_red, IN uint8_t pin_green, IN uint8_t pin_blue)
{
    this->m_pin_red = pin_red;
    this->m_pin_green = pin_green;
    this->m_pin_blue = pin_blue;
}

void RgbLed::SoloChangeColor(IN RgbColor &rgb_color)
{
    
}