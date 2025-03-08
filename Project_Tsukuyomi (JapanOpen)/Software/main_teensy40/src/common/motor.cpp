#include <Arduino.h>    

#include "sensors_variables.hpp"
#include "motor.hpp"

#define GYRO_PGAIN 0.95
#define GYRO_DGAIN 3.5
#define GOAL_PGAIN 0.9
#define GOAL_DGAIN 5.0

#define PID_MAX 80

int pid_value;

int pid_deviation;
int pid_previous_deviation;

int power_f_bldc;
int power_b_bldc;

void set_bldc(int f_power, int b_power)
{
    power_f_bldc = f_power;
    power_b_bldc = b_power;
}

void motor_init()
{
    Serial2.begin(115200);
    Serial2.setTimeout(10);
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

    if(pid_value < 0)
    {
        if(pid_value < -PID_MAX)
        {
            pid_value = -PID_MAX;
        }
    }
    else
    {
        if(pid_value > PID_MAX)
        {
            pid_value = PID_MAX;
        }
    }
}

void pid_camera(int value)
{
    int p_value = 0, d_value = 0;
    int gyro_value = value;
    gyro_value = gyro_value > 180 ? -(gyro_value - 360) : -gyro_value;
    
    p_value = gyro_value;

    pid_deviation = gyro_value;
    d_value = pid_deviation - pid_previous_deviation;
    pid_previous_deviation = gyro_value;

    pid_value = p_value * GOAL_PGAIN + d_value * GOAL_DGAIN;
    
    if(pid_value < 0)
    {
        if(pid_value < -PID_MAX)
        {
            pid_value = -PID_MAX;
        }
    }
    else
    {
        if(pid_value > PID_MAX)
        {
            pid_value = PID_MAX;
        }
    }
}

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

void motor_break()
{
    Serial1.println("1000100010001000");
}

void motor_direct_drive(int a, int b, int c, int d, bool a_b = false, bool b_b = false, bool c_b = false, bool d_b = false)
{
    String power_a = String(min(max(abs(a), 0), 100));
    String power_b = String(min(max(abs(b), 0), 100));
    String power_c = String(min(max(abs(c), 0), 100));
    String power_d = String(min(max(abs(d), 0), 100));
    String power_b_f = String(min(max(abs(power_f_bldc), 0), 100));
    String power_b_b = String(min(max(abs(power_b_bldc), 0), 100));

    if(abs(a) <= 9)
    {
        power_a = "00" + power_a;
    }
    else if(abs(a) <= 99)
    {
        power_a = "0" + power_a;
    }

    if(abs(b) <= 9)
    {
        power_b = "00" + power_b;
    }
    else if(abs(b) <= 99)
    {
        power_b = "0" + power_b;
    }

    if(abs(c) <= 9)
    {
        power_c = "00" + power_c;
    }
    else if(abs(c) <= 99)
    {
        power_c = "0" + power_c;
    }

    if(abs(d) <= 9)
    {
        power_d = "00" + power_d;
    }
    else if(abs(d) <= 99)
    {
        power_d = "0" + power_d;
    }

    if(abs(power_f_bldc) <= 9)
    {
        power_b_f = "00" + power_b_f;
    }
    else if(abs(power_f_bldc) <= 99)
    {
        power_b_f = "0" + power_b_f;
    }

    if(abs(power_b_bldc) <= 9)
    {
        power_b_b = "00" + power_b_b;
    }
    else if(abs(power_b_bldc) <= 99)
    {
        power_b_b = "0" + power_b_b;
    }

    String rotate_a = a < 0 ? "1" : "0";
    String rotate_b = b < 0 ? "1" : "0";
    String rotate_c = c < 0 ? "1" : "0";
    String rotate_d = d < 0 ? "1" : "0";

    if(a_b)
    {
        rotate_a = "1";
        power_a = "000";
    }
    if(b_b)
    {
        rotate_b = "1";
        power_b = "000";
    }
    if(c_b)
    {
        rotate_c = "1";
        power_c = "000";
    }
    if(d_b)
    {
        rotate_d = "1";
        power_d = "000";
    }

    String send_data = rotate_a + power_a + rotate_b + power_b + rotate_c + power_c + rotate_d + power_d + power_b_f + power_b_b;

    Serial2.println(send_data);
}

void motor_move(int deg, int power)
{
    //角度による計算
    float powers[4] = {0, 0, 0, 0};

    deg = 360 - deg;

    powers[0] = sin(radians(deg - 45)) * -power;
    powers[1] = sin(radians(deg + 45)) * -power;
    powers[2] = sin(radians(deg + 135)) * -power;
    powers[3] = sin(radians(deg - 135)) * -power;

    float strongest_power = 0;

    for(int i = 0; i < 4; i++)
    {
        if(abs(powers[i]) > strongest_power)
        {
            strongest_power = abs(powers[i]);
        }
    }   

    if(strongest_power != 0)
    {
        float ratio_of_max_power = power / strongest_power;

        for(int i = 0; i < 4; i++)
        {
            powers[i] = (powers[i] * ratio_of_max_power);
        }
    }

    for(int i = 0; i < 4; i++)
    {
        powers[i] -= pid_value;
    }

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