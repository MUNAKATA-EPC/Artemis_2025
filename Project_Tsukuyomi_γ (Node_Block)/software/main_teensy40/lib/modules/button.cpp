#include <Arduino.h>
#include "button.hpp"

void Button::init(int pin, Button::Button_Value_Type type)
{
    this->_pin = pin;
    this->_value_type = type;

    if(this->_value_type == Button::Button_Value_Type::PULLDOWN)
    {
        pinMode(_pin, INPUT_PULLDOWN);
    }
    else
    {
        pinMode(_pin, INPUT_PULLUP);
    }

    this->_is_init = true;
}

void Button::loop()
{
    int button_value = digitalRead(_pin);

    switch(_value_type)
    {
        case Button::Button_Value_Type::PULLUP:
            {
                if(button_value == LOW)
                {
                    _push_time++;
                }
                else
                {
                    if(_push_time <= 0)
                    {
                        _push_time = 0;
                    }
                    else
                    {
                        _push_time = -1;
                    }
                }
            }
            break;

        case Button::Button_Value_Type::PULLDOWN:
            {
                if(button_value == HIGH)
                {
                    _push_time++;
                }
                else
                {
                    if(_push_time <= 0)
                    {
                        _push_time = 0;
                    }
                    else
                    {
                        _push_time = -1;
                    }
                }
            }
            break;
    }
}

bool Button::is_pushing()
{
    return _push_time >= 1;
}

bool Button::is_pushed()
{
    return _push_time == 1;
}

bool Button::is_released()
{
    return _push_time == -1;
}

bool Button::is_init()
{
    return _is_init;
}