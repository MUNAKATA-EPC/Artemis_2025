#include <Arduino.h>
#include "attacker.hpp"

#include "common/sensors_variables.hpp"
#include "common/motor.hpp"
#include "common/vector.hpp"

int ball_front[2] = {8, 352};
int ball_front_far[2] = {4, 355};
float front_diff = 70;

bool is_ball_front()
{
    return ball_dis > 30 && (ball_deg <= ball_front[0] || ball_deg >= ball_front[1]);
}
bool is_ball_front_far()
{
    return ball_dis <= 30 && (ball_deg <= ball_front_far[0] || ball_deg >= ball_front_far[1]);
}

bool is_ball_hold()
{
    return is_ball_front() && ball_dis >= 216;
}

void init_attacker()
{

}

void process_attacker(int speed)
{
    if(is_ball_hold())
    {
        pid_camera(ygoal_deg);
    }
    else
    {
        pid_gyro();
    }

    if(ball_deg == -1)
    {
        motor_move(0, 0);
    }
    else
    {
        if(is_ball_front() == true || is_ball_front_far() == true)
        {
            if(is_ball_hold()) //近距離
            {
                motor_move(0, 90);
                f_kicker.kick(500, 200);
            }
            else if(ball_dis >= 100)
            { 
                motor_move(0, 70);
            }
            else
            {
                motor_move(0, 80);
            }
        }
        else if((ball_deg <= ball_front[0] + front_diff || ball_deg >= ball_front[1] - front_diff) && (ball_dis >= 100))
        {
            if(ball_dis >= 140)
            {
                if(ball_deg <= ball_front[0] + front_diff)
                {
                    int ball_deg_diff = ball_deg * pow(abs(sin(radians(ball_deg))), 1.4) * 5.5;
                    motor_move(ball_deg_diff, 65);
                }
                else if(ball_deg >= ball_front[1] - front_diff)
                {
                    int ball_deg_diff = (360 - ball_deg) * pow(abs(sin(radians(ball_deg))), 1.4) * 5.5;
                    motor_move(360 - ball_deg_diff, 65);
                }
            }
            else
            {
                motor_move(ball_deg, 75);
            }
        }
        else 
        {
            //ベクトルを用いて処理
            Vector vec_to_ball(radians(ball_deg), ball_dis >= 140 ? -1 : 1);
            Vector vec_to_tan_ball(radians(ball_deg <= 180 ? (ball_deg + 90) : (ball_deg - 90)), ball_dis / 75.0);

            vec_to_ball.add(vec_to_tan_ball);

            int motor_speed = 95;
            int move_speed = motor_speed;

            if(ball_deg <= 90)
            {
                float speed_scale = (ball_deg + 160) / 250.0;
                move_speed = pow(speed_scale, 1.2) * motor_speed;
            }
            else if(ball_deg >= 270)
            {
                float speed_scale = (360 - ball_deg + 160) / 250.0;
                move_speed = pow(speed_scale, 1.2) * motor_speed;
            }

            motor_move(vec_to_ball.get_deg(), move_speed);            
        }
    }
}