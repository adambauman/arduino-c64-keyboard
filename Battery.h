#ifndef BATTERY_H
#define BATTERY_H

//TODO: (Adam) Setup calibration rigging and set these values after testing
#define BATTERY_LEVEL_HIGH 128
#define BATTERY_LEVEL_MEDIUM 100
#define BATTERY_LEVEL_LOW 60
#define BATTERY_LEVEL_CRITICAL 40

class Battery
{
public:
	Battery(const uint8_t pin_photo_sense, const uint8_t pin_check_trigger);
	~Battery();

	void FlashLedLevelIndicator(RgbLed &battery_led);
	void CalibrateBatteryLevels(
		const int lower_reading_limit,
		const long read_interval = 60000);
	bool UserRequestedLevelCheck();

private:
	int GetBatteryLevel();

private:
	uint8_t m_pin_photo_sense;
	uint8_t m_pin_check_trigger;
};

#endif