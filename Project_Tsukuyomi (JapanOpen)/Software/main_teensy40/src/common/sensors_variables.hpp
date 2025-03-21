#include <Arduino.h>
#include "modules/kicker.hpp"
#include "modules/BNO055.hpp"

extern int gyro_deg;

extern int ball_deg;
extern int ball_dis;
extern int ygoal_deg;
extern int ygoal_dis;
extern int bgoal_deg;
extern int bgoal_dis;

extern int line_data[16];

extern Kicker f_kicker;
extern Kicker b_kicker;

extern BNO055 bno055;
