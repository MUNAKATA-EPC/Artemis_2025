#include <Arduino.h>
#include "engelline.hpp"
#include "sensor_variables.hpp"

double line_sensors_deg[16] = { 22.5 * 0,   22.5 * 1,   22.5 * 2,   22.5 * 3,
                                22.5 * 4,   22.5 * 5,   22.5 * 6,   22.5 * 7,
                                22.5 * 8,   22.5 * 9,   22.5 * 10,  22.5 * 11,
                                22.5 * 12,  22.5 * 13,  22.5 * 14,  22.5 * 15,
                                };

int line_deg;
int line_first_deg;
int previous_line_deg;

int line_evacuation_deg;

bool is_halfout;

bool is_exist_line_in_range(int check_value)
{
    if(line_first_deg <= check_value)
    {
        if(line_deg >= line_first_deg + check_value && line_deg <= line_first_deg - check_value + 360)
        {

        }
        else
        {
            line_first_deg = line_deg;
            is_halfout = false;
        }
    }
    else if(line_first_deg >= 360 - check_value)
    {
        if(line_deg >= line_first_deg + check_value - 360 && line_deg <= line_first_deg - check_value)
        {

        }
        else
        {
            line_first_deg = line_deg;
            is_halfout = false;
        }
    }
    else
    {
        if(line_deg >= line_first_deg + check_value || line_deg <= line_first_deg - check_value)
        {

        }
        else
        {
            line_first_deg = line_deg;
            is_halfout = false;
        } 
    }
}

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


        double line_vec_x, line_vec_y;

        for(int i = 0; i < 16; i++)
        {
            if(line_data[i] == 1)
            {
                line_vec_x += cos(radians(line_sensors_deg[i]));
                line_vec_y += sin(radians(line_sensors_deg[i]));
            }
        }

        line_deg = 360 - degrees(atan2(line_vec_y, line_vec_x));

        if(line_deg <= 360)
        {
            line_deg = 360 - line_deg;
        }
        else
        {
            line_deg = 360 - (line_deg - 180) + 180;
        }

        /*
        for(int i = 0; i < 16; i++)
        {
            Serial.print(line_data[i]);
            Serial.print(",");
        }
        Serial.println(line_deg);
        */
    }
    else
    {
        line_deg = -1;
    }

    previous_line_deg = line_deg;
}