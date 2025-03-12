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
        ball_deg = Serial1.readStringUntil('a').toInt();
        ball_dis = Serial1.readStringUntil('b').toInt();
        ygoal_deg = Serial1.readStringUntil('c').toInt();
        ygoal_dis = Serial1.readStringUntil('d').toInt();
        bgoal_deg = Serial1.readStringUntil('e').toInt();
        bgoal_dis = Serial1.readStringUntil('f').toInt();
    }

    Serial.print(ball_deg);
    Serial.print(" ");
    Serial.println(ball_dis);
}