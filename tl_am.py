import serial
import pyautogui
import time

LEDPIN_NORMAL = 10
LEDPIN_DRAG = 11

# y is the mapped pitch val, x is the mapped roll val
# Do not go to extreme vertical orientations 
# there is Lag between actual command in terminal and motion of air mouse, which increases (!!) when air mouse is used TRIGGER-LESS

ser = serial.Serial('COM6', 9600)

while True:
    try:
        # start = time.time()
        data = ser.readline().decode().strip().split(',')
        led, x, y, l, r = map(int, data)
        move_start = time.time()
        if (led == LEDPIN_NORMAL):
            pyautogui.move(x, y, 0.1)
        if (led == LEDPIN_DRAG):
            pyautogui.drag(x, y, 0.1)
        move_end = time.time()
        if (l == 1):
            pyautogui.click()
        elif (r == 1):
            pyautogui.rightClick()
        print(f"{x}, {y}")
        # end = time.time()
        time_interval = move_end - move_start
        print(f"move time = {time_interval}")
    except ValueError:
        pass
