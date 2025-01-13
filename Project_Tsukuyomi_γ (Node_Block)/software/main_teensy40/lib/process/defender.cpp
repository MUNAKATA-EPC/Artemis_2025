#include <Arduino.h>
#include "attacker.hpp"

#include "sensor_variables.hpp"
#include "timer.hpp"
#include "vector.h"

#include "kicker.hpp"

#include "motor.hpp"
#include "engelline.hpp"

/// ディフェンダーの動作モードを定義します。
/// 0 : ライントレースモード

int defender_mode;

int _previous_line_deg;

struct Line_Group
{
    int start_index;
    int end_index;
    int count;
};

void defender_init()
{

}

void defender_process(int speed)
{
    pid_gyro();

    if(defender_mode == 0)
    {
        //まずはラインセンサーの反応群を取得

        int count_of_detected_line_group = 0;
        Line_Group line_detected_groups[4];

        for(int i = 0; i < 4; i++)
        {       
            line_detected_groups[i].start_index = -100;
            line_detected_groups[i].end_index = -1;
            line_detected_groups[i].count = -1;
        }

        for(int i = 0; i < 16; i++)
        {
            if(line_data[i] == 1)
            {   
                //一つ後が反応していないのであれば、それは反応ラインの群の最後である。
                if(line_data[(i + 1) % 16] == 0)
                {
                    if(line_data[(i - 1 + 16) % 16] == 1)
                    {
                        if(i == 0) continue;
                        line_detected_groups[count_of_detected_line_group].end_index = i;
                        line_detected_groups[count_of_detected_line_group].count++;
                        count_of_detected_line_group++; //次の群に移動
                    }
                    else
                    { 
                        line_detected_groups[count_of_detected_line_group].start_index = i;
                        line_detected_groups[count_of_detected_line_group].end_index = i;
                        line_detected_groups[count_of_detected_line_group].count = 1;
                        count_of_detected_line_group++; //次の群に移動
                    }
                }
                //一つ前が反応していないのであれば、それは反応ラインの群の最初である。
                else if(line_data[(i - 1 + 16) % 16] == 0)
                {
                    line_detected_groups[count_of_detected_line_group].start_index = i;
                    line_detected_groups[count_of_detected_line_group].count = 0;
                    int temp_line_count = i;

                    while(line_data[(temp_line_count + 1) % 16] == 1)
                    {
                        line_detected_groups[count_of_detected_line_group].count++;
                        temp_line_count++;
                    }

                    i = temp_line_count - 1;

                    //もしインデックスが15より大きかったら、最後のライン群の追加ができないのでここで追加。
                    if(i >= 15)
                    {
                        line_detected_groups[count_of_detected_line_group].end_index = i % 16;
                        line_detected_groups[count_of_detected_line_group].count++;
                        count_of_detected_line_group++; //次の群に移動
                    }
                }
                //一つ前が反応していて、かつインデックスが1だったら、処理がめんどいから後回し。
                else if(line_data[(i - 1 + 16) % 16] == 1)
                {
                    int temp_line_count = 0;
                    if(i == 0)
                    {
                        while(line_data[temp_line_count] == 1)
                        {
                            temp_line_count++;
                        }

                        i = temp_line_count;
                    }
                }
            }
        }

        /*
        for(int i = 0; i < count_of_detected_line_group; i++)
        {
            Serial.print(line_detected_groups[i].start_index);
            Serial.print(",");
            Serial.print(line_detected_groups[i].end_index);
            Serial.print(",");
            Serial.print(line_detected_groups[i].count);
            Serial.print(" | ");
        }
        if(count_of_detected_line_group > 0)
        {
            for(int i = 0; i < 16; i++)
            {
                Serial.print(",");
                Serial.print(line_data[i]);
            }
            Serial.println();
        }*/

        if(is_line_detected())
        {
            //群が1つだけだったら
            if(count_of_detected_line_group == 1)
            {
                int move_deg = (line_detected_groups[0].start_index + line_detected_groups[0].end_index) / 2.0 * 22.5;
                motor_move((move_deg + 180) % 360, 50);
            }
            //群が2つだったら
            else if(count_of_detected_line_group == 2)
            {
                //取り合えず2つの線分を対称に割る線分を作る
                int degrees_of_two_lines[2] = { ((line_detected_groups[0].start_index + line_detected_groups[0].count / 2) % 16) * 22.5,
                                                ((line_detected_groups[1].start_index + line_detected_groups[1].count / 2) % 16) * 22.5 };
                int tan_of_two_lines[2]     = { (degrees_of_two_lines[0] + degrees_of_two_lines[1]) / 2, 
                                                ((degrees_of_two_lines[0] + degrees_of_two_lines[1]) / 2 + 180) % 360 };

                //それに対して垂直な線分を作ると、それが範囲検知の角度になる。
                int ball_detection_degrees[2]   =   {(tan_of_two_lines[0] + 90) % 360, (tan_of_two_lines[0] + 270) % 360};
                
                //全カメラのボールの角度を考慮した角度を取得
                int ball_deg_from_allcam = -1;

                if(fcam_ball_deg != 500)
                {
                    ball_deg_from_allcam = fcam_ball_deg;
                }
                else if(bcam_ball_deg != 500)
                {
                    ball_deg_from_allcam = bcam_ball_deg;
                }
                else if(cam_ball_deg != 500)
                {
                    ball_deg_from_allcam = cam_ball_deg;
                } 

                Serial.print(ball_detection_degrees[0]);
                Serial.print(",");
                Serial.print(ball_detection_degrees[1]);
                Serial.print(",");
                Serial.print(degrees_of_two_lines[0]);
                Serial.print(",");
                Serial.print(degrees_of_two_lines[1]);
                Serial.print(",");
                Serial.print(ball_deg_from_allcam);
                Serial.print(",");

                int speed = (ball_deg_from_allcam <= 20 || ball_deg_from_allcam >= 340) ? 40 : 80;
            
                if(ball_deg_from_allcam != -1)
                {
                    if(bcam_goal_yellow_deg <= 160)
                    {
                        motor_move(270, 40);
                    }
                    else if(bcam_goal_yellow_deg >= 200)
                    {
                        motor_move(90, 40);
                    }
                    else if(is_exist_deg_value_in_range(ball_deg_from_allcam, ball_detection_degrees[0], 88))
                    {
                        Serial.print("A");
                        if(is_exist_deg_value_in_range(degrees_of_two_lines[0], ball_detection_degrees[0], 90))
                        {
                            motor_move((degrees_of_two_lines[1] + 180) % 360, speed);
                            Serial.print("A");
                        }
                        else if(is_exist_deg_value_in_range(degrees_of_two_lines[1], ball_detection_degrees[0], 90))
                        {
                            motor_move((degrees_of_two_lines[0] + 180) % 360, speed);
                            Serial.print("B");
                        }
                        Serial.println("");
                    }
                    else if(is_exist_deg_value_in_range(ball_deg_from_allcam, ball_detection_degrees[1], 88))
                    {
                        Serial.print("B");
                        if(is_exist_deg_value_in_range(degrees_of_two_lines[0], ball_detection_degrees[1], 90))
                        {
                            motor_move((degrees_of_two_lines[1] + 180) % 360, speed);
                            Serial.print("A");
                        }
                        else if(is_exist_deg_value_in_range(degrees_of_two_lines[1], ball_detection_degrees[1], 90))
                        {
                            motor_move((degrees_of_two_lines[0] + 180) % 360, speed);
                            Serial.print("B");
                        }
                        Serial.println("");
                    }
                    else
                    {
                        motor_move(0, 0);
                    }
                    Serial.println("");
                }
                else
                {
                    motor_move(0, 0);
                }
            }
            _previous_line_deg = line_deg;
        }
        else
        {
            motor_move(_previous_line_deg + 180, 30);
        }
    }
}
