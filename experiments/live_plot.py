import serial
import matplotlib.pyplot as plt
from collections import deque
from matplotlib.animation import FuncAnimation

PORT = "COM4"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)

WINDOW = 500

ir_data = deque(maxlen=WINDOW)
red_data = deque(maxlen=WINDOW)

fig, ax = plt.subplots()

line_ir, = ax.plot([], [], label="IR")
line_red, = ax.plot([], [], label="RED")

ax.legend()
ax.grid(True)


def update(frame):

    while ser.in_waiting:

        try:
            line = ser.readline().decode().strip()

            if "," not in line:
                continue

            ir, red = map(int, line.split(","))

            ir_data.append(ir)
            red_data.append(red)

        except:
            pass

    if len(ir_data) > 10:

        line_ir.set_data(range(len(ir_data)), ir_data)
        line_red.set_data(range(len(red_data)), red_data)

        ax.relim()
        ax.autoscale_view()

    return line_ir, line_red


ani = FuncAnimation(
    fig,
    update,
    interval=20,
    cache_frame_data=False
)

plt.show()