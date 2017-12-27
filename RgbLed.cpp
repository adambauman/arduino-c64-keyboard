#include "common.h"
#include "RgbLed.h"

RgbLed::RgbLed(
	const uint8_t pin_red,
	const uint8_t pin_green,
	const uint8_t pin_blue,
	const RgbColor default_color,
	const float default_intensity)
{
    this->m_pin_red = pin_red;
    this->m_pin_green = pin_green;
    this->m_pin_blue = pin_blue;
	this->m_default_intensity = default_intensity;
	this->m_default_color = default_color;

	this->Reset();
}

void RgbLed::SetColor(const RgbColor &rgb_color)
{
	analogWrite(this->m_pin_red, rgb_color.red);
	analogWrite(this->m_pin_green, rgb_color.green);
	analogWrite(this->m_pin_blue, rgb_color.blue);
	this->m_current_color = rgb_color;
}

void RgbLed::SetIntensity(const float intensity_multiplier)
{
	if (1 < intensity_multiplier) { return; }
	RgbColor new_color{};
	new_color.red = (this->m_current_color.red * intensity_multiplier);
	new_color.green = (this->m_current_color.green * intensity_multiplier);
	new_color.blue = (this->m_current_color.blue * intensity_multiplier);
	this->SetColor(new_color);
#ifdef _DEBUG
	Serial.print("New LED Intensity: "); Serial.print(new_color.red, DEC); Serial.print(", "); Serial.print(new_color.green, DEC); Serial.print(", "); Serial.println(new_color.blue, DEC);
#endif
}

void RgbLed::FadeToColor(const RgbColor &target_color, int fade_time) 
{
	//TODO: (Adam) Super-janky first attempt, check out some examples and redo
	RgbColor mixer_color = this->m_current_color;
	char red_increment = 1;
	char blue_increment = 1;
	char green_increment = 1;
	if (128 < m_current_color.red && 128 < target_color.red && m_current_color.red > target_color.red) { red_increment = -1; }
	if (128 < m_current_color.blue && 128 < target_color.blue && m_current_color.blue > target_color.blue) { blue_increment = -1; }
	if (128 < m_current_color.green && 128 < target_color.green && m_current_color.green > target_color.green) { green_increment = -1; }

	for (;;) {
		if (target_color.red != mixer_color.red) { mixer_color.red += red_increment; }
		if (target_color.green != mixer_color.green) { mixer_color.green += blue_increment; }
		if (target_color.blue != mixer_color.blue) { mixer_color.blue += green_increment; }

		this->SetColor(mixer_color);
		if (target_color.red == mixer_color.red
			&& target_color.green == mixer_color.green
			&& target_color.blue == mixer_color.blue) {
			break;
		}
		delay(fade_time);
	}
}

void RgbLed::Reset() 
{
	this->SetIntensity(this->m_default_intensity);
	this->SetColor(this->m_default_color);
}

void RgbLed::C64StartupCycle(const int inter_color_delay, const uint8_t loop_count)
{
	RgbColor starting_color = this->m_current_color;
	for (uint8_t index = 0; index < loop_count; index++, this->C64StartupCycleAction(inter_color_delay));
	this->SetColor(starting_color);
}

void RgbLed::C64StartupCycleAction(const uint8_t &inter_color_delay)
{
	this->SetColor(LED_COLOR_RED);
	delay(inter_color_delay);
	this->SetColor(LED_COLOR_ORANGE);
	delay(inter_color_delay);
	this->SetColor(LED_COLOR_YELLOW);
	delay(inter_color_delay);
	this->SetColor(LED_COLOR_GREEN);
	delay(inter_color_delay);
	this->SetColor(LED_COLOR_BLUE);
	delay(inter_color_delay);
}