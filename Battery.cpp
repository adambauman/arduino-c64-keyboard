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

void Battery::CheckStatusButton(bool &shutdown_requested, RgbLed &power_led)
{
	shutdown_requested = false;
	unsigned long press_time = 0;
	if (!digitalRead(this->m_pin_check_trigger)) {
		press_time = millis();
		delay(SYSTEM_DEBOUNCE_TIME);
	}

	for (;;) {
		if (0 == press_time) { break; }
		if (SHUTDOWN_BUTTON_TIME <= (millis() - press_time)) {
#ifdef _DEBUG
			Serial.println("System shutdown requested");
#endif
			shutdown_requested = true;
			break;
		}

		//NOTE: (Adam) No longer pushed when pin is high
		if (digitalRead(this->m_pin_check_trigger)) {
			this->FlashLedLevelIndicator(power_led);
			break; 
		} 

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
