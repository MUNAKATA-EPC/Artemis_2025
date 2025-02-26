#ifndef _KICKER_HPP
#define _KICKER_HPP

#include "common/timer.hpp"

class Kicker{
public:
    void init(int charge, int kick);
    void loop();
    void kick(int delay);
    bool is_kicking();

private:
    int _delay_time;
    int _charge_pin;
    int _kick_pin;
    bool _is_kicking;
    Timer _kick_timer;
};

#endif