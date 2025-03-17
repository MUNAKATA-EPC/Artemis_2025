#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, 8, NEO_GRB + NEO_KHZ800);


void setup() {
  pixels.begin();
  pixels.setBrightness(20);

  //Serial.begin(9600);
}

int step_num = 0;

void loop() {
  for(int i = 0; i < 32; i++)
  {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(0, 255, 255));
    pixels.show();
  }
}