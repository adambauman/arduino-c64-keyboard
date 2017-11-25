#include "CD4051.h"

CD4051::CD4051(uint8_t pin_a0, uint8_t pin_a1, uint8_t pin_a2, uint8_t pin_common_io)
{
    this->m_pin_a0 = pin_a0;
    this->m_pin_a1 = pin_a1;
    this->m_pin_a2 = pin_a2;
    this->m_pin_common_io = pin_common_io;
}

void CD4051::Select(uint8_t io_number)
{
    
}