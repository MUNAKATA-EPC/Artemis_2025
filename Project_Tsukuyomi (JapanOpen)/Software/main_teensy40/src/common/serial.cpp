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

int line_data[16];

void init_serial()
{
    Serial1.begin(115200);
    Serial1.setTimeout(10);

    Serial3.begin(115200);
    Serial3.setTimeout(10);
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

        ball_deg = ball_deg != -1 ? (ball_deg + 5) % 360 : -1;
    }

    int _line_data = -1;
    
    while(Serial3.available() > 0)
    {
        _line_data = Serial3.readStringUntil('\n').toInt();
        
        if(_line_data > pow(2, 16))
        {
            return;
        }

        for(int i = 15; i >= 0; i--)
        {
            if(_line_data - pow(2, i) >= 0)
            {
                _line_data -= pow(2, i);
                line_data[i] = 1;
            }
            else
            {
                line_data[i] = 0;
            }
        }
    }

    Serial.print(ball_deg);
    Serial.print(",");
    Serial.println(ball_dis);
        
    // for(int i = 0; i < 16; i++)
    // {
    //     Serial.print(line_data[i]);
    //     Serial.print(",");
    // }
    // Serial.print("\n");
}