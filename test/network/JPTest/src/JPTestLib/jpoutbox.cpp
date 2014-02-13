#include "jpoutbox.h"

JPOutbox::JPOutbox(QObject *parent) : currentPacketData(QByteArray())
{

}

bool JPOutbox::MoreToSend() const
{
    return (!currentPacketData.isEmpty());
}

QByteArray JPOutbox::SendNextPacket(const bool& NextByte /* = false */)
{
    QByteArray outgoingBytes;
    int numBytes = currentPacketData.count();

    if (NextByte)
        numBytes = 1;

    outgoingBytes.append(currentPacketData.left(numBytes));
    currentPacketData.remove(0, numBytes);

    return outgoingBytes;
}

bool JPOutbox::SetNextPacket(const QByteArray &NextPacket)
{
    if (MoreToSend())
        return false;       // Return false if we don't set the next packet (still have some bytes left to send)
    else
        currentPacketData = NextPacket;

    return true;            // Return true if we do set the next packet
}
