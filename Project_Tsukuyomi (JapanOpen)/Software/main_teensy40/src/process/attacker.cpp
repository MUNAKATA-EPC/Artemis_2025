#include "attacker.hpp"

#include "common/sensors_variables.hpp"
#include "common/motor.hpp"
#include "common/vector.hpp"

#include "process/engelline.hpp"

int ball_front_near[2] = {15, 345};
int ball_front[2] = {10, 350};
int ball_front_far[2] = {10, 350};
float front_diff = 60;

bool is_ball_front_far()
{
    return ball_deg != -1 && (ball_dis <= 60 && (ball_deg <= ball_front_far[0] || ball_deg >= ball_front_far[1]));
}

bool is_ball_front()
{
    return ball_deg != -1 && ((ball_dis > 60 && (ball_deg <= ball_front[0] || ball_deg >= ball_front[1])) ||
            ( ball_dis > 155 && (ball_deg <= ball_front_near[0] || ball_deg >= ball_front_near[1])) || is_ball_front_far() == true);
}
bool is_ball_hold()
{
    return ball_deg != -1 && (is_ball_front() == true && ball_dis >= 222);
}

void init_attacker()
{

}

void process_attacker(bool is_yellow, int speed)
{
    int our_goal_deg;
    int our_goal_dis;
    int ene_goal_deg;
    int ene_goal_dis;

    if(is_yellow)
    {
        our_goal_deg = ygoal_deg;
        our_goal_dis = ygoal_dis;
        ene_goal_deg = bgoal_deg;
        ene_goal_dis = bgoal_dis;
    }
    else
    {
        our_goal_deg = bgoal_deg;
        our_goal_dis = bgoal_dis;
        ene_goal_deg = ygoal_deg;
        ene_goal_dis = ygoal_dis;
    }
    
    //if(is_ball_hold())
    {
        if(our_goal_deg != -1)
        {
            pid_camera(our_goal_deg);
        }
        else
        {
            pid_gyro();
        }
    }
    // else
    // {
    //     pid_gyro();
    // }

    motor_set_bldc(10, 0);
            
    if(is_line_evacuation())
    {
        tone(2, 4000, 10);

        motor_move(line_evacuation_deg + 180, 100);
        
        if(ball_deg <= 30 || ball_deg >= 330)
        {
            if(is_ball_hold())
            {
                f_kicker.kick(100);
            }
        }
    }
    else
    {
        if(ball_deg == -1)
        {
            motor_move(0, 0);
        }
        else
        {
            if(is_ball_front() == true || is_ball_front_far() == true)
            {
                if(is_ball_hold() == true) //近距離
                {
                    motor_move(0, speed);
                    f_kicker.kick(200, 200);
                }
                else if(ball_dis >= 100)
                {
                    motor_move(0, speed * 0.9);
                }
                else
                {
                    motor_move(0, 90);
                }
            }
            else if((ball_deg <= ball_front[0] + front_diff || ball_deg >= ball_front[1] - front_diff) && (ball_dis <= 180))
            {
                /*if((ball_deg <= 50 || ball_deg >= 310) && ball_deg <= 170)
                {
                    if(ball_deg <= 50)
                    {
                        if(ball_deg < 8)
                        {
                            motor_move(0, 70);
                        }
                        else
                        {
                            int move_speed = ball_dis >= 160 ? 70 : 75;
                            motor_move(ball_deg + ball_deg * 1.65, move_speed);
                        }
                    }
                    else
                    {
                        if(ball_deg > 348)
                        {
                            motor_move(0, 70);
                        }
                        else
                        {
                            int move_speed = ball_dis >= 160 ? 70 : 75;
                            int ball_deg_ = 360 - ball_deg;
                            ball_deg_ = ball_deg_ * 1.65;
                            ball_deg_ = 360 - ball_deg_;
                            motor_move(ball_deg_, move_speed);
                        }
                    }
                }
                else*/
                {
                    
                    int ball_vec_dis = 240 - ball_dis;
                    int ball_vec_dir = ball_deg;
                    int ball_x = cos(radians(ball_vec_dir)) * ball_vec_dis;
                    int ball_y = sin(radians(ball_vec_dir)) * ball_vec_dis;
                    int ball_deg_from_front = degrees(atan2(ball_y, abs(ball_x - 90)));
                    int move_speed = ball_dis >= 30 ? 70 : 85;

                    motor_move(ball_deg_from_front, move_speed);

                }
            }
            else 
            {
                //ベクトルを用いて処理
                Vector vec_to_ball(radians(ball_deg), ball_dis >= 170 ? -1 : 1);
                Vector vec_to_tan_ball(radians(ball_deg <= 180 ? (ball_deg + 90) : (ball_deg - 90)), ball_dis / 90.0);
    
                vec_to_ball.add(vec_to_tan_ball);
    
                int motor_speed = speed;
                int move_speed = motor_speed;
    
                if(ball_deg <= 90)
                {
                    float speed_scale = (ball_deg + 140) / 230.0;
                    move_speed = pow(speed_scale, 1.5) * motor_speed;
                }
                else if(ball_deg >= 270)
                {
                    float speed_scale = (360 - ball_deg + 140) / 230.0;
                    move_speed = pow(speed_scale, 1.5) * motor_speed;
                }
    
                motor_move(vec_to_ball.get_deg(), move_speed);            
            }
        }
    }
}