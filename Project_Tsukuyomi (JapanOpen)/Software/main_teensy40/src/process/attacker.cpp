#include <Arduino.h>
#include "attacker.hpp"

#include "common/sensors_variables.hpp"
#include "common/motor.hpp"
#include "common/vector.hpp"

int ball_front[2] = {7, 353};

bool is_ball_front()
{
    return ball_deg <= ball_front[0] || ball_deg >= ball_front[1];
}

bool is_ball_hold()
{
    return is_ball_front() && ball_dis >= 220;
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
        //ボール中心角度のずれを修正
        ball_deg = (ball_deg + 3) % 360;
    
        if(is_ball_front())
        {
            if(ball_dis >= 220) //近距離
            {
                motor_move(0, 90);
                f_kicker.kick(500, 200);
            }
            else if(ball_dis >= 30)
            { 
                motor_move(ball_deg, 70);
            }
            else
            {
                motor_move(ball_deg, 90);
            }
        }
        else if(ball_deg <= ball_front[0] + 20 || ball_deg >= ball_front[1] - 20)
        {
            if(ball_deg <= ball_front[0] + 20)
            {
                int ball_deg_diff = (ball_deg) * 1.6;
                float move_scale = (ball_deg_diff - ball_front[0]) / 20.0;
                float move_deg = move_scale * move_scale * ball_deg_diff;
                motor_move(move_deg, 65);
            }
            else if(ball_deg >= ball_front[1] - 20)
            {
                int ball_deg_diff = (360 - ball_deg) * 1.6;
                float move_scale = (ball_deg_diff - (360 - ball_front[1])) / 20.0;
                float move_deg = move_scale * move_scale * ball_deg_diff;
                motor_move(360 - move_deg, 65);
            }
        }
        else 
        {
            //ベクトルを用いて処理
            Vector vec_to_ball(radians(ball_deg), ball_dis >= 160 ? -1 : 1);
            Vector vec_to_tan_ball(radians(ball_deg <= 180 ? (ball_deg + 90) : (ball_deg - 90)), ball_dis / 95.0);

            vec_to_ball.add(vec_to_tan_ball);

            int motor_speed = 95;
            int move_speed = motor_speed;

            if(ball_deg <= 90)
            {
                float speed_scale = (ball_deg + 160) / 250.0;
                move_speed = pow(speed_scale, 2) * motor_speed;
            }
            else if(ball_deg >= 270)
            {
                float speed_scale = (360 - ball_deg + 160) / 250.0 ;
                move_speed = pow(speed_scale, 2) * motor_speed;
            }

            motor_move(vec_to_ball.get_deg(), move_speed);            
        }
    }
}