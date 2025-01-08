#include <Arduino.h>
#include "serial.hpp"
#include "sensor_variables.hpp"

// omnidirectional camera data
int cam_ball_deg;
int cam_ball_distance;
int cam_goal_yellow_deg;
int cam_goal_yellow_distance;
int cam_goal_blue_deg;
int cam_goal_blue_distance;

// front camera data
int fcam_ball_deg;
int fcam_ball_distance;
int fcam_goal_yellow_deg;
int fcam_goal_yellow_distance;
int fcam_goal_blue_deg;
int fcam_goal_blue_distance;

// back camera data
int bcam_ball_deg;
int bcam_ball_distance;
int bcam_goal_yellow_deg;
int bcam_goal_yellow_distance;
int bcam_goal_blue_deg;
int bcam_goal_blue_distance;

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
        cam_ball_deg                = Serial2.readStringUntil('a').toInt();
        cam_ball_distance           = Serial2.readStringUntil('b').toInt();
        cam_goal_yellow_deg         = Serial2.readStringUntil('c').toInt();
        cam_goal_yellow_distance    = Serial2.readStringUntil('d').toInt();
        cam_goal_blue_deg           = Serial2.readStringUntil('e').toInt();
        cam_goal_blue_distance      = Serial2.readStringUntil('f').toInt();
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

        fcam_ball_deg               = fcam_ball_deg == 500          ? 500 : (fcam_ball_deg - 45 + 360) % 360;
        fcam_goal_yellow_deg        = fcam_goal_yellow_deg == 500   ? 500 : (-fcam_goal_yellow_deg + 360) % 360;
        fcam_goal_blue_deg          = fcam_goal_blue_deg == 500     ? 500 : (-fcam_goal_blue_deg + 360) % 360;
    }
    
    //Back camera
    if(Serial4.available() > 0)
    {
        bcam_ball_deg               = Serial4.readStringUntil('a').toInt();
        bcam_ball_distance          = Serial4.readStringUntil('b').toInt();
        bcam_goal_yellow_deg        = Serial4.readStringUntil('b').toInt();
        bcam_goal_yellow_distance   = Serial4.readStringUntil('d').toInt();
        bcam_goal_blue_deg          = Serial4.readStringUntil('e').toInt();
        bcam_goal_blue_distance     = Serial4.readStringUntil('f').toInt();

        bcam_ball_deg               = bcam_ball_deg == 500          ? 500 : (bcam_ball_deg + 135)% 360;
        bcam_goal_yellow_deg        = bcam_goal_yellow_deg == 500   ? 500 : (-bcam_goal_yellow_deg + 180) % 360;
        bcam_goal_blue_deg          = bcam_goal_blue_deg == 500     ? 500 : (-bcam_goal_blue_deg + 180) % 360;

    }

    //Line
    while(Serial5.available() > 0)
    {
        int line_value = Serial5.readStringUntil('\n').toInt();
        Serial.print(line_value);
        Serial.print(" | ");
    
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

    for(int i = 0; i < 16; i++)
    {
        Serial.print(line_data[i]);
        Serial.print(",");
    }
    Serial.println();
}