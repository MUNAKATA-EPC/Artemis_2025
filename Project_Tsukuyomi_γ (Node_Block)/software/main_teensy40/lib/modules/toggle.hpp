#ifndef _TOGGLE_HPP
#define _TOGGLE_HPP

class Toggle
{
public:
    typedef enum{
        PULLUP,
        PULLDOWN
    }Toggle_Value_Type;

    void init(int pin, Toggle_Value_Type type);
    bool is_turned();

private:
    int _pin;
    Toggle_Value_Type _value_type;

};

#endif