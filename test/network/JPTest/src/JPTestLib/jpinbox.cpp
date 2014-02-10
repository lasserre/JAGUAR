#include "jpinbox.h"

JPInbox::JPInbox(QObject* parent) : packetInspector(new JPacketChecker())
  , byteInbox(QByteArray())
{
}

JPInbox::~JPInbox()
{
    delete packetInspector;
}

JPacketDiffResults JPInbox::CheckMail()
{
    QByteArray contents(byteInbox);
    byteInbox.clear();
    return packetInspector->CheckNewBytes(contents);
}

/**
 * @brief JPInbox::WaitingOnP2Data
 * @return true if we are waiting for more bytes of current packet to come in
 */
bool JPInbox::WaitingOnP2Data() const
{
    return !packetInspector->ReadyForNextP2Packet();
}

bool JPInbox::WaitingOnP3Data() const
{
    return !packetInspector->ReadyForNextP3Packet();
}

void JPInbox::AddToInbox(const QByteArray &Bytes)
{
    byteInbox.append(Bytes);
    return;
}

void JPInbox::SetNextP2ExpectedPacket(const JPacket &NextP2Packet)
{
    packetInspector->SetNextExpectedP2Packet(NextP2Packet);
    return;
}

void JPInbox::SetNextP3ExpectedPacket(const JPacket &NextP3Packet)
{
    packetInspector->SetNextExpectedP3Packet(NextP3Packet);
    return;
}
