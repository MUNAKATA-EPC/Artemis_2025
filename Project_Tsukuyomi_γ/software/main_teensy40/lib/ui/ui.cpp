#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#include "ui.hpp"
#include "sensor_variables.hpp"

#include "button.hpp"
#include "timer.hpp"
#include "kicker.hpp"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Button btEnter;
Button btLeft;
Button btRight;

extern Kicker kicker;

//データ送信部
Timer send_timer;
int next_send_time = 0;

int now_floor = 1;
int now_select_mode = 0;
int parent_floor;

typedef enum{
    RUN,
    SENSOR,
    TOTAL,
    TEST,
    CONFIG,
} FIRST_FLOOR;

typedef enum{
    RUN_BACK = 0,
    RUN_AT_GYRO,
    RUN_AT_YELLOW,
    RUN_AT_BLUE,
    RUN_DF_GYRO,
    RUN_DF_YELLOW,
    RUN_DF_BLUE,
    RUN_TOTAL,

    SENSOR_BACK = 20,
    SENSOR_OMNI_CAM,
    SENSOR_FRONT_CAM,
    SENSOR_BACK_CAM,
    SENSOR_LINE,
    SENSOR_GYRO,
    SENSOR_TOUCH,
    SENSOR_TOTAL,

    TEST_BACK = 30,
    TEST_KICKER,
    TEST_DRIBBLER,
    TEST_LINE,
    TEST_TOTAL,

    CONFIG_BACK = 40,
    CONFIG_SPEED,
    CONFIG_TOTAL,
} SECOND_FLOOR;

