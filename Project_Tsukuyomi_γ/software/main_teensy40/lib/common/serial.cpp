#include <Arduino.h>
#include "serial.hpp"
#include "sensor_variables.hpp"

// line sensor
int line_value;
int line_data[16];

int cam1_data;
int cam2_data;
int cam3_data;

void serials_init()
{
    //Cameras
    Serial2.begin(115200);
    Serial3.begin(115200);
    Serial3.setTimeout(10);
    Serial4.begin(115200);

    //Line
    Serial5.begin(115200);
}

void serials_process()
{
    if(Serial3.available() > 0)
    {
        Serial.println(Serial3.readString());
    }

    if(Serial5.available() > 0)
    {
        int line_value = Serial5.readStringUntil('\n').toInt();
    
        for(int i = 15; i >= 0; i--)
        {
            if(line_value - pow(2, i) >= 0)
            {
                line_value -= pow(2, i);
                line_data[i] = 1;
            }
            else
            {
                line_data[i] = 0;
            }
        }
    }
}