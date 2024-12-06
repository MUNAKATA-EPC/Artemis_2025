import sensor, image, time, math
from machine import UART
from fpioa_manager import fm

threshold_for_ball = (46, 66, 42, 87, 23, 69)# ボールの色取り用変数
threshold_for_goal_yellow = (75, 85, -32, -12, 35, 64)# ゴールの色取り用変数(黄色)
#threshold_for_goal_yellow = (17, 35, 1, 18, -50, -27)# ゴールの色取り用変数(黄色)
threshold_for_goal_blue = (29, 47, -17, 17, -45, -18)

screen_center = [160, 120]                  # 画面の中央座標

sensor.reset()
sensor.set_pixformat(sensor.RGB565)#カラースケール
sensor.set_framesize(sensor.QVGA)#解像度Ss
sensor.skip_frames(time = 1500)
sensor.set_contrast(3)#コントラスト
sensor.set_brightness(0)#明るさ
sensor.set_saturation(3)#彩3~-3
sensor.skip_frames(time = 250)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_exposure(False)
sensor.set_auto_whitebal(False, rgb_gain_db = (45, 40, 90))
#sensor.set_auto_whitebal(False)

uart = UART(UART.UART1, 115200, 8, None, 1, timeout= 1000)

clock = time.clock()


while(True):
    clock.tick()
    img = sensor.snapshot()
    img.draw_cross(screen_center[0], screen_center[1])    # クロスヘアの表示
    img.draw_circle(screen_center[0], screen_center[1],190,[0,0,0],120)

    #=======================変数定義ライン=======================

    read_count_ball            =   0                               # コートの色を何度取ったかを代入する変数
    read_count_goal_yellow      =   0                               # 黄ゴールの色を何度取ったかを代入する変数
    read_count_goal_blue        =   0                               # 青ゴールの色を何度取ったかを代入する変数

    maximum_cx_ball            =   0                               # コートの最大色取りサイズの中心x座標
    maximum_cy_ball            =   0                               # コートの最大色取りサイズの中心y座標
    maximum_area_ball          =   0                               # コートの最大色取りサイズのエリアサイズ

    maximum_cx_goal_yellow      =   0                               # 黄ゴールの最大色取りサイズの中心x座標
    maximum_cy_goal_yellow      =   0                               # 黄ゴールの最大色取りサイズの中心y座標
    maximum_area_goal_yellow    =   0                               # 黄ゴールの最大色取りサイズのエリアサイズ

    maximum_cx_goal_blue        =   0                               # 青ゴールの最大色取りサイズの中心x座標
    maximum_cy_goal_blue        =   0                               # 青ゴールの最大色取りサイズの中心y座標
    maximum_area_goal_blue      =   0                               # 青ゴールの最大色取りサイズのエリアサイズ

    ball_deg                   =   0                               # 画面中央からのコートの角度
    ball_distance              =   0                               # 画面中央からのコートの距離
    goal_yellow_deg             =   0                               # 画面中央からの黄ゴールの角度
    goal_yellow_distance        =   0                               # 画面中央からの黄ゴールの距離
    goal_blue_deg               =   0                               # 画面中央からの青ゴールの角度
    goal_blue_distance          =   0                               # 画面中央からの青ゴールの距離

    #=======================配列定義ライン=======================

    cx_ball                    =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # コートの中心x座標保存用配列
    cy_ball                    =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # コートの中心y座標保存用配列
    area_ball                  =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # コートの色取りエリア保存用配列

    cx_goal_yellow              =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 黄ゴールの中心x座標保存用配列
    cy_goal_yellow              =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 黄ゴールの中心y座標保存用配列
    area_goal_yellow            =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 黄ゴールの色取りエリア保存用配列

    cx_goal_blue                =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 青ゴールの中心x座標保存用配列
    cy_goal_blue                =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 青ゴールの中心y座標保存用配列
    area_goal_blue              =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 青ゴールの色取りエリア保存用配列

    #=======================黄ゴール色取りライン=======================

    for blob in img.find_blobs([threshold_for_goal_yellow], pixels_threshold=10, area_threshold=10, merge=True,margin=25):
        if read_count_goal_yellow + 1 >= 10:              # コートの色を10回以上取った場合、それ以上コートの色取りをしない。
            break
        else:                                   # まだコートの色取りが10回行われていない場合、読み取り回数を増やす。
            read_count_goal_yellow += 1

        cx_goal_yellow[read_count_goal_yellow] = blob.cx()
        cy_goal_yellow[read_count_goal_yellow] = blob.cy()
        area_goal_yellow[read_count_goal_yellow] = blob.area()

    for blob in img.find_blobs([threshold_for_goal_blue], pixels_threshold=10, area_threshold=10, merge=True,margin=30):
        if read_count_goal_blue + 1 >= 10:              # コートの色を10回以上取った場合、それ以上コートの色取りをしない。
            break
        else:                                   # まだコートの色取りが10回行われていない場合、読み取り回数を増やす。
            read_count_goal_blue += 1

        cx_goal_blue[read_count_goal_blue] = blob.cx()
        cy_goal_blue[read_count_goal_blue] = blob.cy()
        area_goal_blue[read_count_goal_blue] = blob.area()

    for blob in img.find_blobs([threshold_for_ball], pixels_threshold=10, area_threshold=10, merge=True,margin=25):
        if read_count_ball + 1 >= 10:              # コートの色を10回以上取った場合、それ以上コートの色取りをしない。
            break
        else:                                   # まだコートの色取りが10回行われていない場合、読み取り回数を増やす。
            read_count_ball += 1

        cx_ball[read_count_ball] = blob.cx()
        cy_ball[read_count_ball] = blob.cy()
        area_ball[read_count_ball] = blob.area()

    #==============================================

    maximum_cx_goal_yellow = (max(cx_goal_yellow[:]))
    maximum_cy_goal_yellow = (max(cy_goal_yellow[:]))
    maximum_area_goal_yellow = (max(area_goal_yellow[:]))

    for i in range(0, 9):
        if area_goal_yellow[i] == maximum_area_goal_yellow:
            maximum_cx_goal_yellow = cx_goal_yellow[i]
            maximum_cy_goal_yellow = cy_goal_yellow[i]
            break


    img.draw_cross(maximum_cx_goal_yellow, maximum_cy_goal_yellow)    # コートの中心を交差線で描画
    img.draw_line(screen_center[0], screen_center[1], maximum_cx_goal_yellow, maximum_cy_goal_yellow, thickness=4)  # 画面中心からコート中心へのライン描画

    maximum_cx_goal_blue = (max(cx_goal_blue[:]))
    maximum_cy_goal_blue = (max(cy_goal_blue[:]))
    maximum_area_goal_blue = (max(area_goal_blue[:]))

    for i in range(0, 9):
        if area_goal_blue[i] == maximum_area_goal_blue:
            maximum_cx_goal_blue = cx_goal_blue[i]
            maximum_cy_goal_blue = cy_goal_blue[i]
            break

    img.draw_cross(maximum_cx_goal_blue, maximum_cy_goal_blue)    # コートの中心を交差線で描画
    img.draw_line(screen_center[0], screen_center[1], maximum_cx_goal_blue, maximum_cy_goal_blue, thickness=2)  # 画面中心からコート中心へのライン描画

    maximum_cx_ball = (max(cx_ball[:]))
    maximum_cy_ball = (max(cy_ball[:]))
    maximum_area_ball = (max(area_ball[:]))

    for i in range(0, 9):
        if area_ball[i] == maximum_area_ball:
            maximum_cx_ball = cx_ball[i]
            maximum_cy_ball = cy_ball[i]
            break

    #======================計算フェーズ=======================

    #==============================================

    goal_yellow_deg = math.atan2((maximum_cx_goal_yellow - screen_center[0]), (maximum_cy_goal_yellow - screen_center[1]))
    if goal_yellow_deg < 0:
        goal_yellow_deg = (2 * math.pi) - abs(goal_yellow_deg)
    goal_yellow_deg = (math.floor(goal_yellow_deg / (2 * math.pi) * 180))

    goal_yellow_distance = math.sqrt(math.pow((maximum_cx_goal_yellow - screen_center[0]), 2) + math.pow((maximum_cy_goal_yellow - screen_center[1]), 2));


    goal_blue_deg = math.atan2((maximum_cx_goal_blue - screen_center[0]), (maximum_cy_goal_blue - screen_center[1]))
    if goal_blue_deg < 0:
        goal_blue_deg = (2 * math.pi) - abs(goal_blue_deg)
    goal_blue_deg = (math.floor(goal_blue_deg / (2 * math.pi) * 180))

    goal_blue_distance = math.sqrt(math.pow((maximum_cx_goal_blue - screen_center[0]), 2) + math.pow((maximum_cy_goal_blue - screen_center[1]), 2));

    ball_deg = math.atan2((maximum_cx_ball - screen_center[0]), (maximum_cy_ball - screen_center[1]))
    if ball_deg < 0:
        ball_deg = (2 * math.pi) - abs(ball_deg)
    ball_deg = (((math.floor(ball_deg / (2 * math.pi) * 180))+ 450)% 360)

    ball_distance = math.sqrt(math.pow((maximum_cx_ball - screen_center[0]), 2) + math.pow((maximum_cy_ball - screen_center[1]), 2));

    #==============================================

    #======================出力フェーズ=======================

    if maximum_area_goal_yellow == 0:
        goal_yellow_deg = 255
        goal_yellow_distance = 255

    if maximum_area_goal_blue == 0:
        goal_blue_deg = 255
        goal_blue_distance = 255

    if maximum_area_ball == 0:
        ball_deg = 255
        ball_distance = 255

    uart.write(str(ball_deg))
    uart.write("a")
    uart.write(str(ball_distance))
    uart.write("b")
    uart.write(str(goal_yellow_deg))
    uart.write("c")
    uart.write(str(goal_yellow_distance))
    uart.write("d")
    uart.write(str(goal_blue_deg))
    uart.write("e")
    uart.write(str(goal_blue_distance))
    uart.write("f")


    print(ball_deg)
