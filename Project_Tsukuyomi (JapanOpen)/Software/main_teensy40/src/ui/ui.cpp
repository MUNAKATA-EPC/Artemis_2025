#include <Arduino.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>

#include "ui.hpp"

#include "common/timer.hpp"

#define TOUCH_SCREEN_TOP_X 280.0
#define TOUCH_SCREEN_TOP_Y 300.0
#define TOUCH_SCREEN_BOTTOM_X 3780.0
#define TOUCH_SCREEN_BOTTOM_Y 3680.0
#define UI_SEND_DELAY 100

#define CS_PIN  4
#define TIRQ_PIN  2
// MOSI=11, MISO=12, SCK=13

//XPT2046_Touchscreen ts(CS_PIN);
//XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN);           //タッチIC用
Adafruit_ILI9341 tft = Adafruit_ILI9341(6, 5, 3);   //ディスプレイ用

//**************************************************************************
//タッチ座標変換部分
//**************************************************************************

//タッチ座標を画面座標に変換するための変数
typedef struct{
    int x;
    int y;
    int z;
  } PointPx;
PointPx Touch_Point;

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

//**************************************************************************
//UIシーン関連
//**************************************************************************

enum class UI_MENU
{
    MAIN,
    MODE_SELECT,
    CAMERA_VISION,
    LINE_CHECK,
    TEST_MODE
};
UI_MENU now_menu;

//**************************************************************************
//タッチ関連
//**************************************************************************

int touch_count = 0;
void process_touch()
{
    //タッチ処理
    if(ts.touched())
    {
        getTouchPoint(&Touch_Point);
    }
    else
    {
        Touch_Point.x = -1;
        Touch_Point.y = -1;
        Touch_Point.z = -1;
    }

    if(Touch_Point.x == -1 && Touch_Point.y == -1 && Touch_Point.z == -1)
    {
        touch_count++;
    }
    else
    {
        if(touch_count != -1)
        {
            touch_count = -1;
        }
        else
        {
            touch_count = 0;
        }
    }
}
bool is_released()
{
    return touch_count == -1;
}
bool is_touched()
{
    return touch_count == 1;
}
bool is_touching()
{
    return touch_count >= 1;
}

//**************************************************************************
//UIデータ送信関連
//**************************************************************************

Timer send_timer;
bool is_sending()
{
    return send_timer.get_value() >= UI_SEND_DELAY;
}

//**************************************************************************
//モード実行関連
//**************************************************************************

bool is_running = false;
int process_mode = 0;

void init_ui()
{
    ts.begin();
    ts.setRotation(2);
  
    tft.begin();
    tft.setRotation(0);
}

void process_ui()
{
    send_timer.start();
    send_timer.tick();

    //背景の初期化
    if(is_sending())
    {
        tft.fillScreen(ILI9341_WHITE);  
    }

    if(now_menu == UI_MENU::MAIN)
    {
        //各モードに移行するための表示
        //もう面倒だし1/4の長方形を表示とかが一番楽かもしれない。

        if(is_sending())
        {
            //表示部分
        }

        //処理部分
    }
    else if(now_menu == UI_MENU::MODE_SELECT)
    {
        //ロボットの実行モードを設定。
        //ゴール方向・攻守・ジャイロ設定・簡易的なカメラの色取りチェックなどを行えるようにする

        if(is_sending())
        {
            //表示部分
        }

        //処理部分
    }
    else if(now_menu == UI_MENU::CAMERA_VISION)
    {
        //カメラの視界を確認できる
        //ボール・黄色・青色の角度と距離をGUI及び座標表示で表す。

        if(is_sending())
        {
            //表示部分
        }

        //処理部分
    }
    else if(now_menu == UI_MENU::LINE_CHECK)
    {
        //ラインセンサーの確認
        //反応している角度、engelline.cppで処理された値を表示。

        if(is_sending())
        {
            //表示部分
        }

        //処理部分
    }
    else if(now_menu == UI_MENU::TEST_MODE)
    {
        //テストモード
        //ドリブラー・キッカーの動作確認。

        if(is_sending())
        {
            //表示部分
        }

        //処理部分
    }

    if(send_timer.get_value() >= UI_SEND_DELAY)
    {
        send_timer.reset();
        send_timer.start();
    }
}
