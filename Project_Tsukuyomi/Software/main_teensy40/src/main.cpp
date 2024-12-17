#include <Arduino.h>
#include <Servo.h>

#include "sensor_variables.hpp"
#include "serial.hpp"
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

Servo dribbler;

bool is_running;

void dribbler_init()
{
  dribbler.attach(33);
  dribbler.writeMicroseconds(2000);
  delay(500);
  dribbler.writeMicroseconds(1000);
  delay(500);
}

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
  dribbler_init();
  kicker.init(12, 26);
  bno_imu.init(9);

  bt_start.init(11, Button::Button_Value_Type::PULLDOWN);

  play_startup_sound();
} 

void loop() {
  bno_imu.process();
  serials_process();
  ui_process();
  line_process();
 
  
  gyro_deg = bno_imu.get_degrees();

  kicker.loop();

  bt_start.loop();
  if(bt_start.is_pushed())
  {
    is_running = !is_running;
  }

  if(is_running)
  {
    dribbler.writeMicroseconds(1000);
    attacker_process(50);
  }
  else
  {
    dribbler.writeMicroseconds(1000);
    motor_direct_drive(0, 0, 0, 0);
  }

  Serial.println(fcam_ball_deg);
}