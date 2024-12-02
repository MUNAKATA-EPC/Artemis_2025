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
    motor_direct_drive(speed, speed, speed, speed);
}
