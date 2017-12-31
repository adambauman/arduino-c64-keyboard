#ifndef RGBLED_H
#define RGBLED_H

#define LED_COLOR_RED    { 255, 0,   0   }
#define LED_COLOR_GREEN  { 0,   255, 0   }
#define LED_COLOR_BLUE   { 0,   0,   255 }
#define LED_COLOR_YELLOW { 255, 255, 0   }
#define LED_COLOR_ORANGE { 255, 150, 50  }
#define LED_COLOR_WHITE  { 255, 255, 255 }
#define LED_COLOR_BLACK  { 0,   0,   0   }
#define LED_COLOR_CYAN   { 0,   255, 255 }
#define LED_COLOR_PINK   { 255, 100, 255 }
#define LED_COLOR_PURPLE { 180, 100, 255 }

struct RgbColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class RgbLed {
    public:
        RgbLed(
			const uint8_t pin_red,
			const uint8_t pin_green,
			const uint8_t pin_blue,
			const RgbColor default_color,
			const float default_intensity
		);

        void SetColor(const RgbColor &rgb_color);
		void SetIntensity(const float intensity_multiplier);
		void FadeToColor(const RgbColor &target_color, int step_time);
		void Reset();
		void C64StartupCycle(const int inter_color_delay, const uint8_t loop_count);
		void Blink(const int blink_count, const int blink_delay);
		void Blink(const int blink_count, const int blink_delay, RgbColor blink_color);

	private:
		void C64StartupCycleAction(const uint8_t &inter_color_delay);
		int8_t CalculateFadeSteps(
			uint8_t target_value, 
			uint8_t current_value
		);
		uint8_t CalculateFadeColor(
			const uint8_t &current_color_value,
			const int8_t &step_count,
			const uint8_t &index);

    private:
        uint8_t m_pin_red;
        uint8_t m_pin_green;
        uint8_t m_pin_blue;
		float m_default_intensity;
		RgbColor m_default_color;
		RgbColor m_current_color;
};

#endif