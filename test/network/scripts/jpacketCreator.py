# jpacketCreator.py
#
# This script generates a JAGUAR packet with destination ID,
# source ID, and payload length specified and saves it with
# the filename given. The payload is a repeating sequence of
# ASCII numbers (1234567890).

import os.path, sys

ARG_FORMAT = "<filename> <destination ID> <source ID> <payload length>"

DIRECTORY = "../JPTest/jptestfiles/jpackets"
PACKET_EXT = ".jp"

JAGUAR_HEADER_LEN = 2
MAVLINK_HEADER_LEN = 6
TAIL_LEN = 2

JAGUAR_HEADER_OFFSET = 0
DEST_ID_OFFSET = 1
MAVLINK_HEADER_OFFSET = 2
LEN_OFFSET = 3
SRC_OFFSET = 5

STX = 0xFE

def formatFilename(filename):
	if len(filename) < 3 or filename[-3:] != PACKET_EXT:
		filename += PACKET_EXT
	filename = os.path.join(DIRECTORY, filename)
	return filename

def createHeader(destId, srcId, length):
	header = bytearray(JAGUAR_HEADER_LEN + MAVLINK_HEADER_LEN)
	header[JAGUAR_HEADER_OFFSET] = STX
	header[DEST_ID_OFFSET] = destId
	header[MAVLINK_HEADER_OFFSET] = STX
	header[LEN_OFFSET] = length
	header[SRC_OFFSET] = srcId

	return header

def createPayload(length):
	payload = bytearray(length)

	data = (0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30)

	j = 0
	for i in range(len(payload)):
		payload[i] = data[j]
		if j == len(data) - 1:
			j = 0
		else:
			j += 1

	return payload

def createTail():
	# calculate checksum in the future
	tail = bytearray(TAIL_LEN)
	tail[0] = tail[1] = 0x11

	return tail

def createPacket(destId, srcId, length):
	return createHeader(destId, srcId, length) + createPayload(length) + createTail()

def main():
	if len(sys.argv) <= 4:
		print(ARG_FORMAT)
		sys.exit(1)

	filename = formatFilename(sys.argv[1])
	destId = int(sys.argv[2])
	if destId < 0 or destId > 255:
		print "Error! The destination ID must be from 0 to 255"
		sys.exit(1)
	srcId = int(sys.argv[3])
	if srcId < 0 or srcId > 255:
		print "Error! The source ID must be from 0 to 255"
		sys.exit(1)
	length = int(sys.argv[4])
	if length < 0 or length > 255:
		print "Error! The payload length must be from 0 to 255"
		sys.exit(1)

	f = open(filename, "wb")

	f.write(createPacket(destId, srcId, length))

	f.close()

# run main
main()
