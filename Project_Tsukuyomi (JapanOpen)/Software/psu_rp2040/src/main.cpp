#include <Arduino.h>

int voltage;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(3, INPUT);

  analogWriteResolution(10);
  analogReadResolution(10);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(22, HIGH);

  delay(50);
}
