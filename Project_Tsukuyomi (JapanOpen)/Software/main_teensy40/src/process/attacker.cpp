#include <Arduino.h>
#include "attacker.hpp"

#include "common/sensors_variables.hpp"
#include "common/motor.hpp"

void init_attacker()
{

}

void process_attacker()
{
    pid_gyro();
    
    if(ball_deg == -1)
    {
        motor_move(0, 0);
    }
    else
    {
        if(ball_deg <= 10 || ball_deg >= 350)
        {
            motor_move(0, 80);

            f_kicker.kick(1000);
        }
        else if(ball_deg <= 20)
        {
            motor_move(20, 70);
        }
        else if(ball_deg >= 340)
        {
            motor_move(320, 70);
        }
        else
        {
            if(ball_deg <= 60)
            {
                motor_move(ball_deg + 50, 70);
            }
            else if(ball_deg <= 180)
            {
                motor_move(ball_deg + 50, 90);
            }
            else if(ball_deg <= 300)
            {
                motor_move(ball_deg + 50, 90);
            }
            else
            {
                motor_move(ball_deg - 50, 70);
            }
        }
    }
}