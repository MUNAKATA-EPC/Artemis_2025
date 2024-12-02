#include <Arduino.h>

#include "serial.hpp"
#include "sensor_variables.hpp"
#include "timer.hpp"

#include "DSR1202.h"
#include "BNO055.hpp"
#include "kicker.hpp"
#include "button.hpp"
#include "toggle.hpp"

#include "motor.hpp"
#include "engelline.hpp"
#include "attacker.hpp"

#include "ui.hpp"

BNO055 bno_imu;
Kicker kicker;
Button bt_start;

bool is_running;

void play_startup_sound()
{
  tone(3, 2700, 90);
  delay(110);
  tone(3, 3500, 90);
  delay(110);
  tone(3, 4500, 90);
  delay(110);
}

void setup() {
  Serial.begin(9600);

  ui_init();
  serials_init();
  line_init();
  motor_init();
  bno_imu.init(9);

  bt_start.init(11, Button::Button_Value_Type::PULLDOWN);

  play_startup_sound();
} 

void loop() {
  bno_imu.process();
  serials_process();
  ui_process();
  line_process();

  if(bt_start.is_pushed())
  {
    is_running = !is_running;
  }

  if(is_running)
  {
    attacker_process(50);
  }
  else
  {
    motor_direct_drive(0, 0, 0, 0);
  }
}