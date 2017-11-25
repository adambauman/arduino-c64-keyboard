#include "CD4051.h"

CD4051::CD4051(uint8_t pin_a0, uint8_t pin_a1, uint8_t pin_a2, uint8_t pin_common_io)
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

void CD4051::Select(uint8_t io_number)
{
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
    //NOTE: (Adam) Running CD4051 matrix as button active on LOW
    pinMode(this->m_pin_common_io, OUTPUT);
    digitalWrite(this->m_pin_common_io, LOW);    
}