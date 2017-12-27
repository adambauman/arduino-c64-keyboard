#include "Common.h"
#include "CD4051.h"

CD4051::CD4051(uint8_t pin_a0, uint8_t pin_a1, uint8_t pin_a2, uint8_t pin_common_io)
{
#ifdef _DEBUG
	Serial.print("CD4051 starting, a0="); Serial.print(pin_a0); Serial.print(" a1="); Serial.print(pin_a1); 
	Serial.print(" a2="); Serial.print(pin_a2); Serial.print(" common_io="); Serial.print(pin_common_io); Serial.println("");
#endif
    this->m_pin_a0 = pin_a0;
    this->m_pin_a1 = pin_a1;
    this->m_pin_a2 = pin_a2;
    this->m_pin_common_io = pin_common_io;

    pinMode(this->m_pin_a0, OUTPUT);
    pinMode(this->m_pin_a1, OUTPUT);
    pinMode(this->m_pin_a2, OUTPUT);

	this->ClearCommonModeTracking();
	this->Reset();
}

void CD4051::Select(uint8_t io_number)
{
#ifdef _DEBUG
	//Serial.print("Selecting: "); Serial.println(io_number);
#endif
    digitalWrite(this->m_pin_a0, bitRead(io_number, 0));
    digitalWrite(this->m_pin_a1, bitRead(io_number, 1));
    digitalWrite(this->m_pin_a2, bitRead(io_number, 2));
    m_current_selection = io_number;
}

uint8_t CD4051::ReadCommonValue()
{
	uint8_t common_value = digitalRead(this->m_pin_common_io);
#ifdef _DEBUG
	//Serial.print("Common IO Value: "); Serial.println(common_value);
#endif
    return (common_value);
}

void CD4051::SetAsInputPullup()
{
#ifdef _DEBUG
	Serial.println("CD4051 set as INPUT_PULLUP");
#endif
    pinMode(this->m_pin_common_io, INPUT_PULLUP);
	this->ClearCommonModeTracking();
	this->m_common_mode_input_pullup = true;
}

void CD4051::SetAsOutput()
{
#ifdef _DEBUG
	Serial.println("CD4051 set as OUTPUT");
#endif
	pinMode(this->m_pin_common_io, OUTPUT);
	digitalWrite(this -> m_pin_common_io, LOW);
	this->ClearCommonModeTracking();
	this->m_common_mode_output = true;
}

void CD4051::SetAsInput()
{
#ifdef _DEBUG
	Serial.println("CD4051 set as INPUT");
#endif
	pinMode(this->m_pin_common_io, OUTPUT);
	digitalWrite(this->m_pin_common_io, LOW);
	this->ClearCommonModeTracking();
	this->m_common_mode_input = true;
}

void CD4051::Reset()
{
	this->Select(0);
	if (!m_common_mode_input_pullup) { digitalWrite(this->m_pin_common_io, LOW); }
}

void CD4051::ClearCommonModeTracking()
{
	this->m_common_mode_input = false;
	this->m_common_mode_output = false;
	this->m_common_mode_input_pullup = false;
}
