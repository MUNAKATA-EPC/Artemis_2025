#include <Arduino.h>
#include "engelline.hpp"
#include "sensor_variables.hpp"

double line_sensors_deg[16] = { 22.5 * 0,   22.5 * 1,   22.5 * 2,   22.5 * 3,
                                22.5 * 4,   22.5 * 5,   22.5 * 6,   22.5 * 7,
                                22.5 * 8,   22.5 * 9,   22.5 * 10,  22.5 * 11,
                                22.5 * 12,  22.5 * 13,  22.5 * 14,  22.5 * 15,
                                };

bool is_line_detected()
{
    bool ret = false;

    for(int i = 0; i < 16; i++)
    {
        if(line_data[i] == 1)
        {
            ret = true;
        }
    }

    return ret;
}

void line_init()
{
    
}

void line_process()
{
    if(is_line_detected())
    {
        tone(3, 4000, 100);
    }
}