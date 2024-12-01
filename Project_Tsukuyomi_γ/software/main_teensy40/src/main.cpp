#include <Arduino.h>

#include "serial.hpp"

#include "DSR1202.h"
#include "BNO055.hpp"

#include "timer.hpp"
#include "button.hpp"
#include "kicker.hpp"

#include "engelline.hpp"

#include "ui.hpp"

DSR1202 md = DSR1202(0);
BNO055 bno_imu;

Kicker kicker;

void setup() {
  Serial.begin(9600);

  ui_init();
  serials_init();
  line_init();

  tone(3, 2700, 90);
  delay(110);
  tone(3, 3500, 90);
  delay(110);
  tone(3, 4500, 90);
  delay(110);

  md.Init();

  bno_imu.init(11);
} 

void loop() {
  //md.move(100, 100, 100, 100);

  ui_process();
  serials_process();
  line_process();

  bno_imu.process();
  //Serial.println(bno_imu.get_degrees());
}