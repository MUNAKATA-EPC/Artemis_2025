#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>

#include "BNO055.hpp"
#include "button.hpp"

int raw_deg;
int reset_deg;

int degrees;

void BNO055::init(int reset_pin, int sub_pin = -1)
{
    if (!bno.begin(OPERATION_MODE_IMUPLUS)) // センサの初期化
    {
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1);
    }

    bno.setExtCrystalUse(true);

    reset.init(reset_pin, Button::Button_Value_Type::PULLDOWN);
    if(sub_pin != -1)
        reset_s.init(sub_pin, Button::Button_Value_Type::PULLDOWN);
}

void BNO055::process()
{
    reset.loop();

    raw_deg = bno.getVector(Adafruit_BNO055::VECTOR_EULER).x();

    degrees = (int)(raw_deg - reset_deg + 360) % 360;

    if(reset.is_pushed() || (reset_s.is_init() ? reset_s.is_pushed() : false))
    {
        reset_deg = raw_deg;
    }
}

int BNO055::get_degrees()
{
    return degrees;
}