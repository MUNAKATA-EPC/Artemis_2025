#include <Arduino.h>
const int PIN_MOTOR_INA[4] = {2, 4, 6, 8};
const int PIN_MOTOR_INB[4] = {3, 5, 7, 9};

class Motor{
  private:
    int _rotate;
    int _power;
  
  public:
    /// @brief モーターデータをセットします。
    /// @param power モーター駆動パワー(0~100%)
    /// @param rotate モーター回転方向(0:順転, 1:逆転)
    void set(int power, int rotate)
    {
      this->_power = power;
      this->_rotate = rotate;
    }
    
    /// @brief モーター駆動パワーを取得します。
    /// @return モーター駆動パワー(0-100%)
    int get_power()
    {
      return this->_power;
    }
  
    /// @brief モーター回転方向を取得します。
    /// @return モーター回転方向(0:順転, 1:逆転)
    int get_rotate()
    {
      return this->_rotate;
    }
};
  
Motor motors[4];

void get_motor_data()
{
  String data = "aaa";

  if(Serial1.available() > 0)
  {
    data = Serial1.readStringUntil('\n');
  }

  if(data[0] != 'a')
  {
    int motor_a_rotate = ((String)data[0]).toInt();
    int motor_b_rotate = ((String)data[4]).toInt();
    int motor_c_rotate = ((String)data[8]).toInt();
    int motor_d_rotate = ((String)data[12]).toInt();

    int motor_a_power = ((String)(data[1] + data[2] + data[3])).toInt();
    int motor_b_power = ((String)(data[5] + data[6] + data[7])).toInt();
    int motor_c_power = ((String)(data[9] + data[10] + data[11])).toInt();
    int motor_d_power = ((String)(data[13] + data[14] + data[15])).toInt();

    motors[0].set(motor_a_power, motor_a_rotate);
    motors[1].set(motor_b_power, motor_b_rotate);
    motors[2].set(motor_c_power, motor_c_rotate);
    motors[3].set(motor_d_power, motor_d_rotate);
  }
}

void setup() {
  // put your setup code here, to run once:

  analogWriteResolution(10);
  analogReadResolution(10);

  for(int i = 0; i < 4; i++)
  {
    motors[i].set(0, 0);
  }

  Serial.begin(9600);

  Serial1.begin(115200);
  Serial1.setTimeout(10);
}

void loop() {
  // put your main code here, to run repeatedly:

  get_motor_data();

  for(int i = 0; i < 4; i++)
  {
    if(motors[i].get_rotate() == 0)
    {
      if(motors[i].get_power() == 0)
      {
        analogWrite(PIN_MOTOR_INA[i], 0);
        analogWrite(PIN_MOTOR_INB[i], 0);
      }
      else
      {
        analogWrite(PIN_MOTOR_INA[i], (int)((motors[i].get_power()) / 100.0 * 1023.0));
        analogWrite(PIN_MOTOR_INB[i], 0);
      }
    }
    else
    {
      if(motors[i].get_power() == 0)
      {
        analogWrite(PIN_MOTOR_INA[i], 1023);
        analogWrite(PIN_MOTOR_INB[i], 1023);
      }
      else
      {
        analogWrite(PIN_MOTOR_INA[i], 0);
        analogWrite(PIN_MOTOR_INB[i], (int)((motors[i].get_power()) / 100.0 * 1023.0));
      }
    }
  }
}