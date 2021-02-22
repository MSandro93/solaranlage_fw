import serial
import sys
import keyboard
from datetime import datetime

port = input('which port number to open?: ')

try:
    ser = serial.Serial('COM'+port, 9600, timeout=20)
except:
    print('could not open serial port '+port)
    sys.exit(-1)

f = open('log.csv', 'a')

print('start logging. hold down q for at least 20s to terminate.')

while True:

    if keyboard.is_pressed('q'):
        break

    line = ser.readline()
    time = datetime.now().now.strftime("%H:%M:%S")
    f.write(time+';'+line+'\n')
    f.flush()

f.flush()
f.close()
ser.close()





