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
        if(ball_deg <= 25 || ball_deg >= 335)
        {
            int ball_front[2] = {4, 350};
            if(ball_dis >= 220)
            {
                if(ball_deg <= 4 || ball_deg >= 350)
                {
                    motor_move(0, 80);
                    f_kicker.kick(500);
                }
                else
                {
                    if(ball_deg <= 25)
                    {
                        int ball_deg_diff = ball_deg * 1.7;
                        float move_scale = (ball_deg_diff - ball_front[0]) / 20.0;
                        float move_deg = pow(move_scale, 1.5) * ball_deg_diff;
                        motor_move(move_deg, 65);
                    }
                    else if(ball_deg >= 335)
                    {
                        int ball_deg_diff = (360 - ball_deg) * 1.7;
                        float move_scale = (ball_deg_diff - (360 - ball_front[1])) / 20.0;
                        float move_deg = pow(move_scale, 1.5) * ball_deg_diff;
                        motor_move(360 - move_deg, 65);
                    }
                }
            }
            else if(ball_dis >= 30)
            {
                if(ball_deg <= 25)
                {
                    int ball_deg_diff = ball_deg * 1.7;
                    float move_scale = (ball_deg_diff - ball_front[0]) / 20.0;
                    float move_deg = pow(move_scale, 2.0) * ball_deg_diff;
                    motor_move(move_deg, 65);
                }
                else if(ball_deg >= 335)
                {
                    int ball_deg_diff = (360 - ball_deg) * 1.7;
                    float move_scale = (ball_deg_diff - (360 - ball_front[1])) / 25.0;
                    float move_deg = pow(move_scale, 2.0) * ball_deg_diff;
                    motor_move(360 - move_deg, 65);
                }
                else
                {
                    motor_move(ball_deg, 65);
                }
            }
            else
            {
                motor_move(ball_deg, 80);
            }
        }
        else 
        {
            if(ball_dis <= 70 && ball_deg != 0)
            {
                motor_move(ball_deg, 80);

            }
            else
            {
                if(ball_deg <= 60)
                {
                    motor_move(ball_deg + 40, 65);

                }
                else if(ball_deg <= 160)
                {

                    motor_move(ball_deg + 55, 70);
                }
                else if(ball_deg <= 180)
                {
                    motor_move(ball_deg + 70, 75);
                }
                else if(ball_deg <= 200)
                {
                    motor_move(ball_deg - 70, 75);
                }
                else if(ball_deg <= 300)
                {
                    motor_move(ball_deg - 55, 70);
                }
                else
                {
                    motor_move(ball_deg - 40, 65);

                }
            }
        }
    }
}