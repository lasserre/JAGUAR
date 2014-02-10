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
    // Send entire packet for now... (implement step mode here, later on)
    QByteArray outgoingBytes(currentPacketData);
    currentPacketData.remove(0, currentPacketData.length());
    return outgoingBytes;
}

void JPOutbox::SetNextPacket(const QByteArray &NextPacket)
{
    currentPacketData = NextPacket;
    return;
}
