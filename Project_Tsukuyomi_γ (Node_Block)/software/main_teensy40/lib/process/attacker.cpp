#include <Arduino.h>
#include "attacker.hpp"

#include "sensor_variables.hpp"
#include "timer.hpp"
#include "vector.h"

#include "kicker.hpp"

#include "motor.hpp"
#include "engelline.hpp"

Timer ball_check;

void attacker_init()
{
    

}

void attacker_process(int speed)
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

    if(fcam_goal_blue_deg == 255)
    {
        pid_camera(fcam_goal_blue_deg);
    }
    else
    {
        pid_camera(fcam_goal_blue_deg);
    }

    if(is_line_evacuation())
    {
        motor_move(line_evacuation_deg, 60);
    }
    else
    {
        if(fcam_ball_deg != 255)
        {
            ball_check.stop();
            ball_check.reset();

            if(fcam_ball_deg <= 10 || fcam_ball_deg >= 350)
            {
                motor_move(0, 70);
            }
            else
            {
                if(fcam_ball_deg >= 340)
                {
                    motor_move(fcam_ball_deg - 15, 60);
                }
                else if(fcam_ball_deg >= 315)
                {
                    motor_move(fcam_ball_deg + 70, 60);
                }
                else if(fcam_ball_deg <= 20)
                {
                    motor_move(fcam_ball_deg + 15, 60);
                }
                else if(fcam_ball_deg <= 45)
                {
                    motor_move(fcam_ball_deg - 70, 60);
                }
        
            }
        }
        else if(bcam_ball_deg != 255)
        {
            ball_check.stop();
            ball_check.reset();

            if(bcam_ball_deg >= 200)
            {
                motor_move(bcam_ball_deg - 30, 60);
            }
            else if(bcam_ball_deg >= 180)
            {
                motor_move(bcam_ball_deg - 60, 60);
            }
            else if(bcam_ball_deg >= 160)
            {
                motor_move(bcam_ball_deg + 60, 60);
            }
            else if(bcam_ball_deg >= 135)
            {
                motor_move(bcam_ball_deg + 30, 60);
            }
            
        } 
        else
        {
            if(cam_ball_deg == 500)
            {
                ball_check.start();
                if(ball_check.get_value() < 300)
                {
                    motor_move(180,600);
                }
                else
                {
                    motor_move(180,0);
                }
            }
            else
            {
                
                ball_check.stop();
                ball_check.reset();

                if(cam_ball_deg <= 90)
                {
                    motor_move(cam_ball_deg + 45, 60);
                }
                else if(cam_ball_deg <= 180)
                {
                    motor_move(cam_ball_deg + 30, 60);
                }
                else if(cam_ball_deg <= 270)
                {
                    motor_move(cam_ball_deg - 30, 60);
                }
                else if(cam_ball_deg <= 360)
                {
                    motor_move(cam_ball_deg - 45, 60);
                }
            }
        }
    }
}
