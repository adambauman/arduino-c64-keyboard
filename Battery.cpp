#include "Common.h"
#include "RgbLed.h"
#include "Battery.h"

Battery::Battery(const uint8_t pin_photo_sense, const uint8_t pin_check_trigger)
{
	this->m_pin_photo_sense = pin_photo_sense;
	this->m_pin_check_trigger = pin_check_trigger;
	pinMode(this->m_pin_check_trigger, INPUT_PULLUP);
}

Battery::~Battery(){}

void Battery::FlashLedLevelIndicator(RgbLed &battery_led)
{
	//NOTE: (Adam) Works well for packs with multi-stage status LEDs, if you have
	//			a pack that can be cracked opened then reading actual cell voltage
	//		    would be a neat feature to implement
	int battery_level = this->GetBatteryLevel();
	if (battery_level > BATTERY_LEVEL_MEDIUM) {
		battery_led.Blink(3 , 400, LED_COLOR_GREEN);
	} else if (battery_level > BATTERY_LEVEL_LOW) {
		battery_led.Blink(3 , 400, LED_COLOR_YELLOW);
	} else if (battery_level > BATTERY_LEVEL_DEAD) {
		battery_led.Blink(10, 100, LED_COLOR_RED);
	}
}

void Battery::CalibrateBatteryLevels(
	const int lower_reading_limit, 
	const long read_interval)
{
	//NOTE: (Adam) Calling this will read battery levels and output to the serial terminal
	//			until the levels reach the threshold passed to it. _DEBUG must be defined
#ifdef _DEBUG
	Serial.println("Starting battery calibration...");
	for (;;) {
		int battery_level = this->GetBatteryLevel();
		if (lower_reading_limit >= battery_level) { break;}
		Serial.println(battery_level, DEC);
		delay(read_interval);
	}
	Serial.println("Battery calibration complete!");
#endif
}

int Battery::GetBatteryLevel()
{
	int battery_level = 0;
	for (uint8_t index; 5 > index; index++) {
		delay(100);
		battery_level = battery_level + analogRead(this->m_pin_photo_sense);
	}
	battery_level = battery_level / 5;
#ifdef _DEBUG
	Serial.print("Battery photo_sense reading: "); Serial.println(battery_level, DEC);
#endif
	return(battery_level);
}
