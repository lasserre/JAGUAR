# CopyFileToSerialPort.py
# -----------------------
# Description:  Test driver to send contents of filename byte-by-byte out to
#               a serial port. Intended to be used for testing MSP430 routing
#               software.

# Config parameters ----------------------
SERIAL_PORT = "/dev/cu.usbserial-A600eIjc"
BAUD = 57600
# ----------------------------------------

import time, serial, sys

# get user inputs
if (len(sys.argv) < 2):
	print "Error! A filename must be specified"
	sys.exit(1)
filename = sys.argv[1]

if (len(sys.argv) >= 3):
	loops = int(sys.argv[2])
else:
	loops = 1

try:
	ser = serial.Serial(SERIAL_PORT, BAUD)
except(OSError):
	print "Error! Could not connect to port \"" + SERIAL_PORT + "\""
	sys.exit(1)

print "Hex  ASCII"
print "---  -----"

count = 0
while loops == 0 or count < loops:
	f = open(filename, "rb")

	try:
		byte = f.read(1)
		while byte != "":
			# byte = f.read(1)
			print " " + byte.encode("hex").upper() + (" " * 6) + byte
			# print "before:", ser.outWaiting()
			ser.write(byte)
			# print "after:", ser.outWaiting()
			# raw_input("")
			# time.sleep(0.05)
			byte = f.read(1)
	finally:
		f.close()

	count += 1

ser.close()
