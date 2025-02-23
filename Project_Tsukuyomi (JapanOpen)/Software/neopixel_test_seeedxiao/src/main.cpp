#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(10, 5, NEO_GRB + NEO_KHZ800);


void setup() {
  pixels.setBrightness(100);
  pixels.begin();
+
  Serial.begin(9600);
}

int step_num = 0;

void loop() {
  for(int i = 0; i < 10; i++)
  {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.show();
  }
}