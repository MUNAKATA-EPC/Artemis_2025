#include <Arduino.h>
#include "attacker.hpp"

#include "common/sensors_variables.hpp"
#include "common/motor.hpp"

void init_attacker()
{

}

void process_attacker(int speed)
{
    
    if(ygoal_deg != -1)
    {
        pid_camera(ygoal_deg);
    }
    else
    {
        pid_gyro();
    }

    

    
    /*if(ball_deg == -1)
    {
        motor_move(0, 0);
    }
    else
    {
        if(ball_deg <= 8 || ball_deg >= 350)
        {
            motor_move(0, speed);
        }
        else 
        {
            if(ball_deg <= 45)
            {
                motor_move(ball_deg + 20, speed * 0.8);
            }
            else if(ball_deg <= 180)
            {
                motor_move(ball_deg + 45, speed );
            }
            else if(ball_deg <= 315)
            {
                motor_move(ball_deg - 45, speed );
            }
            else
            {
                motor_move(ball_deg - 20, speed * 0.8);
            }
            
        }
    }*/
    motor_move(0, 0);
}