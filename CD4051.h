#ifndef CD4051_H
#define CD4051_H

#include <Arduino.h>
#include <inttypes.h>

class CD4051 {
    public:
        CD4051(uint8_t pin_a0, uint8_t pin_a1, uint8_t pin_a2, uint8_t pin_common_io);
        void Select(uint8_t io_number);
        uint8_t ReadCommonValue();
        void SetAsMatrixSink();
    private:
        uint8_t m_pin_a0;
        uint8_t m_pin_a1;
        uint8_t m_pin_a2;
        uint8_t m_pin_common_io;
};

#endif