#include "Common.h"
#include "CD4051.h"

CD4051::CD4051(IN uint8_t pin_a0, IN uint8_t pin_a1, IN uint8_t pin_a2, IN uint8_t pin_common_io)
{
    this->m_pin_a0 = pin_a0;
    this->m_pin_a1 = pin_a1;
    this->m_pin_a2 = pin_a2;
    this->m_pin_common_io = pin_common_io;

    pinMode(this->m_pin_common_io, INPUT_PULLUP); // column common_io was set to OUTPUT before code cleanup
    pinMode(this->m_pin_a0, OUTPUT);
    pinMode(this->m_pin_a1, OUTPUT);
    pinMode(this->m_pin_a2, OUTPUT);

    this->Select(0);
}

void CD4051::Select(IN uint8_t io_number)
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
    return (digitalRead(this->m_pin_common_io));
}

void CD4051::SetAsMatrixSink()
{
#ifdef _DEBUG
	Serial.println("CD4051 set as sink");
#endif
    //NOTE: (Adam) Running CD4051 matrix as button active on LOW
    pinMode(this->m_pin_common_io, OUTPUT);
    digitalWrite(this->m_pin_common_io, LOW);    
}

void CD4051::SetAsMatrixEnergize()
{
	pinMode(this->m_pin_common_io, INPUT_PULLUP);
	digitalWrite(this->m_pin_common_io, HIGH);
}