const unsigned char artemis_logo_bmp[128 * 64 / 8] =
{ 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000011, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00111111, 0b11111111, 0b11100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    
    0b00000000, 0b00000001, 0b11111111, 0b11111111, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000011, 0b11111111, 0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00001111, 0b11111111, 0b11111111, 0b11111111, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00011111, 0b11111111, 0b11100000, 0b00000111, 0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00111111, 0b11111111, 0b00000000, 0b00000000, 0b11110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b01111111, 0b11111100, 0b00000000, 0b00000000, 0b00111000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b11111111, 0b11110000, 0b00000000, 0b00000000, 0b00011100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000001, 0b11111111, 0b11100000, 0b00000000, 0b00000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    
    0b00000011, 0b11111111, 0b11000000, 0b00000000, 0b00000000, 0b00000010, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000111, 0b11111111, 0b10000000, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000111, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00001111, 0b11111110, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00001111, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00011111, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00011111, 0b11111000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00111111, 0b11111000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    
    0b00111111, 0b11110000, 0b00000001, 0b11000000, 0b00011111, 0b11111110, 0b00000111, 0b11111111, 0b11001111, 0b11111110, 0b00011110, 0b00000011, 0b11100001, 0b11000001, 0b11111111, 0b11111100, 
    0b00111111, 0b11110000, 0b00000011, 0b11100000, 0b00011111, 0b11111111, 0b00000111, 0b11111111, 0b11001111, 0b11111110, 0b00011110, 0b00000011, 0b11100001, 0b11000001, 0b11111111, 0b11111100, 
    0b00111111, 0b11110000, 0b00000111, 0b11110000, 0b00011111, 0b11111111, 0b10000111, 0b11111111, 0b11001111, 0b11111110, 0b00011111, 0b00000111, 0b11100001, 0b11000001, 0b11111111, 0b11111100, 
    0b01111111, 0b11110000, 0b00000111, 0b01110000, 0b00011100, 0b00000111, 0b10000000, 0b00111000, 0b00000000, 0b00000000, 0b00011111, 0b00000111, 0b11100001, 0b11000001, 0b00000000, 0b00000000, 
    0b01111111, 0b11100000, 0b00000110, 0b00110000, 0b00011100, 0b00000011, 0b10000000, 0b00111000, 0b00000000, 0b00000000, 0b00011111, 0b10001110, 0b11100001, 0b11000001, 0b00000000, 0b00000000, 
    0b01111111, 0b11100000, 0b00001110, 0b00111000, 0b00011100, 0b00000011, 0b10000000, 0b00111000, 0b00000000, 0b00000000, 0b00011101, 0b10001100, 0b11100001, 0b11000001, 0b00000000, 0b00000000, 
    0b01111111, 0b11100000, 0b00001110, 0b00111000, 0b00011100, 0b00000111, 0b10000000, 0b00111000, 0b00001111, 0b11111110, 0b00011101, 0b11011100, 0b11100001, 0b11000001, 0b11111111, 0b11111100, 
    0b01111111, 0b11100000, 0b00001100, 0b00011000, 0b00011100, 0b11111111, 0b10000000, 0b00111000, 0b00001111, 0b11111110, 0b00011100, 0b11011000, 0b11100001, 0b11000001, 0b11111111, 0b11111100, 
    
    0b01111111, 0b11100000, 0b00011100, 0b00011100, 0b00011100, 0b11111111, 0b00000000, 0b00111000, 0b00001111, 0b11111110, 0b00011100, 0b11111000, 0b11100001, 0b11000001, 0b11111111, 0b11111100, 
    0b01111111, 0b11100000, 0b00011100, 0b00011100, 0b00011100, 0b11111110, 0b00000000, 0b00111000, 0b00000000, 0b00000000, 0b00011100, 0b11110000, 0b11100001, 0b11000000, 0b00000000, 0b00011100, 
    0b01111111, 0b11100000, 0b00111000, 0b00001110, 0b00011100, 0b11110000, 0b00000000, 0b00111000, 0b00000000, 0b00000000, 0b00011100, 0b01110000, 0b11100001, 0b11000000, 0b00000000, 0b00011100, 
    0b01111111, 0b11110000, 0b00111000, 0b00001110, 0b00011100, 0b01111000, 0b00000000, 0b00111000, 0b00000000, 0b00000000, 0b00011100, 0b01111000, 0b11100001, 0b11000000, 0b00000000, 0b00011100, 
    0b00111111, 0b11110000, 0b00111000, 0b00001110, 0b00011100, 0b00111100, 0b00000000, 0b00111000, 0b00001111, 0b11111110, 0b00011100, 0b00111000, 0b11100001, 0b11000001, 0b11111111, 0b11111100, 
    0b00111111, 0b11110000, 0b01110000, 0b00000111, 0b00011100, 0b00011110, 0b00000000, 0b00111000, 0b00001111, 0b11111110, 0b00011100, 0b00111100, 0b11100001, 0b11000001, 0b11111111, 0b11111100, 
    0b00111111, 0b11110000, 0b01110000, 0b00000111, 0b00011100, 0b00001111, 0b10000000, 0b00111000, 0b00001111, 0b11111110, 0b00011100, 0b00011100, 0b11100001, 0b11000001, 0b11111111, 0b11111100, 
    0b00111111, 0b11111000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    
    0b00011111, 0b11111000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00011111, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001010, 0b10101010, 0b01001010, 0b01001110, 0b01000000, 0b11100011, 0b10001110, 0b00000000, 
    0b00001111, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00001110, 0b10101110, 0b10101100, 0b10100100, 0b10100000, 0b11100011, 0b10001000, 0b00000000, 
    0b00001111, 0b11111110, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00001010, 0b11101010, 0b10101010, 0b10100100, 0b10100100, 0b11101010, 0b00101110, 0b10000000, 
    0b00000111, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000111, 0b11111111, 0b10000000, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000011, 0b11111111, 0b11000000, 0b00000000, 0b00000000, 0b00000010, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000001, 0b11111111, 0b11100000, 0b00000000, 0b00000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    
    0b00000000, 0b11111111, 0b11110000, 0b00000000, 0b00000000, 0b00011100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b01111111, 0b11111100, 0b00000000, 0b00000000, 0b00111000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00111111, 0b11111111, 0b00000000, 0b00000000, 0b11110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00011111, 0b11111111, 0b11100000, 0b00000111, 0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00001111, 0b11111111, 0b11111111, 0b11111111, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000011, 0b11111111, 0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000001, 0b11111111, 0b11111111, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00111111, 0b11111111, 0b11100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    
    0b00000000, 0b00000000, 0b00000011, 0b11111110, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
};

bool is_selected()
{
    return btEnter.is_pushed();
}

void ui_init()
{
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);

    display.setRotation(0);

    display.clearDisplay();
    display.drawBitmap(0, -3, artemis_logo_bmp, 128, 64, SSD1306_WHITE);

    display.setCursor(64 - 8*5, 56);
    display.println("setup now...");

    display.display();

    btLeft.init(6, Button::Button_Value_Type::PULLDOWN);
    btEnter.init(5, Button::Button_Value_Type::PULLDOWN);
    btRight.init(4, Button::Button_Value_Type::PULLDOWN);

    send_timer.start();
}

