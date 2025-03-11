#include <Arduino.h>
#include "attacker.hpp"

#include "common/sensors_variables.hpp"
#include "common/motor.hpp"

void init_attacker()
{

}

void process_attacker(int speed)
{
    


    pid_gyro();

    
    if(ball_deg == -1)
    {
        motor_move(0, 0);
    }
    else
    {
        if(ball_deg <= 5 || ball_deg >= 345)
        {
            motor_move(0, speed);
            f_kicker.kick(1000);
        }
        else 
        {
            if(ball_deg <= 60)
            {
                motor_move(ball_deg + 30, 90);
            }
            else if(ball_deg <= 180)
            {
                motor_move(ball_deg + 60, speed );
            }
            else if(ball_deg <= 300)
            {
                motor_move(ball_deg - 60, speed );
            }
            else
            {
                motor_move(ball_deg - 30, 90);
            }
            
        }
    }
}