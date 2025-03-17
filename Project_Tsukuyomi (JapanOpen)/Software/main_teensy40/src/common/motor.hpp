#ifndef _MOTOR_HPP
#define _MOTOR_HPP

extern void init_motor();
extern void motor_direct_drive(int a, int b, int c, int d, bool a_b = false, bool b_b = false, bool c_b = false, bool d_b = false);
extern void motor_move(int deg, int power);
extern void motor_break();
extern void motor_set_bldc(int f_power, int b_power);

extern void pid_gyro();
extern void pid_camera(int value);

#endif
