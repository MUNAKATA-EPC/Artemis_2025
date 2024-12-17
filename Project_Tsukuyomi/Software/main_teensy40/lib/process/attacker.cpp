#include <Arduino.h>
#include "attacker.hpp"

#include "sensor_variables.hpp"
#include "timer.hpp"
#include "vector.h"

#include "kicker.hpp"

#include "motor.hpp"
#include "engelline.hpp"

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

    if((fcam_goal_blue_deg == 255 ) || fcam_ball_deg == 255)
    {
        pid_gyro();
    }
    else
    {
        pid_camera(fcam_goal_blue_deg);
    }


    if(is_line_evacuation())
    {
        motor_move(line_evacuation_deg, 100);
    }
    else
    {
        if(fcam_ball_deg != 255)
        {
            if(fcam_ball_deg <= 5 || fcam_ball_deg >= 355)
            {
                motor_move(0, 80);
            }
            else
            {
                if(fcam_ball_deg <= 15)
                {
                    motor_move(fcam_ball_deg, 80);
                }
                else if(fcam_ball_deg <= 50)
                {
                    motor_move(fcam_ball_deg + 30, 50);
                }
                else if(fcam_ball_deg <= 90)
                {
                    motor_move(fcam_ball_deg + 70, 50);
                }
                else if(fcam_ball_deg >= 345)
                {
                    motor_move(fcam_ball_deg, 80);
                }
                else if(fcam_ball_deg <= 310)
                {
                    motor_move(fcam_ball_deg - 30, 50);
                }
                else if(fcam_ball_deg >= 270)
                {
                    motor_move(fcam_ball_deg - 70, 50);
                }
            }
        }
        else
        {
            if(cam_ball_deg != 500)
            {
                if(cam_ball_deg <= 90)
                {
                    motor_move(cam_ball_deg + 45, 80);
                }
                else if(cam_ball_deg <= 180)
                {
                    motor_move(cam_ball_deg + 30, 80);
                }
                else if(cam_ball_deg <= 270)
                {
                    motor_move(cam_ball_deg - 30, 80);
                }
                else if(cam_ball_deg <= 360)
                {
                    motor_move(cam_ball_deg - 45, 80);
                }
            }
            else
            {
                motor_move(180,0);
            }
           
        }
    }
}
