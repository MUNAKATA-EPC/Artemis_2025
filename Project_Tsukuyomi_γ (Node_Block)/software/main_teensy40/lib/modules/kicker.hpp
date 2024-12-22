#ifndef _KICKER_HPP
#define _KICKER_HPP

#include "timer.hpp"

class Kicker{
public:
    void init(int charge, int kick);
    void loop();
    void kick();
    bool is_kicking();

private:
    int _charge_pin;
    int _kick_pin;
    bool _is_kicking;
    Timer _kick_timer;
};

#endif