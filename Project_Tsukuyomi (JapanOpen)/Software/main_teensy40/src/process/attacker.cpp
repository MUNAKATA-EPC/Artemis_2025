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
        if(ball_deg <= 6 || ball_deg >= 345)
        {
            motor_move(0, 90);
            f_kicker.kick(600);
        }
        else 
        {
            if(ball_dis != 0)
            {             
                if(ball_deg <= 60)
                {
                    motor_move(ball_deg + 45, 60);
                }
                else if(ball_deg <= 140)
                {
                    motor_move(ball_deg + 30, 70 );
                }
                else if(ball_deg <= 180)
                {
                    if(ball_dis > 60)
                    {
                        motor_move(ball_deg + 80, 70 );
                    }
                    else
                    {
                        motor_move(ball_deg, 70 );
                    }
                }
                else if(ball_deg <= 220)
                {
                    if(ball_dis > 60)
                    {
                        motor_move(ball_deg - 80, 70 );
                    }
                    else
                    {
                        motor_move(ball_deg, 70 );
                    }
                    
                }
                else if(ball_deg <= 300)
                {
                    motor_move(ball_deg - 30, 70 );
                }
                else
                {
                    motor_move(ball_deg - 45, 60);
                }
            }
            else
            {
                
                motor_move(ball_deg ,80);
                
            }
        }
    }
}