#include <Arduino.h>

#define DEBUG 1

#define PIN_DATA 0

#define PIN_S0 1
#define PIN_S1 2
#define PIN_S2 3
#define PIN_S3 4

#define PIN_E 5

int line_circle_values[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int output_value = 0;

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

    delayMicroseconds(10);

    return analogRead(PIN_DATA);
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_DATA, INPUT_PULLUP);
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S3, OUTPUT);

  pinMode(PIN_E, OUTPUT);

  Serial1.begin(115200);
  Serial1.setTimeout(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 16; i++)
  {
    line_circle_values[15 - i] = get_from_multiplexer(i) > 250 ? 1 : 0;
  }

  output_value = 0;
  
  for(int i = 0; i < 16; i++)
  {
    if(line_circle_values[i] == 1)
    {
      output_value += pow(2, i);
    }
  }

  if(DEBUG)
  {
    for(int i = 0; i < 16; i++)
    {
      Serial.print(line_circle_values[i]);
      Serial.print(",");
    }
    Serial.println();
  }
  

  Serial1.println(output_value);
  Serial1.flush();
}