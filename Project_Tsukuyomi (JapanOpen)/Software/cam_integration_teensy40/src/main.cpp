#include <Arduino.h>

//1~6個のカメラのデータ
//{ball_dir, ball_dis, goal_y_dir, goal_y_dis, goal_b_dir, goal_b_dis}
int cam_data[6][6] = { 
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
  };

HardwareSerialIMXRT serial_list[6] = {Serial2, Serial3, Serial4, Serial5, Serial6, Serial7};

int read_serial_type = 0;
int ball_deg = -1;

void read_cam_data(HardwareSerialIMXRT serial, int index)
{
  if(serial.available() > 0)
  {
    cam_data[index][0] = serial.readStringUntil('a').toInt();
    cam_data[index][1] = serial.readStringUntil('b').toInt();
    cam_data[index][2] = serial.readStringUntil('c').toInt();
    cam_data[index][3] = serial.readStringUntil('d').toInt();
    cam_data[index][4] = serial.readStringUntil('e').toInt();
    cam_data[index][5] = serial.readStringUntil('f').toInt();
  }
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  Serial1.begin(115200);
  Serial1.setTimeout(10);

  Serial2.begin(9600);
  Serial2.setTimeout(10);
  Serial3.begin(9600);
  Serial3.setTimeout(10);
  Serial4.begin(9600);
  Serial4.setTimeout(10);
  Serial5.begin(9600);
  Serial5.setTimeout(10);
  Serial6.begin(9600);
  Serial6.setTimeout(10);
  Serial7.begin(9600);
  Serial7.setTimeout(10);
}

int calculate_average() 
{
  int sum = 0;
  int count = 0;
  bool added360 = false;  // 360を加算したかどうかのフラグ
  int originalVal = cam_data[0][0];  // cam_data[0][0] の元の値を保存

  for (int i = 0; i < 6; i++) 
  {
      // 1回目のループだけ cam_data[0][0] に 360 を加算
      if (i == 0 && !added360 && cam_data[0][0] != 500 && cam_data[5][0] != 500)
      {
          cam_data[0][0] += 360;  // cam_data[0][0] に 360 を加算
          added360 = true;  // 加算したことを記録
      }

      // cam_data[i][0] が 500 でない場合にのみ sum に加算
      if (cam_data[i][0] != 500)
      {
          sum += cam_data[i][0];  // cam_data[i][0] を足す
          count++;
      }
  }

  // ループ終了後、cam_data[0][0] の値を元に戻す
  cam_data[0][0] = originalVal;

  if (count > 0) 
  {
    // 平均を計算
    float average = (float)sum / count;

    // 360で割った余りを求める
    if (average >= 360)
    {
      if (average >= 720)
      {
        return (((int)(average) % 720) + 315) % 360;
      }
      else
      {
        return (((int)(average) % 360) + 315) % 360;
      }
    }
    else
    {
      return ((int)(average) + 315) % 360;  // 余りが360未満の場合、平均をそのまま整数で返す
    }
  } 
  else 
  {
    return -1;  // データがなければ -1 を返す
  }
}

