#ifndef LIGHTING_H
#define LIGHTING_H

struct RGBLED{
    int pin_red;
    int pin_green;
    int pin_blue;
    int value_red;
    int value_green;
    int value_blue;
};

class Lighting {
    public:
    SetRGBLED(int red, int green, int blue);
};

#endif