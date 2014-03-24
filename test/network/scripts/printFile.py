# CopyFileToSerialPort.py
# -----------------------
# Description:  Test driver to send contents of filename byte-by-byte out to
#               a serial port. Intended to be used for testing MSP430 routing
#               software.
# Command-line args: [filename] [# loops] [delay between messages] [delay between bytes]

# Config parameters ----------------------
# SERIAL_PORT = "/dev/cu.usbserial-A600eIjc"
# SERIAL_PORT = "/dev/cu.usbserial-A40081Ez"
SERIAL_PORT = "/dev/cu.usbserial-A603H5JC"
# SERIAL_PORT = "/dev/cu.usbmodemfd121"
BAUD = 9600
# ----------------------------------------

import time, serial, sys

# get user inputs
if (len(sys.argv) < 2):
	print "Error! A filename must be specified"
	sys.exit(1)
filename = sys.argv[1]

byteDelay = 0
msgDelay = 0
loops = 1
if len(sys.argv) >= 3:
	loops = int(sys.argv[2])
if len(sys.argv) >= 4:
	msgDelay = float(sys.argv[3])
if len(sys.argv) >= 5:
	byteDelay = float(sys.argv[4])

try:
	ser = serial.Serial(SERIAL_PORT, BAUD)
	time.sleep(4) # this needs to be uncommented when talking to an Arduino to wait for it to reset :P
except(OSError):
	print "Error! Could not connect to port \"" + SERIAL_PORT + "\""
	sys.exit(1)

print "Hex  ASCII"
print "---  -----"

count = 0
while loops == 0 or count < loops:
	f = open(filename, "rb")

	try:
		byte = str(count) + " " + f.read(1)
		while byte != "":
			# byte = f.read(1)
			print " " + byte.encode("hex").upper() + (" " * 6) + byte
			ser.write(byte)
			# raw_input("")
			time.sleep(byteDelay)
			byte = f.read(1)
	finally:
		f.close()

	f.close()
	count += 1
	time.sleep(msgDelay)

ser.close()
