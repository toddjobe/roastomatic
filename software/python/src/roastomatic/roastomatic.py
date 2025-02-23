# standard packages
from datetime import datetime

# 3rd party packages
import serial

ser = serial.Serial('COM6', 115200, timeout=1)  # Change to your port
start_time = datetime.now().strftime("%Y%m%dT%H%M%S")
with open(f"data/roastomatic_{start_time}.txt", "w") as f:
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            print(line)  # Optional: Show output in console
            f.write(line + '\n')
