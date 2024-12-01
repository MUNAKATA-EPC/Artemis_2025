#ifndef _TIMER_HPP
#define _TIMER_HPP

class Timer{
private:
    int _timer_val;
    int _start_val;
    bool _is_ticking;
public:
    void tick();
    void start();
    void stop();
    void reset();
    int get_value();
    void set_value(int value);
    bool is_ticking();
};

#endif