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

void RgbLed::FadeToColor(const RgbColor &target_color, int step_time) 
{
	RgbColor mixer_color = this->m_current_color;
	uint8_t red_step = this->CalculateFadeSteps(target_color.red, mixer_color.red);
	uint8_t green_step = this->CalculateFadeSteps(target_color.green, mixer_color.green);
	uint8_t blue_step = this->CalculateFadeSteps(target_color.blue, mixer_color.blue);
#ifdef _DEBUG
	Serial.print("red_step: "); Serial.print(red_step); Serial.print(" green_step: "); Serial.print(green_step); Serial.print(" blue_step: "); Serial.println(blue_step);
#endif
	//NOTE: (Adam): This is still janky and should be considered unimplemented for now
	//TODO: (Adam): Implement proper color cross-fading

	for (uint8_t index = 0; index <= 765; index++) {
		mixer_color.red = this->CalculateFadeColor(mixer_color.red, red_step, index);
		mixer_color.green = this->CalculateFadeColor(mixer_color.green, green_step, index);
		mixer_color.blue = this->CalculateFadeColor(mixer_color.blue, blue_step, index);
#ifdef _DEBUG
		Serial.print("fade red: "); Serial.print(mixer_color.red); Serial.print(" green: "); Serial.print(mixer_color.green); Serial.print(" blue: "); Serial.println(mixer_color.blue);
#endif
		this->SetColor(mixer_color);
		delay(step_time);
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

void RgbLed::Blink(
	const int blink_count,
	const int blink_delay)
{
	this->Blink(blink_count, blink_delay, this->m_current_color);
}

void RgbLed::Blink(
	const int blink_count,
	const int blink_delay,
	const RgbColor blink_color)
{
	RgbColor starting_color = this->m_current_color;
	for (int index = 0; blink_count > index; index++) {
		this->SetColor(blink_color);
		delay(blink_delay);
		this->SetColor(LED_COLOR_BLACK);
		delay(blink_delay);
	}
	this->SetColor(starting_color);
}


int8_t RgbLed::CalculateFadeSteps(
	uint8_t target_value,
	uint8_t current_value)
{
#ifdef _DEBUG
	//Serial.print("Fade target: "); Serial.print(target_value); Serial.print(" Current value: "); Serial.println(current_value);
#endif
	if (0 == target_value && 0 == current_value) { return(0); }

	if (2 > target_value) { target_value = 2; }
	if (2 > current_value) { current_value = 2; }
	int8_t step_count = current_value / target_value;
	step_count = -step_count;
	if (current_value < target_value) {
		step_count = target_value / current_value;
		Serial.println(step_count);
	}

	return(step_count);
}

uint8_t RgbLed::CalculateFadeColor(
	const uint8_t &current_color_value,
	const int8_t &step_count,
	const uint8_t &index)
{
	//TODO: (Adam) Removed scrub debug output
	uint8_t calculated_color_value = current_color_value;
	if (0 == index % step_count) { 
		calculated_color_value = current_color_value + step_count;
		Serial.print("modulas hit, clc_color_value: "); Serial.println(calculated_color_value);
	}

	return(calculated_color_value);
}
