#include <Arduino.h>    

#include "DSR1202.h"
#include "sensor_variables.hpp"
#include "motor.hpp"

#define GYRO_PGAIN 1.2
#define GYRO_DGAIN 3.4
#define GOAL_PGAIN 0.5
#define GOAL_DGAIN 1.0

DSR1202 md = DSR1202(0);

int pid_value;

int pid_deviation;
int pid_previous_deviation;

float get_max_value_in_array(float* ar, int ar_cnt)
{
    float ret;

    for(int i = 0; i < ar_cnt; i++)
    {
        if(abs(ar[i]) > ret)
        {
            ret = abs(ar[i]);
        }
    }   

    return ret;
}

void motor_init()
{
    md.Init();
}

void pid_gyro()
{
    int p_value = 0, d_value = 0;
    int gyro_value = gyro_deg;
    gyro_value = gyro_value > 180 ? gyro_value - 360 : gyro_value;
    
    p_value = gyro_value;

    pid_deviation = gyro_value;
    d_value = pid_deviation - pid_previous_deviation;
    pid_previous_deviation = gyro_value;

    pid_value = p_value * GYRO_PGAIN + d_value * GYRO_DGAIN;
}
void pid_camera(int value)
{
    int p_value = 0, d_value = 0;
    int gyro_value = value;
    gyro_value = gyro_value > 180 ? gyro_value - 360 : gyro_value;
    
    p_value = gyro_value;

    pid_deviation = gyro_value;
    d_value = pid_deviation - pid_previous_deviation;
    pid_previous_deviation = gyro_value;

    pid_value = p_value * GOAL_PGAIN + d_value * GOAL_DGAIN;
}

void motor_break()
{
    md.motor_break();
}

void motor_direct_drive(int a, int b, int c, int d)
{
    md.move(a, b, -c, -d);
}

void motor_move(int deg, int power)
{
    //角度による計算
    float powers[4] = {0, 0, 0, 0};

    powers[0] = sin(radians(deg + 50)) * -power;
    powers[1] = sin(radians(deg - 50)) * -power;
    powers[2] = sin(radians(deg + 130)) * -power;
    powers[3] = sin(radians(deg - 130)) * -power;

    //モーターの出力がちゃんとpower通りになるようにする
    float strongest_power = get_max_value_in_array(powers, 4);

    if(strongest_power != 0)
    {
        float ratio_of_max_power = power / strongest_power;

        for(int i = 0; i < 3; i++)
        {
            powers[i] = (powers[i] * ratio_of_max_power);
        }
    }

    //PID処理
    powers[0] -= pid_value;
    powers[1] += pid_value;
    powers[2] += pid_value;
    powers[3] -= pid_value;

    //PIDの値によって値が100より大きくなったとき補正する
    strongest_power = get_max_value_in_array(powers, 4);

    if(strongest_power > 100)
    {
        int diff_val = strongest_power - 100;

        for(int i = 0; i < 4; i++)
        {
            if(powers[i] > 0)
            {
                powers[i] = powers[i] - diff_val;
            }
            else
            {
                powers[i] = powers[i] + diff_val;
            }
        }
    }
    else
    {
        for(int i = 0; i < 4; i++)
        {
            powers[i] = max(min(powers[i], 100), -100);
        }
    }

    motor_direct_drive((int)powers[0], (int)powers[1], (int)powers[2], (int)powers[3]);
}