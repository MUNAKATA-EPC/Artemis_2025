#include <Arduino.h>
#include "timer.hpp"

void Timer::tick()
{
    if(_is_ticking)
    {
        _timer_val = millis() - _start_val;
    }
}

void Timer::start()
{
    if(!_is_ticking)
    {
        _start_val = millis();
        _is_ticking = true;
    }
}

void Timer::stop()
{
    _is_ticking = false;
}

void Timer::reset()
{
    _is_ticking = false;
    _timer_val = 0;
}

int Timer::get_value()
{
    return _timer_val;
}

void Timer::set_value(int value)
{
    _timer_val = value;
}

bool Timer::is_ticking()
{
    return _is_ticking;
}