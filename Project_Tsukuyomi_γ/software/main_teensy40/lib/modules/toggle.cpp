#include <Arduino.h>
#include "toggle.hpp"

void Toggle::init(int pin, Toggle_Value_Type type)
{
    this->_pin = pin;
    this->_value_type = type;

    pinMode(pin, 
            type == Toggle_Value_Type::PULLDOWN ? INPUT_PULLDOWN : INPUT_PULLUP);
}

bool Toggle::is_turned()
{
    switch(_value_type)
    {
        case Toggle_Value_Type::PULLDOWN:
            {
                if(digitalRead(_pin) == HIGH)
                {
                    return true;
                }
            }
            break;

        case Toggle_Value_Type::PULLUP:
            {
                if(digitalRead(_pin) == LOW)
                {
                    return true;
                }
            }
            break;
    }

    return false;
}