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
    // 【アタッカープログラム構想】
    // A.ボールが見つからなかった場合
    //  自陣ゴール前まで移動。定位置で停止。
    //
    // B.ボールが見つかった場合
    //  B-1. ボールを前方で保持している場合
    //    ゴール方向にシュートをする。
    //
    //  B-2. ボールを後方でシュートしている場合
    //    前方のゴールの空いている方向にマカオシュート。
    //
    //  B-3. ボールがロボット前方に存在する場合
    //    ボールチェイスを行う
    //
    //  B-4. ボールがロボット後方に存在する場合
    //    マカオシュートを行うため、ボールを保持しに行く
    //  
    //  (B-3, B-4はボール方向に向いてもいいかもしれない)
    //
    // C.白線を踏んだ場合
    //   コート内に戻る。

    motor_direct_drive(speed, speed, speed, speed);
}
