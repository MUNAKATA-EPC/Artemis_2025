import sensor, image, time, math,lcd
import Maix
from fpioa_manager import fm
from machine import UART
from Maix import GPIO



#定数定義
WIDTH = const(160)
HEIGHT = const(120)
ANGLE_CONVERSION = 90 / WIDTH
GOAL_ANGLE_CONVERSION = 3.55555
PROXIMITY_HEIGHT = const(110)
PROXIMITY_CNT_NUM = const(10)
fm.register(35, fm.fpioa.UART1_TX, force=True)
fm.register(34, fm.fpioa.UART1_RX, force=True)

uart = UART(UART.UART1, 115200, 8, None, 1, timeout= 1000)

clock = time.clock()

#ホモグラフィー変換行列
homegraphy_matrix = [   [ 5.93750000e+00,  0.00000000e+00,  0.00000000e+00],
                        [ 0.00000000e+00, -6.11111111e+00,  1.46666667e+03],
                        [ 0.00000000e+00,  3.91666667e-01,  1.00000000e+00]]
#センサーの設定

sensor.reset(dual_buff=True)
sensor.set_pixformat(sensor.RGB565)#カラースケール
sensor.set_framesize(sensor.QQVGA)#解像度Ss
sensor.skip_frames(time = 400)
sensor.set_contrast(0)#コントラスト
sensor.set_brightness(-3)#明るさ
sensor.set_saturation(3)#彩3~-3
sensor.skip_frames(time = 250)
sensor.set_auto_gain(False) # must be turned off for color tracking
#sensor.set_auto_exposure(False, exposure_us = 1750)
#sensor.set_auto_whitebal(True, (-3, -0.5, -0.5))
sensor.set_auto_whitebal(False, rgb_gain_db = (40,  32, 55))
#sensor.__write_reg(0x13, 0x00001000)
#sensor.set_jb_quality(1)

sensor.skip_frames(time = 200)

#各閾値
ball_thresholds = [(36, 74, 36, 85, 51, 88)]
y_goal_thresholds = [(23, 62, -24, -5, 26, 63)]
b_goal_thresholds = [(30, 46, 6, 75, -93, -29)]



#変数定義
proximity_cnt = [0] * 7
proximity = [0] * 7


def MatrixMultiply(matrix, vector): #行列の積を計算する関数
    result = [[sum(a * b for a, b in zip(row, col)) for col in zip(*vector)] for row in matrix]
    return result

def CheckGreen(x, y):
    def Check(thresholds, lab):
        l_min, l_max, a_min, a_max, b_min, b_max = thresholds[0]
        return 1 if l_min < lab[0] < l_max and a_min < lab[1] < a_max and b_min < lab[2] < b_max else 0

    bit = []
    for dx in [-3, 0, 3]:
        for dy in [-3, 0, 3]:
            bit.append(Check(court_thresholds, image.rgb_to_lab(img.get_pixel(x + dx, y + dy))))

    return 0 if any(bit) else 1

def HomographyProjection(center_x, center_y): #ホモグラフィー変換をする関数
    #カメラ座標でのコートベクトル
    camera_vector = [[center_x],
                     [center_y],
                     [1       ]]

    #ワールド座標に変換
    coordinate = MatrixMultiply(homegraphy_matrix, camera_vector)
    world_vector = [[coordinate[0][0] / coordinate[2][0]],
                    [coordinate[1][0] / coordinate[2][0]]]
    return world_vector

