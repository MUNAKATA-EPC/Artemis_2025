import sensor, image, time, lcd
from machine import UART
from fpioa_manager import fm

threshold_for_ball = (42, 95, -22, 121, 35, 99)
screen_center = [172, 128]                  # 画面の中央座標

lcd.init(freq=15000000)

sensor.reset(dual_buff=True)
sensor.set_pixformat(sensor.RGB565)#カラースケール
sensor.set_framesize(sensor.QVGA)#解像度Ss
sensor.set_contrast(1)#コントラスト
sensor.set_brightness(3)#明るさ
sensor.set_saturation(3)#彩3~-3
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_exposure(False)
#sensor.set_auto_whitebal(True)
sensor.skip_frames(time = 2000)

clock = time.clock()                # Create a clock object to track the FPS.

fm.register(35, fm.fpioa.UART1_TX, force=True)
fm.register(34, fm.fpioa.UART1_RX, force=True)

uart = UART(UART.UART1, 115200, 8, None, 1, timeout= 1000)

while(True):
    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot()         # Take a picture and return the image.

    lcd.display(img)                # Display on LCD



    read_count_goal_yellow       =   0
    read_count_goal_yellow       =   0
    maximum_cx_goal_yellow      =   0                               # 黄ゴールの最大色取りサイズの中心x座標
    maximum_cy_goal_yellow      =   0                               # 黄ゴールの最大色取りサイズの中心y座標
    maximum_area_goal_yellow    =   0                               # 黄ゴールの最大色取りサイズのエリアサイズ

    cx_goal_yellow              =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 黄ゴールの中心x座標保存用配列
    cy_goal_yellow              =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 黄ゴールの中心y座標保存用配列
    area_goal_yellow            =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 黄ゴールの色取りエリア保存用配列

    for blob in img.find_blobs([threshold_for_ball], pixels_threshold=5, area_threshold=5, merge=True,  margin=5):
        if read_count_goal_yellow + 1 >= 10:              # コートの色を10回以上取った場合、それ以上コートの色取りをしない。
            break
        else:                                   # まだコートの色取りが10回行われていない場合、読み取り回数を増やす。
            read_count_goal_yellow += 1

        cx_goal_yellow[read_count_goal_yellow] = blob.cx()
        cy_goal_yellow[read_count_goal_yellow] = blob.cy()
        area_goal_yellow[read_count_goal_yellow] = blob.area()

    maximum_cx_goal_yellow = (max(cx_goal_yellow[:]))
    maximum_cy_goal_yellow = (max(cy_goal_yellow[:]))
    maximum_area_goal_yellow = (max(area_goal_yellow[:]))
    img.draw_line(screen_center[0], screen_center[1], maximum_cx_goal_yellow, maximum_cy_goal_yellow, thickness=4)

    #print((maximum_cx_goal_yellow) / 320.0 * 60 - 30)
    uart.write(str(clock.fps()))

    print(clock.fps())
