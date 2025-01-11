#include <Arduino.h>
#include "attacker.hpp"

#include "sensor_variables.hpp"
#include "timer.hpp"
#include "vector.h"

#include "kicker.hpp"

#include "motor.hpp"
#include "engelline.hpp"

Timer ball_check;
#define MOTER_SPEED 60

void attacker_init()
{
    

}

void attacker_process(int speed, bool isYellow)
{
    // 【アタッカープログラム構想】
    // A.ボールが見つからなかった場合
    //  自陣ゴール前まで移動。定位置で停止。
    //
    // B.ボールが見つかった場合
    //  B-1. ボールを前方で保持している場合
    //    ゴール方向にシュートをする。
    //
    //  B-2. ボールを後方でシュートしている場合
    //    前方のゴールの空いている方向にマカオシュート。
    //
    //  B-3. ボールがロボット前方に存在する場合
    //    ボールチェイスを行う
    //
    //  B-4. ボールがロボット後方に存在する場合
    //    マカオシュートを行うため、ボールを保持しに行く
    //  
    //  (B-3, B-4はボール方向に向いてもいいかもしれない)
    //
    // C.白線を踏んだ場合
    //   コート内に戻る。

    int fcam_deg, fcam_dis, bcam_deg, bcam_dis;
    if(isYellow)
    {
        fcam_deg = fcam_goal_yellow_deg;
        fcam_dis = fcam_goal_yellow_distance;
        bcam_deg = bcam_goal_blue_deg;
        bcam_dis = bcam_goal_blue_distance;
    }
    else
    {
        fcam_deg = fcam_goal_blue_deg;
        fcam_dis = fcam_goal_blue_distance;
        bcam_deg = bcam_goal_yellow_deg;
        bcam_dis = bcam_goal_yellow_distance;
    }

    if((fcam_deg != 500) && (fcam_ball_deg != 500))
    {
        //pid_gyro();
        pid_camera(fcam_deg);
    }
    else
    {
        pid_gyro();
    }

    if(is_line_evacuation())
    {
        motor_move(line_evacuation_deg, 80);
    }
    else
    {
        if(fcam_ball_deg != 500)
        {
            if(fcam_ball_deg <= 5 || fcam_ball_deg >= 355)
            {
                motor_move(0, speed);
            }
            else 
            {
                if(fcam_ball_deg >= 150)
                {
                    motor_move(fcam_ball_deg, speed);    
                }
                else
                {
                    if(fcam_ball_deg <= 20)
                    {
                        int deg = (fcam_ball_deg) * 0.2;
                        motor_move(deg, speed * 0.9);
                    }
                    else if(fcam_ball_deg >= 340)
                    {
                        int deg = 360 - (360 - fcam_ball_deg) * 0.2;
                        motor_move(deg, speed * 0.9);
                    }
                    else
                    {
                        if(fcam_ball_deg <= 35)
                        {
                            motor_move(fcam_ball_deg + 30, speed * 0.5);
                        }
                        else if(fcam_ball_deg <= 60)
                        {
                            motor_move(fcam_ball_deg + 50, speed * 0.7);
                        }
                        else if(fcam_ball_deg <= 325)
                        {
                            motor_move(fcam_ball_deg - 50, speed * 0.7);
                        }
                        else
                        {
                            motor_move(fcam_ball_deg - 50, speed * 0.5);
                        }
                    }
                }
            }
        }
        else if(bcam_ball_deg != 500)
        {
            if(bcam_ball_distance <= 150)
            {
                if(bcam_ball_deg >= 200)
                {
                    motor_move(bcam_ball_deg - 90, speed);
                }
                else if(bcam_ball_deg >= 180)
                {
                    motor_move(bcam_ball_deg - 90, speed);
                }
                else if(bcam_ball_deg >= 160)
                {
                    motor_move(bcam_ball_deg + 90, speed);
                }
                else if(bcam_ball_deg >= 135)
                {
                    motor_move(bcam_ball_deg + 90, speed);
                }
            }
            else
            {
                if(bcam_ball_deg >= 200)
                {
                    motor_move(bcam_ball_deg - 60, speed);
                }
                else if(bcam_ball_deg >= 180)
                {
                    motor_move(bcam_ball_deg - 80, speed);
                }
                else if(bcam_ball_deg >= 160)
                {
                    motor_move(bcam_ball_deg + 80, speed);
                }
                else if(bcam_ball_deg >= 135)
                {
                    motor_move(bcam_ball_deg + 60, speed);
                }
            }
        } 
        else
        {
            if(cam_ball_deg == 500)
            {
                motor_move(180, speed * 0.7);
            }
            else
            {
                if(cam_ball_distance >= 145)
                {
                    motor_move(cam_ball_deg, speed);
                }
                else
                {
                    if(cam_ball_deg <= 90)
                    {
                        motor_move(cam_ball_deg + 30, speed * 0.8);
                    }
                    else if(cam_ball_deg <= 180)
                    {
                        motor_move(cam_ball_deg + 45, speed);
                    }
                    else if(cam_ball_deg <= 270)
                    {
                        motor_move(cam_ball_deg - 45, speed);
                    }
                    else if(cam_ball_deg <= 360)
                    {
                        motor_move(cam_ball_deg - 30, speed * 0.8);
                    }
                }
            }
        }
    }
}