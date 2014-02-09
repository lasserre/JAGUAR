#include "jpacketchecker.h"

JPacketChecker::JPacketChecker() : nextP2Packet(JPacket())
  , nextP3Packet(JPacket())
  , byteStagingQueue(QByteArray())
  , CurrentState(Ready)
  , msp430mode(false)
  , p2id(-1)
  , p3id(-1)
{
}

JPacketDiffResults JPacketChecker::CheckNewBytes(const QByteArray &Bytes)
{
    byteStagingQueue.append(Bytes);

    return ReevaluateState(Bytes.length());
}

void JPacketChecker::SetMSP430Mode(const bool &MSP430ModeOn /* = true */)
{
    this->msp430mode = MSP430ModeOn;
    return;
}

bool JPacketChecker::MSP430Mode() const
{
    return this->msp430mode;
}

void JPacketChecker::SetP2ID(const int &P2ID)
{
    this->p2id = P2ID;
    return;
}

void JPacketChecker::SetP3ID(const int &P3ID)
{
    this->p3id = P3ID;
    return;
}

void JPacketChecker::SetNextExpectedP2Packet(const JPacket &NextP2Packet)
{
    this->nextP2Packet = NextP2Packet;
    return;
}

void JPacketChecker::SetNextExpectedP3Packet(const JPacket &NextP3Packet)
{
    this->nextP3Packet = NextP3Packet;
    return;
}

JPacketDiffResults JPacketChecker::ReevaluateState(const int& NumBytesAdded)
{
    JPacketDiffResults diffResult;

    switch(CurrentState)
    {
    case Ready:
        if (SRCByteReceived())
        {
            diffResult.jaguarID = GetPacketSource();

            if (IsValidSourceID(diffResult.jaguarID))
            {
                if (LENByteReceived())
                {
                }
            }
            else
            {
                // Source is garbage! Nothing we can do until next packet

                diffResult.jaguarID = JAGID::Garbage;
                diffResult.packetDetected = true;

                CurrentState = LookingForPacketStart;
            }
        }
        else
        {
            // SRC byte not in yet
            CurrentState = WaitingForSrc;
        }
        break;
    case WaitingForSrc:
        break;
    case WaitingForPacketFinish:
        break;
    case LookingForPacketStart:
        break;
    default:
        qDebug() << "Unknown state! Check out " << __FUNCTION__ << " to debug";
    }

    DiffPacketBytes(diffResult);

    return diffResult;
}

int JPacketChecker::GetSRCByteIndex() const
{
    if (MSP430Mode())
        return (MAVPACKET::SRCBYTE);
    else
        return (MAVPACKET::JHDR_OFFSET + MAVPACKET::SRCBYTE);
}

int JPacketChecker::GetLENByteIndex() const
{
    if (MSP430Mode())
        return (MAVPACKET::LENBYTE);
    else
        return (MAVPACKET::JHDR_OFFSET + MAVPACKET::LENBYTE);
}

bool JPacketChecker::SRCByteReceived() const
{
    if (byteStagingQueue.length() > GetSRCByteIndex())
        return true;
    else
        return false;
}

bool JPacketChecker::LENByteReceived() const
{
    if (byteStagingQueue.length() > GetLENByteIndex())
        return true;
    else
        return false;
}

int JPacketChecker::GetPacketSource() const
{
    return (unsigned int)byteStagingQueue.at(GetSRCByteIndex());
}

int JPacketChecker::GetPacketLength() const
{
    return (unsigned int)byteStagingQueue.at(GetLENByteIndex());
}

int JPacketChecker::GetP2ID() const
{
    return p2id;
}

int JPacketChecker::GetP3ID() const
{
    return p3id;
}

bool JPacketChecker::IsValidSourceID(const int &JagID) const
{
    if (JagID != JAGID::GCS)
    {
        if (JagID != JAGID::MS)
        {
            if (JagID != JAGID::QC)
            {
                if (JagID != JAGID::Broadcast)
                    return false;
            }
        }
    }

    return true;
}

void JPacketChecker::DiffPacketBytes(JPacketDiffResults &DiffResults)
{
    if (DiffResults.jaguarID == JAGID::Garbage)
    {
        for (int i = 0; i < byteStagingQueue.length(); i++)
        {
            DiffResults.diffs.insert(byteStagingQueue.at(i), false);
        }
    }
    else if (DiffResults.jaguarID == GetP2ID())
    {

    }
    else if (DiffResults.jaguarID == GetP3ID())
    {

    }
    else
    {
        // Broadcast?
    }
    return;
}

//QList<int> JPTest::DiffByteArrays(const QByteArray &first, const QByteArray &second)
//{
//    QList<int> conflictIndices;
//    int maxDiffLength = first.count();

//    if (second.count() != first.count())
//    {
//        qDebug() << "Number of bytes in packet diff do NOT match!";
//        if (second.count() < first.count()) maxDiffLength = second.count();
//    }

//    for (int i = 0; i < maxDiffLength; i++)
//    {
//        if (first.at(i) != second.at(i))
//            conflictIndices.append(i);
//    }

//    return conflictIndices;
//}
