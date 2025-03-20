#include <Arduino.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#define _ADAFRUIT_GFX_H
#include <ILI9341_t3.h>
#include <font_ArialBold.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>

#include "ui.hpp"

#include "common/timer.hpp"

#define TOUCH_SCREEN_TOP_X 280.0
#define TOUCH_SCREEN_TOP_Y 300.0
#define TOUCH_SCREEN_BOTTOM_X 3780.0
#define TOUCH_SCREEN_BOTTOM_Y 3680.0
#define UI_SEND_DELAY 100
#define NEOPIXEL_SEND_DELAY 200

#define CS_PIN  9
#define TIRQ_PIN  -1
// MOSI=11, MISO=12, SCK=13

//XPT2046_Touchscreen ts(CS_PIN);
//XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
XPT2046_Touchscreen ts(CS_PIN);               //タッチIC用
ILI9341_t3 tft = ILI9341_t3(10, 27, 26);    //ディスプレイ用
//Adafruit_ILI9341 tft = Adafruit_ILI9341(1, 1, 1);    //ディスプレイ用

Adafruit_NeoPixel pixels_ui = Adafruit_NeoPixel(64, 4, NEO_GRB + NEO_KHZ800);

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
UI_MENU now_menu = UI_MENU::MAIN;

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

    if(!(Touch_Point.x == -1 && Touch_Point.y == -1 && Touch_Point.z == -1))
    {
        touch_count++;
    }
    else
    {
        if(touch_count <= 0)
        {
            touch_count = 0;
        }
        else
        {
            touch_count = -1;
        }
    }

    //Serial.println(touch_count);
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
Timer send_timer_neopixel;

bool is_update = false;

bool is_sending()
{
    return send_timer.get_value() >= UI_SEND_DELAY;
}
bool is_neopixel_sending()
{
    return send_timer_neopixel.get_value() >= NEOPIXEL_SEND_DELAY;
}

//**************************************************************************
//モード実行関連
//**************************************************************************

bool is_running = false;
int process_mode = 0;

Timer blind_fade;

bool is_exists_point_in_rect(int x, int y, int w, int h)
{
    return  Touch_Point.x >= x && 
            Touch_Point.x <= x + w &&
            Touch_Point.y >= y &&
            Touch_Point.y <= y + h;
}

void init_ui()
{
    pixels_ui.begin();
    pixels_ui.setBrightness(40);

    SPI.setMISO(12);
    SPI.setMOSI(11);
    SPI.setSCK(13);

    ts.begin();
    ts.setRotation(1);
  
    tft.begin();
    tft.setRotation(3);

    tft.fillScreen(ILI9341_BLACK);

    pixels_ui.clear();

    for(int i = 0; i < 64; i++)
    {
        pixels_ui.setPixelColor(i, pixels_ui.Color(255, 0, 255));

        pixels_ui.show();

        delay(5);
    }

    blind_fade.start();
}

void process_ui()
{
    send_timer.start();
    send_timer.tick();
    send_timer_neopixel.start();
    send_timer_neopixel.tick();

    blind_fade.tick();

    if(blind_fade.get_value() >= 2000)
    {
        blind_fade.reset();
        blind_fade.start();
    }

    process_touch();

    //tft.fillScreen(ILI9341_BLACK); 

    //背景の初期化
    if(is_sending())
    { 
        //tft.fillScreen(ILI9341_BLACK);
    }

    if(now_menu == UI_MENU::MAIN)
    {
        //各モードに移行するための表示
        //もう面倒だし1/4の長方形を表示とかが一番楽かもしれない。

        //表示部分
        if(is_sending())
        {
            if(!is_update)
            {
                tft.fillRect(10, 10, 160 - 15, 120 - 15, tft.color565(255, 255, 255));
                tft.fillRect(320 / 2 + 5, 10, 160 - 15, 120 - 15, tft.color565(255, 255, 255));
                tft.fillRect(10, 240 / 2 + 5, 160 - 15, 120 - 15, tft.color565(255, 255, 255));
                tft.fillRect(320 / 2 + 5, 240 / 2 + 5, 160 - 15, 120 - 15, tft.color565(255, 255, 255));

                tft.setCursor(20, 20);
                tft.setTextColor(ILI9341_YELLOW);
                tft.print("PROGRAM MODE");

                is_update = true;
            }
        }
        if(is_neopixel_sending())
        {   
            for(int i = 0; i < 64; i++)
            {
                int color_value = blind_fade.get_value() <= 1000 ? sin(radians(blind_fade.get_value() / 1000.0 * 90)) * 192.0 + 63 : 255 - sin(radians((blind_fade.get_value() - 1000) / 1000.0 * 90)) * 192.0;
                pixels_ui.setPixelColor(i, pixels_ui.Color(color_value, 0, color_value));
            }

            pixels_ui.show();
        }

        //処理部分
        
        if(is_touched())
        {
            if(is_exists_point_in_rect(10, 10, 160 - 15, 120 - 15))
            {
                tone(2, 4500, 15);

                now_menu = UI_MENU::MODE_SELECT;
                is_update = false;

                for(int i = 0; i < 64; i++)
                {
                    pixels_ui.setPixelColor(i, pixels_ui.Color(0, 0, 0));
                }
    
                pixels_ui.show();
            }

            Serial.println("touched!!");
        }
        if(is_released())
        {
            Serial.println("released!!");
        }
    }
    else if(now_menu == UI_MENU::MODE_SELECT)
    {
        //ロボットの実行モードを設定。
        //ゴール方向・攻守・ジャイロ設定・簡易的なカメラの色取りチェックなどを行えるようにする

        //表示部分
        if(is_sending())
        {
            if(!is_update)
            {
                tft.fillScreen(tft.color565(240, 240, 250));

                tft.setCursor(5, 5);
                tft.setFont(Arial_24_Bold);
                tft.setTextColor(ILI9341_BLACK);
                tft.print("PROGRAM MODE");

                tft.setFont(Arial_12_Bold);
                tft.setCursor(25, 55);
                tft.print("MODE & ATTACK DIRECTION");

                if(process_mode == 0)
                {
                    tft.setFont(Arial_24_Bold);
                    tft.setTextColor(tft.color565(0, 0, 0));
                    tft.setCursor(80, 95);
                    tft.print("YELLOW");
                    tft.setCursor(60, 125);
                    tft.print("ATTACKER");
                }
                else if(process_mode == 1)
                {
                    tft.setFont(Arial_24_Bold);
                    tft.setTextColor(ILI9341_BLUE);
                    tft.setCursor(105, 95);
                    tft.print("BLUE");
                    tft.setCursor(60, 125);
                    tft.print("ATTACKER");
                }
                else if(process_mode == 2)
                {
                    tft.setFont(Arial_24_Bold);
                    tft.setTextColor(ILI9341_GREENYELLOW);
                    tft.setCursor(8, 102);
                    tft.print("YELLOW KEEPER");
                }
                else if(process_mode == 3)
                {
                    tft.setFont(Arial_24_Bold);
                    tft.setTextColor(ILI9341_BLUE);
                    tft.setCursor(30, 102);
                    tft.print("BLUE KEEPER");
                }

                is_update = true;
            }
        }

        //処理部分

        if(is_touched())
        {
            if(is_exists_point_in_rect(0, 70, 320, 70))
            {
                tone(2, 4500, 15);

                process_mode = (process_mode + 1) % 4;

                is_update = false;
            }
        }
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
    if(send_timer_neopixel.get_value() >= NEOPIXEL_SEND_DELAY)
    {
        send_timer_neopixel.reset();
        send_timer_neopixel.start();
    }
}
