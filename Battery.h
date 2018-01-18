#ifndef BATTERY_H
#define BATTERY_H

//TODO: (Adam) Setup calibration rigging and set these values after testing
#define BATTERY_LEVEL_HIGH 606
#define BATTERY_LEVEL_MEDIUM 540
#define BATTERY_LEVEL_LOW 358
#define BATTERY_LEVEL_CRITICAL 195
#define BATTERY_LEVEL_DEAD 12

class Battery
{
public:
	Battery(const uint8_t pin_photo_sense, const uint8_t pin_check_trigger);
	~Battery();

	void FlashLedLevelIndicator(RgbLed &battery_led);
	void CheckStatusButton(bool &shutdown_requested, RgbLed &power_led);
	void CalibrateBatteryLevels(
		const int lower_reading_limit,
		const long read_interval = 60000);

private:
	int GetBatteryLevel();

private:
	uint8_t m_pin_photo_sense;
	uint8_t m_pin_check_trigger;
};

#endif