void ui_process()
{
    send_timer.tick();

    btEnter.loop();
    btLeft.loop();
    btRight.loop();

    if(is_selected())
    {
        tone(3, 4500, 15);
    }
    
    //if(!is_able_to_run && (parent_floor == (int)FIRST_FLOOR::TEST ? true : false))
    {
        if(btLeft.is_pushed())
        {
            tone(3, 4000, 15);

            if(now_floor == 1)
            {
                now_select_mode = (now_select_mode - 1 + FIRST_FLOOR::TOTAL) % FIRST_FLOOR::TOTAL;
            }
            else
            {
                if(parent_floor == (int)FIRST_FLOOR::RUN)
                {
                    now_select_mode = (now_select_mode - 1 + SECOND_FLOOR::RUN_TOTAL) % SECOND_FLOOR::RUN_TOTAL;
                }
                else if(parent_floor == (int)FIRST_FLOOR::SENSOR)
                {
                    now_select_mode = (now_select_mode - 1 - 20 + (SECOND_FLOOR::SENSOR_TOTAL - 20)) % (SECOND_FLOOR::SENSOR_TOTAL - 20) + 20;
                }
                else if(parent_floor == (int)FIRST_FLOOR::TEST)
                {
                    now_select_mode = (now_select_mode - 1 - 30 + (SECOND_FLOOR::TEST_TOTAL - 30)) % (SECOND_FLOOR::TEST_TOTAL - 30) + 30;
                }
                else if(parent_floor == (int)FIRST_FLOOR::CONFIG)
                {
                    now_select_mode = (now_select_mode - 1 - 40 + (SECOND_FLOOR::CONFIG_TOTAL - 40)) % (SECOND_FLOOR::CONFIG_TOTAL - 40) + 40;
                }
            }
        }
        if(btRight.is_pushed())
        {
            tone(3, 4000, 15);

            if(now_floor == 1)
            {
                now_select_mode = (now_select_mode + 1) % FIRST_FLOOR::TOTAL;
            }
            else
            {
                if(parent_floor == (int)FIRST_FLOOR::RUN)
                {
                    now_select_mode = (now_select_mode + 1) % SECOND_FLOOR::RUN_TOTAL;
                }
                else if(parent_floor == (int)FIRST_FLOOR::SENSOR)
                {
                    now_select_mode = (now_select_mode + 1 - 20 + (SECOND_FLOOR::SENSOR_TOTAL - 20)) % (SECOND_FLOOR::SENSOR_TOTAL - 20) + 20;
                }
                else if(parent_floor == (int)FIRST_FLOOR::TEST)
                {
                    now_select_mode = (now_select_mode + 1 - 30 + (SECOND_FLOOR::TEST_TOTAL - 30)) % (SECOND_FLOOR::TEST_TOTAL - 30) + 30;
                }
                else if(parent_floor == (int)FIRST_FLOOR::CONFIG)
                {
                    now_select_mode = (now_select_mode + 1 - 40 + (SECOND_FLOOR::CONFIG_TOTAL - 40)) % (SECOND_FLOOR::CONFIG_TOTAL - 40) + 40;
                }
            }
        }
    }

    display.clearDisplay();


    if(now_floor == 1)
    {
        if(now_select_mode == (int)FIRST_FLOOR::RUN)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("> RUN");

            if(is_selected())
            {
                now_floor = 2;
                parent_floor = (int)FIRST_FLOOR::RUN;
                now_select_mode = (int)SECOND_FLOOR::RUN_BACK;
            }
        }
        else if(now_select_mode == (int)FIRST_FLOOR::SENSOR)
        { 
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("> SENSOR");
            display.startscrolldiagleft(0, 100);

            if(is_selected())
            {
                now_floor = 2;
                parent_floor = (int)FIRST_FLOOR::SENSOR;
                now_select_mode = (int)SECOND_FLOOR::SENSOR_BACK;
            }
        }
        else if(now_select_mode == (int)FIRST_FLOOR::TEST)
        { 
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("> TEST");

            if(is_selected())
            {
                now_floor = 2;
                parent_floor = (int)FIRST_FLOOR::TEST;
                now_select_mode = (int)SECOND_FLOOR::TEST_BACK;
            }
        }
        else if(now_select_mode == (int)FIRST_FLOOR::CONFIG)
        { 
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("> CONFIG");

            if(is_selected())
            {
                now_floor = 2;
                parent_floor = (int)FIRST_FLOOR::CONFIG;
                now_select_mode = (int)SECOND_FLOOR::CONFIG_BACK;
            }
        }
    }
    else if(now_floor == 2)
    {
        if(parent_floor == FIRST_FLOOR::RUN)
        {
            if(now_select_mode == SECOND_FLOOR::RUN_BACK)
            {
                display.setTextSize(2);
                display.setCursor(0, 35);
                display.println("BACK TO HOME");

                if(is_selected())
                {
                    now_floor = 1;
                    now_select_mode = (int)FIRST_FLOOR::RUN;
                }
            }
            else if(now_select_mode == SECOND_FLOOR::RUN_AT_GYRO)
            {
            }
            else if(now_select_mode == SECOND_FLOOR::RUN_AT_YELLOW)
            {
                
            }
            else if(now_select_mode == SECOND_FLOOR::RUN_AT_BLUE)
            {
                
            }
            else if(now_select_mode == SECOND_FLOOR::RUN_DF_GYRO)
            {

            }
            else if(now_select_mode == SECOND_FLOOR::RUN_DF_YELLOW)
            {
                
            }
            else if(now_select_mode == SECOND_FLOOR::RUN_DF_BLUE)
            {
                
            }
        }
        else if(parent_floor == FIRST_FLOOR::SENSOR)
        {
            if(now_select_mode == SECOND_FLOOR::SENSOR_BACK)
            {
                display.setTextSize(2);
                display.setCursor(0, 35);
                display.println("BACK TO HOME");

                if(is_selected())
                {
                    now_floor = 1;
                    now_select_mode = (int)FIRST_FLOOR::SENSOR;
                }
            }
            else if(now_select_mode == SECOND_FLOOR::SENSOR_OMNI_CAM)
            {
                display.setTextSize(1);
                display.setCursor(0, 20);
                display.println("OMNI CAM");
                display.setCursor(0, 30);
                display.println("ball:" + String(cam_ball_deg)          + "," + String(cam_ball_distance));
                display.setCursor(0, 40);
                display.println("yell:" + String(cam_goal_yellow_deg)   + "," + String(cam_goal_yellow_distance));
                display.setCursor(0, 50);
                display.println("blue:" + String(cam_goal_blue_deg)     + "," + String(cam_goal_blue_distance));
            }
            else if(now_select_mode == SECOND_FLOOR::SENSOR_FRONT_CAM)
            {
                display.setTextSize(1);
                display.setCursor(0, 20);
                display.println("FRONT CAM");
                display.setCursor(0, 30);
                display.println("ball:" + String(fcam_ball_deg)          + "," + String(fcam_ball_distance));
                display.setCursor(0, 40);
                display.println("yell:" + String(fcam_goal_yellow_deg)   + "," + String(fcam_goal_yellow_distance));
                display.setCursor(0, 50);
                display.println("blue:" + String(fcam_goal_blue_deg)     + "," + String(fcam_goal_blue_distance));
            }
            else if(now_select_mode == SECOND_FLOOR::SENSOR_BACK_CAM)
            {
                display.setTextSize(1);
                display.setCursor(0, 20);
                display.println("BACK CAM");
                display.setCursor(0, 30);
                display.println("ball:" + String(bcam_ball_deg)          + "," + String(bcam_ball_distance));
                display.setCursor(0, 40);
                display.println("yell:" + String(bcam_goal_yellow_deg)   + "," + String(bcam_goal_yellow_distance));
                display.setCursor(0, 50);
                display.println("blue:" + String(bcam_goal_blue_deg)     + "," + String(bcam_goal_blue_distance));
            }
            else if(now_select_mode == SECOND_FLOOR::SENSOR_GYRO)
            {
                display.setTextSize(1);
                display.setCursor(0, 20);
                display.println("GYRO");
                display.setCursor(0, 30);
                display.println("value:" + String(gyro_deg));
            }
            else if(now_select_mode == SECOND_FLOOR::SENSOR_LINE)
            {

            }
            else if(now_select_mode == SECOND_FLOOR::SENSOR_TOUCH)
            {

            }
        }
        else if(parent_floor == FIRST_FLOOR::TEST)
        {

        }
        else if(parent_floor == FIRST_FLOOR::CONFIG)
        {

        }
    }
        
    if(send_timer.get_value() >= next_send_time)
    {
        display.display();

        next_send_time = send_timer.get_value() + 100;
    }
}