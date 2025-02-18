#include <Arduino.h>

int voltage;
int voltage_counter = 0;

float voltage_buffer[100];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  analogWriteResolution(10);
  analogReadResolution(10);
}

void loop() {
  // put your main code here, to run repeatedly:

  float voltage = (analogRead(A3)) / 1023.0 * 13.2;
  voltage_counter++;

  if(voltage_counter >= 100) 
    voltage_counter = 100;

  for(int i = 99; i >= 0; i--)
  {
    if(i != 99)
    {
      voltage_buffer[i + 1] = voltage_buffer[i];
    }
    else
    {
      voltage_buffer[0] = voltage;
    }
  }

  float average_voltage = 0;
  for(int i = 0; i < 100; i++)
  {
    average_voltage += voltage_buffer[i];
  }

  average_voltage = average_voltage / voltage_counter;
  //tone(23, 3000, 10);

  digitalWrite(22, HIGH);

  Serial.println(average_voltage);

  delay(50);
}
