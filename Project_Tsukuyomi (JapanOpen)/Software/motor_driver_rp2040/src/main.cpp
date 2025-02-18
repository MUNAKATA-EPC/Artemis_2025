#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:

  analogWriteResolution(10);
  analogReadResolution(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(2, 0);
  analogWrite(3, 500);
  delay(1000);
  analogWrite(2, 500);
  analogWrite(3, 0);
  delay(1000);
}