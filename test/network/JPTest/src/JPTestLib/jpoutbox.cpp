#include "jpoutbox.h"

JPOutbox::JPOutbox(QObject *parent) : currentPacketData(QByteArray())
{

}

bool JPOutbox::MoreToSend() const
{
    return (!currentPacketData.isEmpty());
}

QByteArray JPOutbox::GetSendNextPacket()
{
    // Send entire packet
    QByteArray outgoingBytes = currentPacketData.remove(0, currentPacketData.length());

    return outgoingBytes;
}

void JPOutbox::SetNextPacket(const QByteArray &NextPacket)
{
    currentPacketData = NextPacket;
    return;
}
