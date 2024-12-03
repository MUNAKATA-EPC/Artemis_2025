#include <Arduino.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>

#define TOUCH_SCREEN_TOP_X 280.0
#define TOUCH_SCREEN_TOP_Y 300.0
#define TOUCH_SCREEN_BOTTOM_X 3780.0
#define TOUCH_SCREEN_BOTTOM_Y 3680.0

#define CS_PIN  4
// MOSI=11, MISO=12, SCK=13

//XPT2046_Touchscreen ts(CS_PIN);
#define TIRQ_PIN  2
//XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling
Adafruit_ILI9341 tft = Adafruit_ILI9341(6, 5, 3);

typedef struct{
  int x;
  int y;
  int z;
} PointPx;

PointPx Touch_Point;


/**
*  座標を、240*320に変換
*
* @param caliblationX タッチされた座標 TS_Point.x
* @param caliblationY タッチされた座標 TS_Point.y
*
  @param px 変換した座標を格納する構造体のポインター
*/
void convertTouchScreenToPx(float caliblationX, float caliblationY, PointPx *px)
{

  int x = ((caliblationX - TOUCH_SCREEN_TOP_X) / (TOUCH_SCREEN_BOTTOM_X - TOUCH_SCREEN_TOP_X)) * ILI9341_TFTWIDTH;
  int y = ((caliblationY - TOUCH_SCREEN_TOP_Y) / (TOUCH_SCREEN_BOTTOM_Y - TOUCH_SCREEN_TOP_Y)) * ILI9341_TFTHEIGHT;
  px->x = x >= ILI9341_TFTWIDTH ? ILI9341_TFTWIDTH : x;
  px->y = y >= ILI9341_TFTHEIGHT ? ILI9341_TFTHEIGHT : y;
}
void getTouchPoint(PointPx *px)
{
  int count = 1;
  float toucheSumX = 0.0;
  float toucheSumY = 0.0;
  TS_Point p;
  while (ts.touched() && count <= 1)
  {
    p = ts.getPoint();
    toucheSumX += p.x;
    toucheSumY += p.y;
    count++;
  }

  float caliblationX = toucheSumX / 1.0;
  float caliblationY = toucheSumY / 1.0;
  px->z = ts.getPoint().z;

  convertTouchScreenToPx(caliblationX, caliblationY, px);
}

void setup() {
  Serial.begin(38400);
  
  ts.begin();
  ts.setRotation(2);

  tft.begin();
  tft.setRotation(0);

  tft.fillScreen(ILI9341_BLACK);

}

void loop() {

  if (ts.touched()) {    
    getTouchPoint(&Touch_Point);

    Serial.print("Pressure = ");
    Serial.print(Touch_Point.z);
    Serial.print(", x = ");
    Serial.print(Touch_Point.x);
    Serial.print(", y = ");
    Serial.print(Touch_Point.y);

    tft.drawPixel(Touch_Point.x, Touch_Point.y, ILI9341_BLUE);

    Serial.println();
  }
}

