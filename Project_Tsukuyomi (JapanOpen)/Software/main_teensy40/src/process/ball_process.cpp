#include <Arduino.h>
#include "ball_process.hpp"

#include "common/sensors_variables.hpp"
#include "common/timer.hpp"
#include "common/vector.hpp"

double now_ball_deg;
double now_ball_dis;
double previous_ball_deg;
double previous_ball_dis;

int moving_diff_ball_x;
int moving_diff_ball_y;

Vector vec_ball_speed(0, 0);
Timer measuring_timer;


void init_ball_process()
{

}

void process_ball_process()
{
    //速度算出用タイマーの設定
    measuring_timer.tick();
    measuring_timer.start();

    if((previous_ball_deg == -1 && ball_deg != -1) || (previous_ball_deg != -1 && ball_deg == -1))
    {
        previous_ball_deg = ball_deg;
        previous_ball_dis = 240 - ball_dis;
        return;
    }

    now_ball_deg = ball_deg;
    now_ball_dis = 240 - ball_dis;

    //まずは前回のボールの位置と今回のボールの位置を算出する。
    double now_ball_x = cos(radians(now_ball_deg)) * now_ball_dis * 10;
    double now_ball_y = sin(radians(now_ball_deg)) * now_ball_dis * 10;
    double previous_ball_x = cos(radians(previous_ball_deg)) * previous_ball_dis * 10;
    double previous_ball_y = sin(radians(previous_ball_deg)) * previous_ball_dis * 10;

    //次に、移動した座標を計算
    int _ball_moving_x = previous_ball_x - now_ball_x;
    int _ball_moving_y = previous_ball_y - now_ball_y;

    //0.1sごとに速度を計算
    if(measuring_timer.get_value() <= 100)
    {
        moving_diff_ball_x += _ball_moving_x;
        moving_diff_ball_y += _ball_moving_y;
    }
    else
    {   
        float vec_rad = atan2(moving_diff_ball_y, moving_diff_ball_x);
        float vec_size = sqrt(pow(moving_diff_ball_x, 2) + pow(moving_diff_ball_y, 2));

        if(vec_size == 0)
        {
            vec_ball_speed.set(0, 0);
        }
        else
        {
            vec_ball_speed.set(vec_rad, vec_size);
        }
        
        // Serial.print(vec_ball_speed.get_deg());
        // Serial.print(":");
        // Serial.println(vec_ball_speed.get_size());
    
        moving_diff_ball_x = 0.00;
        moving_diff_ball_y = 0.00;
        measuring_timer.reset();
    }
    
    previous_ball_deg = now_ball_deg;
    previous_ball_dis = now_ball_dis;
}

Vector get_ball_speed()
{
    return vec_ball_speed;
}