void loop() {

  if(Serial2.available() > 0 && (read_serial_type == 0 || read_serial_type == 1 || read_serial_type == 2))
  {
    cam_data[0][0] = Serial2.readStringUntil('a').toInt();
    cam_data[0][1] = Serial2.readStringUntil('b').toInt();
    cam_data[0][2] = Serial2.readStringUntil('c').toInt();
    cam_data[0][3] = Serial2.readStringUntil('d').toInt();
    cam_data[0][4] = Serial2.readStringUntil('e').toInt();
    cam_data[0][5] = Serial2.readStringUntil('f').toInt();
  }

  if(Serial3.available() > 0 && (read_serial_type == 1 || read_serial_type == 2 || read_serial_type == 3))
  {
    cam_data[1][0] = Serial3.readStringUntil('a').toInt();
    cam_data[1][1] = Serial3.readStringUntil('b').toInt();
    cam_data[1][2] = Serial3.readStringUntil('c').toInt();
    cam_data[1][3] = Serial3.readStringUntil('d').toInt();
    cam_data[1][4] = Serial3.readStringUntil('e').toInt();
    cam_data[1][5] = Serial3.readStringUntil('f').toInt();

    cam_data[1][0] = cam_data[1][0] == 500 ? 500 : cam_data[1][0] + 60;
    cam_data[1][2] = cam_data[1][0] == 500 ? 500 : cam_data[1][2] + 60;
    cam_data[1][4] = cam_data[1][0] == 500 ? 500 : cam_data[1][4] + 60;
  }

  if(Serial4.available() > 0 && (read_serial_type == 2 || read_serial_type == 3 || read_serial_type == 4))
  {
    cam_data[2][0] = Serial4.readStringUntil('a').toInt();
    cam_data[2][1] = Serial4.readStringUntil('b').toInt();
    cam_data[2][2] = Serial4.readStringUntil('c').toInt();
    cam_data[2][3] = Serial4.readStringUntil('d').toInt();
    cam_data[2][4] = Serial4.readStringUntil('e').toInt();
    cam_data[2][5] = Serial4.readStringUntil('f').toInt();

    cam_data[2][0] = cam_data[2][0] == 500 ? 500 : cam_data[2][0] + 60 * 2;
    cam_data[2][2] = cam_data[2][0] == 500 ? 500 : cam_data[2][2] + 60 * 2;
    cam_data[2][4] = cam_data[2][0] == 500 ? 500 : cam_data[2][4] + 60 * 2;
  }

  if(Serial5.available() > 0 && (read_serial_type == 3 || read_serial_type == 4 || read_serial_type == 5))
  {
    cam_data[3][0] = Serial5.readStringUntil('a').toInt();
    cam_data[3][1] = Serial5.readStringUntil('b').toInt();
    cam_data[3][2] = Serial5.readStringUntil('c').toInt();
    cam_data[3][3] = Serial5.readStringUntil('d').toInt();
    cam_data[3][4] = Serial5.readStringUntil('e').toInt();
    cam_data[3][5] = Serial5.readStringUntil('f').toInt();

    cam_data[3][0] = cam_data[3][0] == 500 ? 500 : cam_data[3][0] + 60 * 3;
    cam_data[3][2] = cam_data[3][0] == 500 ? 500 : cam_data[3][2] + 60 * 3;
    cam_data[3][4] = cam_data[3][0] == 500 ? 500 : cam_data[3][4] + 60 * 3;
  }

  if(Serial6.available() > 0 && (read_serial_type == 4 || read_serial_type == 5 || read_serial_type == 0))
  {
    cam_data[4][0] = Serial6.readStringUntil('a').toInt();
    cam_data[4][1] = Serial6.readStringUntil('b').toInt();
    cam_data[4][2] = Serial6.readStringUntil('c').toInt();
    cam_data[4][3] = Serial6.readStringUntil('d').toInt();
    cam_data[4][4] = Serial6.readStringUntil('e').toInt();
    cam_data[4][5] = Serial6.readStringUntil('f').toInt();

    cam_data[4][0] = cam_data[4][0] == 500 ? 500 : cam_data[4][0] + 60 * 4;
    cam_data[4][2] = cam_data[4][0] == 500 ? 500 : cam_data[4][2] + 60 * 4;
    cam_data[4][4] = cam_data[4][0] == 500 ? 500 : cam_data[4][4] + 60 * 4;
  }

  if(Serial7.available() > 0 && (read_serial_type == 5 || read_serial_type == 0 || read_serial_type == 1))
  {
    cam_data[5][0] = Serial7.readStringUntil('a').toInt();
    cam_data[5][1] = Serial7.readStringUntil('b').toInt();
    cam_data[5][2] = Serial7.readStringUntil('c').toInt();
    cam_data[5][3] = Serial7.readStringUntil('d').toInt();
    cam_data[5][4] = Serial7.readStringUntil('e').toInt();
    cam_data[5][5] = Serial7.readStringUntil('f').toInt();

    cam_data[5][0] = cam_data[5][0] == 500 ? 500 : cam_data[5][0] + 60 * 5;
    cam_data[5][2] = cam_data[5][0] == 500 ? 500 : cam_data[5][2] + 60 * 5;
    cam_data[5][4] = cam_data[5][0] == 500 ? 500 : cam_data[5][4] + 60 * 5;
  }

  read_serial_type = (read_serial_type + 1) % 6;

  for(int i = 0; i < 6; i++)
  {
    Serial.print(cam_data[i][0]);
    Serial.print("\t");
  }

  //Serial.println(read_serial_type);

  int average = calculate_average();
  Serial.println(average);

}
