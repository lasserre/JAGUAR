# CopyFileToSerialPort.py
# -----------------------
# Description:  Test driver to send contents of FILENAME byte-by-byte out to
#               a serial port. Intended to be used for testing MSP430 routing
#               software.

# Config parameters ----------------------
FILENAME = "ENTER_FILENAME_HERE"
SERIAL_PORT = "/dev/cu.usbserial-A600eIjc"
BAUD = 57600
# ----------------------------------------

import serial
ser = serial.Serial(SERIAL_PORT, BAUD)
f = open(FILENAME, "rb")

try:
    byte = f.read(1)
    while byte != "":
        # byte = f.read(1)
        print str('printing ' + byte)
        ser.write(byte)
        byte = f.read(1)
finally:
    f.close()

ser.close()
