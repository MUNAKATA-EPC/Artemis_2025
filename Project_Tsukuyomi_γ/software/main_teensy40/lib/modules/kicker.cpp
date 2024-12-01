#include <Arduino.h>
#include "kicker.hpp"

void Kicker::init(int charge, int kick)
{
    this->_charge_pin = charge;
    this->_kick_pin = kick;
}

void Kicker::loop()
{
    _kick_timer.tick();

    if(_is_kicking)
    {
        Serial.println("kicker");

        if(_kick_timer.get_value() <= 5)
        {
            analogWrite(_charge_pin, 0);
            analogWrite(_kick_pin, 0);
        }
        else if(_kick_timer.get_value() <= 50)
        {
            analogWrite(_charge_pin, 0);
            analogWrite(_kick_pin, 1023);
        }
        else if(_kick_timer.get_value() <= 60)
        {
            analogWrite(_charge_pin, 0);
            analogWrite(_kick_pin, 0);
        }
        else if(_kick_timer.get_value() <= 300)
        {
            analogWrite(_charge_pin, 1023);
            analogWrite(_kick_pin, 0);
        }
        else
        {
            _kick_timer.reset();
            _is_kicking = false;
        }
    }
    else
    {
        analogWrite(_kick_pin, 0);
        analogWrite(_charge_pin, 1023);
    }
}

void Kicker::kick()
{
    if(!_is_kicking)
    {
        _kick_timer.start();
        _is_kicking = true;
    }
}

bool Kicker::is_kicking()
{
    return this->_is_kicking;
}