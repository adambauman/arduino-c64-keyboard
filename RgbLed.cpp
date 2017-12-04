#include <Arduino.h>
#include "RgbLed.h"

RgbLed::RgbLed(uint8_t pin_red, uint8_t pin_green, uint8_t pin_blue)
{
    this->m_pin_red = pin_red;
    this->m_pin_green = pin_green;
    this->m_pin_blue = pin_blue;
	this->m_current_color = { 255, 255, 255 };

	this->m_color_red =		{ 255, 0,   0   };
	this->m_color_green =	{ 0,   255, 0   };
	this->m_color_blue =	{ 0,   0,   255 };
	this->m_color_yellow =	{ 255, 255, 0   };
	this->m_color_orange =	{ 255, 150, 50  };
	this->m_color_white =	{ 255, 255, 255 };
	this->m_color_cyan =	{ 0,   255, 255 };
	this->m_color_pink =	{ 255, 100, 255 };
	this->m_color_purple =	{ 180, 100, 255 };
}

void RgbLed::SetColor(RgbColor &rgb_color)
{
	analogWrite(this->m_pin_red, rgb_color.red);
	analogWrite(this->m_pin_green, rgb_color.green);
	analogWrite(this->m_pin_blue, rgb_color.blue);
}