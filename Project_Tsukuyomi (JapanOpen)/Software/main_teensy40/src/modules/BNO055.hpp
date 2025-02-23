#ifndef _BNO055_HPP
#define _BNO055_HPP

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>

#include "button.hpp"

class BNO055{
public:
    void init(int reset_pin, int sub_pin = -1);
    void process();
    int get_degrees();

private:
    Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
    Button reset;
};

#endif