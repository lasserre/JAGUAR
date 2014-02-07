#include "jpinbox.h"

JPInbox::JPInbox() : moreDataToReceive(false)
{
}

void JPInbox::CheckMail()
{
    qDebug() << "Need to implement "  << __FUNCTION__;
    return;
}

bool JPInbox::MoreToReceive() const
{
    return moreDataToReceive;
}

//void JPTest::ProcessInbox()
//{
//    if (currentPacketSrc == -1)
//    {
//        // Get src byte of packet, if it has come in yet
//        if (inbox->count() > (MAVPACKET::SRCBYTE + JaguarHeaderOffset))
//        {
//            currentPacketSrc = (unsigned char) inbox->at(JaguarHeaderOffset + MAVPACKET::SRCBYTE);
//            currentPacketLen = (unsigned char) inbox->at(JaguarHeaderOffset + MAVPACKET::LENBYTE);  // Get payload length
//            currentPacketLen += MAVPACKET::MVHDR_OFFSET + MAVPACKET::tail_CS_OFFSET;    // Account for header and tail
//        }
//        else return;    // Not enough data in yet...wait until we get at least up to the SRC byte
//    }

//    ProcessCurrentPacket();

//    return;
//}

//void JPTest::ProcessCurrentPacket()
//{
//    if (inbox->count() < (currentPacketLen + JaguarHeaderOffset)) return;

//    QList<int> conflictList;
//    QByteArray receivedPacket = inbox->mid(JaguarHeaderOffset, currentPacketLen);

//    if (currentPacketSrc == testOptions->P2ID && P2nextPacket < P2packetInbox->count())
//    {
//        //conflictList = DiffByteArrays(GetJPktPayload(P2packetInbox->at(P2nextPacket++)), receivedPacket);
//        emit P2PacketReceived(receivedPacket, conflictList);
//    }
//    else if ((currentPacketSrc == testOptions->P3ID) && P3nextPacket < P3packetInbox->count())
//    {
//            //conflictList = DiffByteArrays(GetJPktPayload(P3packetInbox->at(P3nextPacket++)), receivedPacket);
//            emit P3PacketReceived(receivedPacket, conflictList);
//    }
//    else
//    {
//        // Packet SRC is wrong!! Handle garbage here (send bytes up to LEN byte marked as garbage)
//        emit GarbagePacketReceived(inbox->mid(JaguarHeaderOffset, MAVPACKET::LENBYTE));

//        FindGoodPacketStart();

//        currentPacketLen = -1;
//        currentPacketSrc = -1;

//        return;
//    }

//    inbox->remove(0, JaguarHeaderOffset + receivedPacket.length());
//    currentPacketLen = -1;
//    currentPacketSrc = -1;

//    return;
//}

//void JPTest::CheckMail()
//{
//    if (MailReady) ProcessInbox();

//    MailReady = false;

//    return;
//}

//void JPTest::FindGoodPacketStart()
//{
//    int goodPacketIndex = -1;

//    for (int i = 0; i < (inbox->count() - 2); i++)
//    {
//        int jagStx = (unsigned char) inbox->at(i);
//        if (jagStx == MAVPACKET::STXVALUE)
//        {
//            int mavStx = (unsigned char) inbox->at(i+2);
//            if (mavStx == MAVPACKET::STXVALUE)
//            {
//                goodPacketIndex = i;
//                break;
//            }
//        }
//    }

//    if (goodPacketIndex != -1)
//        inbox->remove(0, (inbox->length() - ++goodPacketIndex));
//    else
//        inbox->clear();
//    return;
//}

//void JPTest::GetMailFromPort()
//{
//    qDebug() << "In " << __FUNCTION__;

//    QByteArray mail = port->ReadData();
//    inbox->append(mail);

//    MailReady = true;

//    for (int i = 0; i < mail.count(); i++)
//        emit ByteReceived(mail.at(i));

//    return;
//}
