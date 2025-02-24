#include <Arduino.h>
#include "serial.hpp"
#include "sensors_variables.hpp"

int gyro_deg;

int ball_deg;
int ball_dis;
int ygoal_deg;
int ygoal_dis;
int bgoal_deg;
int bgoal_dis;

void init_serial()
{
    Serial1.begin(9600);
    Serial1.setTimeout(10);
}

void process_serial()
{
    if(Serial1.available() > 0)
    {
        Serial.println(Serial1.readStringUntil('a').toInt());
    }
}