#include <Arduino.h>

#define PIN_DATA 0

#define PIN_S0 1
#define PIN_S1 2
#define PIN_S2 3
#define PIN_S3 4

#define PIN_E 5


int line_circle_values[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int output_value;

class average
{
private:
  int stock_rotation_term = 20; // 移動平均の期間（標本数）
  int stock_rotation_number = 0; // 輪番用のカウント
  float stock_data[50];         // データをストックする配列
  float new_data;                // センサから取得した最新のデータ

  bool ave = false;
  float result;
public:
  float moving_average(float data)
  { // IMUデータの移動平均値の取得
  // 二次元配列の0番から輪番で最新のデータを入れていく。指定個数を上回ったら0番に戻す
    if (stock_rotation_number >= stock_rotation_term)
    {
      stock_rotation_number = 0;
      ave = true;
    }

    stock_data[stock_rotation_number] = data; // ストックの輪番箇所にデータ上書き

    result = 0; // 結果を初期化
    if(ave == true){
      for (int i = 0; i < stock_rotation_term-1; i++)
      { // ストックした値を合計する
        result += stock_data[i];
      }

      result = result / stock_rotation_term; // 合計値を標本数でを割る

    }else{
      for (int i = 0; i <= stock_rotation_number; i++)
      { // ストックした値を合計する
        result += stock_data[i];
      }

      result = result / (stock_rotation_number + 1); // 合計値を標本数でを割る

    }

    stock_rotation_number += 1;

    return result;
  }
};

/// @brief デバッグ出力をする関数です。
void print_debug_value()
{
  for(int i = 0; i < 16; i++)
  {
    Serial.print(line_circle_values[i]);
    Serial.print("\t");
  }

  Serial.println(output_value);
}

/// @brief マルチプレクサーからの値を取得する関数です。
/// @param idx マルチプレクサから取得する値のインデックス。
/// @return 指定したインデックスのアナログ値。
int get_from_multiplexer(int idx)
{
    uint8_t mul_address = idx;
    uint8_t bit;
    
    for(uint8_t j = 0; j < 4; j++)
    {
      bit = 1 << j;

      if(bit & mul_address){
        digitalWrite(j + PIN_S0, HIGH);
      }
      else{
        digitalWrite(j + PIN_S0, LOW);
      }
    }

    delayMicroseconds(50);

    return analogRead(PIN_DATA);
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_DATA, INPUT_PULLDOWN);
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S3, OUTPUT);

  pinMode(PIN_E, OUTPUT);

  Serial1.begin(115200);
  Serial1.setTimeout(10);
}

average line_data[16];

void loop() {
  //出力時は常にPIN_EはLOWにする必要がある
  digitalWrite(PIN_E, LOW);

  //16ピン分のセンサーの値をマルチプレクサから取得する
  for(int i = 0; i < 16; i++)
  {
    line_circle_values[i] = line_data[i].moving_average(get_from_multiplexer(i)) >= 34 ? 1 : 0;
  }
  output_value = 0;
  
  for(int i = 0; i < 16; i++)
  {
    if(line_circle_values[i] == 1)
    {
      output_value += pow(2, i);
    }
  }

  print_debug_value();

  Serial1.println(output_value);
  Serial1.flush();

  //delay(10);
}