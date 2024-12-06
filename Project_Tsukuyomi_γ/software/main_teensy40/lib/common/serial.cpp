#include <Arduino.h>
#include "serial.hpp"
#include "sensor_variables.hpp"

// front camera data
int fcam_ball_deg;
int fcam_ball_distance;
int fcam_goal_yellow_deg;
int fcam_goal_yellow_distance;
int fcam_goal_blue_deg;
int fcam_goal_blue_distance;

// gyro_sensor
int gyro_deg;

// line sensor
int line_value;
int line_data[16];

int cam1_data;
int cam2_data;
int cam3_data;

void serials_init()
{
    //Omnidirectional Camera
    Serial2.begin(115200);
    Serial2.setTimeout(10);

    //Front Camera
    Serial3.begin(115200);
    Serial3.setTimeout(10);

    //Back camera
    Serial4.begin(115200);
    Serial4.setTimeout(10);

    //Line
    Serial5.begin(115200);
    Serial5.setTimeout(10);
}

void serials_process()
{
    //Omnidirectional Camera
    if(Serial2.available() > 0)
    {
    }

    //Front Camera
    if(Serial3.available() > 0)
    {
        fcam_ball_deg               = Serial3.readStringUntil('a').toInt();
        fcam_ball_distance          = Serial3.readStringUntil('b').toInt();
        fcam_goal_yellow_deg        = Serial3.readStringUntil('c').toInt();
        fcam_goal_yellow_distance   = Serial3.readStringUntil('d').toInt();
        fcam_goal_blue_deg          = Serial3.readStringUntil('e').toInt();
        fcam_goal_blue_distance     = Serial3.readStringUntil('f').toInt();

        fcam_ball_deg = fcam_ball_deg == 255 ? 255 : (fcam_ball_deg - 45 + 360) % 360;
        fcam_goal_yellow_deg = fcam_goal_yellow_deg == 255 ? 255 : (-fcam_goal_yellow_deg + 360) % 360;
    }

    Serial.println(fcam_goal_yellow_distance);
    
    //Back camera
    if(Serial4.available() > 0)
    {
    }

    //Line
    while(Serial5.available() > 0)
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