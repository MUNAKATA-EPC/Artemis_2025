#include <Arduino.h>
#include "defender.hpp"

#include "common/sensors_variables.hpp"
#include "common/timer.hpp"
#include "common/motor.hpp"
#include "common/vector.hpp"

#include "process/engelline.hpp"
#include "process/ball_process.hpp"

struct Line_Group
{
    int start_index;
    int end_index;
    int count;
};

int defender_mode = 0;
int goalkeep_mode = 0;

bool is_fast_coming = false;
int keeper_previous_line_deg;

void init_defender()
{

}

void process_defender()
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
                //一つ前が反応していないのであれば、それは反応ラインの群の最初である。
                if(line_data[(i - 1 + 16) % 16] == 0)
                {
                    line_detected_groups[count_of_detected_line_group].start_index = i;
                    line_detected_groups[count_of_detected_line_group].count = 0;
                    int temp_line_count = i;

                    while(line_data[(temp_line_count + 1) % 16] == 1)
                    {
                        line_detected_groups[count_of_detected_line_group].count++;
                        temp_line_count++;
                    }

                    i = temp_line_count;

                    line_detected_groups[count_of_detected_line_group].end_index = i % 16;
                    line_detected_groups[count_of_detected_line_group].count++;
                    count_of_detected_line_group++; //次の群に移動
                }
            }
        }

        if(is_line_detected())
        {
            if(count_of_detected_line_group == 1)
            {
                Vector start_line_vec = Vector(radians(line_detected_groups[0].start_index * 22.5), 1);
                Vector end_line_vec = Vector(radians(line_detected_groups[0].end_index * 22.5), 1);
                start_line_vec.add(end_line_vec);

                motor_move(start_line_vec.get_deg(), 50);
            }
            else if(count_of_detected_line_group == 2)
            {
                //ゴールの反対側にボールがあったら止まる
                //[0]が[1]より小さくなるように調整
                int stop_base_deg[2] = {ygoal_deg, (ygoal_deg + 180) % 360};

                if(stop_base_deg[0] > stop_base_deg[1])
                {
                    stop_base_deg[0] = (ygoal_deg + 180) % 360;
                    stop_base_deg[1] = ygoal_deg;
                }

                //2つの群の角度を計算
                Vector first_line_start_vec = Vector(radians(line_detected_groups[0].start_index * 22.5), 1);
                Vector first_line_end_vec = Vector(radians(line_detected_groups[0].end_index * 22.5), 1);
                Vector second_line_start_vec = Vector(radians(line_detected_groups[1].start_index * 22.5), 1);
                Vector second_line_end_vec = Vector(radians(line_detected_groups[1].end_index * 22.5), 1);
                
                first_line_start_vec.add(first_line_end_vec);
                second_line_start_vec.add(second_line_end_vec);

                int first_line_deg = first_line_start_vec.get_deg() > 0 ? first_line_start_vec.get_deg() : 360 + first_line_start_vec.get_deg();
                int second_line_deg = second_line_start_vec.get_deg() > 0 ? second_line_start_vec.get_deg() : 360 + second_line_start_vec.get_deg();

                //真横に動きやすくする
                if(is_exist_deg_value_in_range(first_line_deg, 90, 30))
                {
                    first_line_deg = 90;
                }
                else if(is_exist_deg_value_in_range(first_line_deg, 270, 30))
                {
                    first_line_deg = 270;
                }

                if(is_exist_deg_value_in_range(second_line_deg, 90, 30))
                {
                    second_line_deg = 90;
                }
                else if(is_exist_deg_value_in_range(second_line_deg, 270, 30))
                {
                    second_line_deg = 270;
                }

                Serial.print(first_line_deg);
                Serial.print(",");
                Serial.print(second_line_deg);
                Serial.print(",");
                Serial.print(ygoal_deg);
                Serial.print(",");
                Serial.print(stop_base_deg[0] + 90);
                Serial.print(",");
                Serial.print( (stop_base_deg[1] + 90) % 360);
                Serial.print(",");
                Serial.print(goalkeep_mode);
                Serial.print(",");
                Serial.print(is_exist_deg_value_in_range(ball_deg, stop_base_deg[0] + 90, 40));
                Serial.print(",");
                Serial.println(is_exist_deg_value_in_range(ball_deg,(stop_base_deg[1] + 90) % 360, 40));
                
                int ball_stop_range = ball_dis >= 180 ? 2 : ball_dis >= 100 ? 3 : 3;
                
                int speed = 0;
                
                if( !is_exist_deg_value_in_range(ball_deg, (stop_base_deg[1] + 90) % 360, 90 - ball_stop_range - 3) && 
                    !is_exist_deg_value_in_range(ball_deg, (stop_base_deg[1] + 90) % 360, 90 - ball_stop_range - 3))
                {
                    speed = 60;
                }
                else
                {
                    if(ball_deg >= 100 && ball_deg <= 260)
                    {
                        speed = 60;
                    }
                    else
                    {
                        if(ball_dis >= 180)
                        {
                            if((ball_deg >= 70 && ball_deg <= 100) || (ball_deg >= 260 && ball_deg <= 290))
                            {
                                speed = 100;
                            }
                            else if((ball_deg >= 15 && ball_deg <= 70) || (ball_deg >= 290 && ball_deg <= 345))
                            {
                                speed = 95;
                            }
                            else
                            {
                                speed = 75;
                            }
                        }
                        else
                        {
                            if((ball_deg >= 70 && ball_deg <= 100) || (ball_deg >= 260 && ball_deg <= 290))
                            {
                                speed = 100;
                            }
                            else
                            {
                                speed = 90;
                            }
                        }
                    }
                }

                if(ball_deg == -1)
                {
                    if(is_exist_deg_value_in_range(ygoal_deg, 180, 3))
                    {
                        motor_move(0, 0);
                    }
                    else
                    {
                        if(is_exist_deg_value_in_range(first_line_deg, ygoal_deg, 90))
                        {
                            motor_move(first_line_deg, 65);
                        }
                        else
                        {
                            motor_move(second_line_deg, 65);
                        }
                    }
                }
                else
                { 
                    if(goalkeep_mode == 0)
                    {
                        if(is_exist_deg_value_in_range(ball_deg, stop_base_deg[0] + 90, 90 - ball_stop_range))
                        {
                            if(is_exist_deg_value_in_range(first_line_deg, stop_base_deg[0] + 90, 90))
                            {
                                motor_move(first_line_deg, speed);
                            }
                            else if(is_exist_deg_value_in_range(second_line_deg, stop_base_deg[0] + 90, 90))
                            {
                                motor_move(second_line_deg, speed);
                            }
                        }
                        else if(is_exist_deg_value_in_range(ball_deg, (stop_base_deg[1] + 90) % 360, 90 - ball_stop_range))
                        {
                            if(is_exist_deg_value_in_range(first_line_deg, (stop_base_deg[1] + 90) % 360, 90))
                            {
                                motor_move(first_line_deg, speed);
                            }
                            else if(is_exist_deg_value_in_range(second_line_deg, (stop_base_deg[1] + 90) % 360, 90))
                            {
                                motor_move(second_line_deg, speed);
                            }
                        }
                        else
                        {
                            motor_move(0, 0);
                        }

                        //縦のラインに入ったのでモード変更
                        if( (is_exist_deg_value_in_range(first_line_deg, 0, 45) && is_exist_deg_value_in_range(second_line_deg, 180, 45)) || 
                            (is_exist_deg_value_in_range(second_line_deg, 0, 45) && is_exist_deg_value_in_range(first_line_deg, 180, 45)))
                            {
                                goalkeep_mode = 1;
                            }
                    }
                    else if(goalkeep_mode == 1)
                    {
                        if(is_exist_deg_value_in_range(ball_deg, stop_base_deg[0] + 90, 90 - ball_stop_range))
                        {
                            if(is_exist_deg_value_in_range(first_line_deg, stop_base_deg[0] + 90, 90))
                            {
                                motor_move(first_line_deg, 50);
                            }
                            else if(is_exist_deg_value_in_range(second_line_deg, stop_base_deg[0] + 90, 90))
                            {
                                motor_move(second_line_deg, 50);
                            }
                        }
                        else if(is_exist_deg_value_in_range(ball_deg, (stop_base_deg[1] + 90) % 360, 90 - ball_stop_range))
                        {
                            if(is_exist_deg_value_in_range(first_line_deg, (stop_base_deg[1] + 90) % 360, 90))
                            {
                                motor_move(first_line_deg, 85);
                            }
                            else if(is_exist_deg_value_in_range(second_line_deg, (stop_base_deg[1] + 90) % 360, 90))
                            {
                                motor_move(second_line_deg, 85);
                            }
                        }
                        else
                        {
                            motor_move(0, 60);
                        }

                        if( !(is_exist_deg_value_in_range(first_line_deg, 0, 45) && is_exist_deg_value_in_range(second_line_deg, 180, 45)) && 
                            !(is_exist_deg_value_in_range(second_line_deg, 0, 45) && is_exist_deg_value_in_range(first_line_deg, 180, 45)))
                            {
                                goalkeep_mode = 0;
                            }
                    }
                    else
                    {
                        motor_move(0, 60);
                    }
                }
            }
        }
        else
        {
            defender_mode = 1;
        }
    }
    else if(defender_mode == 1) //ちょっとラインから外れた時
    {
        motor_move(keeper_previous_line_deg, 60); 

        if(is_line_detected())
            defender_mode = 0;
    }

    if(line_deg != -1)
        keeper_previous_line_deg = line_deg;
}