while True:

    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot() #映像の取得

    #img.draw_rectangle(0, 0, 320, 50, (0, 0, 0), 0, True)
    #img.draw_rectangle(0, 190, 320, 240, (0, 0, 0), 0, True)

    #ボールを見つける
    ball_rectarray = []
    ball_x = 0
    ball_y = 0

    #黄色ゴールを見つける
    y_goal_rectarray = []
    y_goal_x = 0
    y_goal_y = 0
    y_goal_width = 0
    y_goal_hight = 0

    #青色ゴールを見つける
    b_goal_rectarray = []
    b_goal_x = 0
    b_goal_y = 0
    b_goal_width = 0
    b_goal_hight = 0

    for blob in img.find_blobs(ball_thresholds, pixel_threshold = 4, area_threshold = 4, merge = True, margin = 10):
        if blob[2] < 150:
            ball_rectarray.append(list(blob.rect()))     #見つかった閾値内のオブジェクトをリストに格納
    try:
        ball_maxrect = max(ball_rectarray, key = lambda x: x[1])    #配列の中から一番画面の下にあるものを選定
        ball_x = ball_maxrect[0] + (ball_maxrect[2] * 0.5)  #中心のx座標の算出
        ball_y = ball_maxrect[1] + (ball_maxrect[3] * 0.5)  #中心のy座標の算出
        img.draw_circle(int(ball_x), int(ball_y), int((ball_maxrect[2] * 0.5 + ball_maxrect[3] * 0.5) * 0.5))
        check_ball =  1;

    except ValueError as err:   #オブジェクトがひとつも見つからなかった場合の例外処理
           check_ball =  0;
           pass

    for blob in img.find_blobs(y_goal_thresholds,  pixel_threshold = 100, area_threshold = 100):
        y_goal_rectarray.append(list(blob.rect()))     #見つかった閾値内のオブジェクトをリストに格納

    try:
        y_goal_maxrect = max(y_goal_rectarray, key = lambda x: x[2] * x[3])    #配列の中から面積の一番大きい物を選定
        y_goal_x = y_goal_maxrect[0] + (y_goal_maxrect[2] * 0.5)  #中心のx座標の算出
        y_goal_y = y_goal_maxrect[1] + (y_goal_maxrect[3] * 0.5)
        img.draw_rectangle(y_goal_maxrect)     #オブジェクトを囲う四角形の描画
        check_y_goal =  1;

    except ValueError as err:   #オブジェクトがひとつも見つからなかった場合の例外処理
           check_y_goal =  0;
           pass


    for blob in img.find_blobs(b_goal_thresholds, pixel_threshold = 100, area_threshold = 100, merge = True, margin = 75):
        b_goal_rectarray.append(list(blob.rect()))     #見つかった閾値内のオブジェクトをリストに格納

    try:
        b_goal_maxrect = min(b_goal_rectarray, key = lambda x: x[2] * x[3])    #配列の中から面積の一番大きい物を選定
        b_goal_x = b_goal_maxrect[0] + (b_goal_maxrect[2] * 0.5)  #中心のx座標の算出
        b_goal_y = b_goal_maxrect[1] + (b_goal_maxrect[3] * 0.5)
        img.draw_rectangle(b_goal_maxrect)     #オブジェクトを囲う四角形の描画
        check_b_goal =  1;


    except ValueError as err:   #オブジェクトがひとつも見つからなかった場合の例外処理
           check_b_goal =  0;
           pass

    #カメラ座標でのボールベクトル
    world_ball_vector = HomographyProjection(ball_x - 160,ball_y)
    world_y_goal_vector = HomographyProjection(y_goal_x - 160,y_goal_y)
    world_b_goal_vector = HomographyProjection(b_goal_x - 160,b_goal_y)


    if(check_ball == 1):
        ball_dir = int(ball_x * ANGLE_CONVERSION) #ボールの角度を求める
        ball_dis = int(math.sqrt((world_ball_vector[0][0] - (ball_x - HEIGHT) * 0.0625) ** 2 + world_ball_vector[1][0] ** 2) * 2) * (1.0 / (abs(math.cos(math.radians(ball_dir - 45))) ** 0.5))
    else:
        ball_dir = 500
        ball_dis = 500

    if(check_y_goal == 1):
        y_goal_dir = (int(y_goal_x ) / WIDTH * 90) - 45
        y_goal_dis = int(math.sqrt((world_y_goal_vector[0][0] - (y_goal_x - HEIGHT) * 0.0625) ** 2 + world_y_goal_vector[1][0] ** 2) * 2) * (1.0 / (abs(math.cos(math.radians(y_goal_dir - 45))) ** 0.5))
    else:
        y_goal_dir = 500
        y_goal_dis = 500

    if(check_b_goal == 1):
        b_goal_dir = (int(b_goal_x) / WIDTH * 90) - 45
        b_goal_dis = int(math.sqrt((world_b_goal_vector[0][0] - (b_goal_x - HEIGHT) * 0.0625) ** 2 + world_b_goal_vector[1][0] ** 2) * 2) * (1.0 / (abs(math.cos(math.radians(b_goal_dir - 45))) ** 0.5))
    else:
        b_goal_dir = 500
        b_goal_dis = 500

    print(ball_dir)

    #uart送信
    uart.write(str(ball_dir))
    uart.write("a")
    uart.write(str(ball_dis))
    uart.write("b")
    uart.write(str(y_goal_dir))
    uart.write("c")
    uart.write(str(y_goal_dis))
    uart.write("d")
    uart.write(str(b_goal_dir))
    uart.write("e")
    uart.write(str(b_goal_dis))
    uart.write("f")
