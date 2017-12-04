#ifndef RGBLED_H
#define RGBLED_H

struct RgbColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class RgbLed {
    public:
        RgbLed(uint8_t pin_red, uint8_t pin_green, uint8_t pin_blue);

        void SetColor(RgbColor &rgb_color);
		void LedOn();
		void LedOff();

	public:
		RgbColor m_color_red;
		RgbColor m_color_green;
		RgbColor m_color_blue;
		RgbColor m_color_yellow;
		RgbColor m_color_orange;
		RgbColor m_color_white;
		RgbColor m_color_cyan;
		RgbColor m_color_pink;
		RgbColor m_color_purple;

    private:
        uint8_t m_pin_red;
        uint8_t m_pin_green;
        uint8_t m_pin_blue;
		RgbColor m_current_color;
};

#endif