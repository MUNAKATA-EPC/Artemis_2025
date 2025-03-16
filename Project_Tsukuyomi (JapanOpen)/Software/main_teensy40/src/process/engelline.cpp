#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "engelline.hpp"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, 3, NEO_GRB + NEO_KHZ800);

void init_engelline()
{
    pixels.setBrightness(100);
    pixels.begin();  

    for(int i = 0; i < 32; i++)
    {
      //pixels.setPixelColor(i, pixels.Color(30, 245, 80));
      //pixels.setPixelColor(i, pixels.Color(180, 40, 255));
      pixels.rainbow(i);
      for(int j = 0; j < 32; j++)
      {
        if(j > i)
        {
            pixels.setPixelColor(j, pixels.Color(0, 0, 0));
        }
      }
      pixels.show();

      delay(30);
    }
}

void process_engelline()
{
    pixels.clear();

    for(int i = 0; i < 32; i++)
    {
      //pixels.setPixelColor(i, pixels.Color(30, 245, 80));
      pixels.setPixelColor(i, pixels.Color(180, 40, 255));
    }

    pixels.show();
}
