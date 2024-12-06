#ifndef _MOTOR_HPP
#define _MOTOR_HPP

extern void motor_init();
extern void motor_direct_drive(int a, int b, int c, int d);
extern void motor_move(int deg, int power);
extern void motor_break();

extern void pid_gyro();
extern void pid_camera(int value);

#endif
