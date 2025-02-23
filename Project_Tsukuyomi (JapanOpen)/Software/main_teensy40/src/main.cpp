//                                                                                                 ,,
//   MMP""MM""YMM                     `7MM                                                           db
//   P'   MM   `7                       MM
//        MM      ,pP"Ybd `7MM  `7MM    MM  ,MP'`7MM  `7MM  `7M'   `MF' ,pW"Wq.  `7MMpMMMb.pMMMb.  `7MM
//        MM      8I   `"   MM    MM    MM ;Y     MM    MM    VA   ,V  6W'   `Wb   MM    MM    MM    MM
//        MM      `YMMMa.   MM    MM    MM;Mm     MM    MM     VA ,V   8M     M8   MM    MM    MM    MM
//        MM      L.   I8   MM    MM    MM `Mb.   MM    MM      VVV    YA.   ,A9   MM    MM    MM    MM
//      .JMML.    M9mmmP'   `Mbod"YML..JMML. YA.  `Mbod"YML.    ,V      `Ybmd9'  .JMML  JMML  JMML..JMML.
//                                                           ,V
//                                                        OOb"

// Tsukuyomi Main Program For JapanOpen-2025

// Please check out the history of updates : https://github.com/MUNAKATA-EPC/Artemis_2025/commits/main/

#include <Arduino.h>

#include "common/sensors_variables.hpp"
#include "common/serial.hpp"
#include "common/motor.hpp"

#include "modules/BNO055.hpp"

BNO055 bno055;

void play_startup_sound()
{
    tone(2, 3500, 90);
    delay(110);
    tone(2, 3500, 90);
    delay(440);
    tone(2, 2700, 90);
    delay(110);
    tone(2, 3500, 90);
    delay(110);
    tone(2, 4500, 90);
    delay(110);
}

void setup() {
    Serial.println(9600);

    motor_init();

    bno055.init(6);

    play_startup_sound();
}

void loop() {
    bno055.process();

    gyro_deg = bno055.get_degrees();

    pid_gyro();

    motor_move(0, 0);
}