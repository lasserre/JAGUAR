import serial

ser = serial.Serial(port="/dev/ttyO4", baudrate=9600, timeout=3.0)

while True:
	rcv = ser.read(7)
	ser.write("1234567890")
	rcv = ser.read(18)

