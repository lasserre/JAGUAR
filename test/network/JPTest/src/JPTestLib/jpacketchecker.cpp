#include "jpacketchecker.h"

JPacketChecker::JPacketChecker() : nextP2Packet(JPacket())
  , nextP3Packet(JPacket())
  , byteStagingQueue(QByteArray())
  , CurrentState(EmptyQueue)
  , msp430mode(false)
  , p2id(-1)
  , p3id(-1)
  , needP2Packet(true)
  , needP3Packet(true)
  , P2Finished(false)
  , P3Finished(false)
  , EntirePacketReceived(false)
  , stxIndex(0)
  , detectedSrcID(-1)
{
}

void JPacketChecker::Reset()
{
    this->byteStagingQueue.clear();
    this->CurrentState = EmptyQueue;
    this->detectedSrcID = -1;
    this->EntirePacketReceived = false;
    this->needP2Packet = true;
    this->needP3Packet = true;
    this->nextP2Packet.SetPayload(QByteArray());
    this->nextP3Packet.SetPayload(QByteArray());
    this->P2Finished = false;
    this->P3Finished = false;
    this->stxIndex = 0;

    return;
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
    if (!MSP430Mode())
    {
        this->nextP2Packet = NextP2Packet;
    }
    else
    {
        JPacket packet;
        QByteArray fullPayload = NextP2Packet.GetPayload();
        QByteArray expectedPayload = fullPayload.right(fullPayload.length() - MAVPACKET::JHDR_OFFSET);
        packet.SetPayload(expectedPayload);
        this->nextP2Packet = packet;
    }

    needP2Packet = false;

    return;
}

void JPacketChecker::SetNextExpectedP3Packet(const JPacket &NextP3Packet)
{
    if (!MSP430Mode())
    {
        this->nextP3Packet = NextP3Packet;
    }
    else
    {
        JPacket packet;
        QByteArray fullPayload = NextP3Packet.GetPayload();
        QByteArray expectedPayload = fullPayload.right(fullPayload.length() - MAVPACKET::JHDR_OFFSET);
        packet.SetPayload(expectedPayload);
        this->nextP3Packet = packet;
    }

    needP3Packet = false;

    return;
}

JPacketDiffResults JPacketChecker::ReevaluateState(const int& NumBytesAdded)
{
    JPacketDiffResults diffResult;

    // State machine

    switch(CurrentState)
    {
    case EmptyQueue:

        if (byteStagingQueue.isEmpty())
            break;
        else
        {
            // Bytes in queue! Update state and fall through to WaitingForSrc
            CurrentState = WaitingForSrc;
        }

    case WaitingForSrc:

        if (!SRCByteReceived())
        {
            detectedSrcID = JAGID::Unknown;       // SRC not received - still WaitingForSrc
            break;
        }
        else
        {
            /* ------- SRC byte received! ------- */

            if (IsGarbageSourceID(detectedSrcID = GetPacketSource()))
            {
                /* ------- SRC is garbage ------- */

                diffResult.garbageDetected = true;      // Nothing we can do until next packet
                detectedSrcID = JAGID::Garbage;
                CurrentState = LookingForPacketStart;
                break;
            }
            else
            {
                /* ------- SRC is valid. ------- */

                diffResult.packetDetected = true;           // Update state and fall through to WaitingForPacketFinish
                CurrentState = WaitingForPacketFinish;
            }
        }

    case WaitingForPacketFinish:

        detectedSrcID = GetPacketSource();
        qDebug() << "p2ID: " << GetP2ID();
        qDebug() << "p3ID: " << GetP3ID();
        qDebug() << "detectedID: " << detectedSrcID;

        if (detectedSrcID == GetP2ID() && !needP2Packet)
        {
            if (byteStagingQueue.length() >= nextP2Packet.GetPayload().length())
            {
                EntirePacketReceived = true;
                CurrentState = WaitingForSrc;
            }
        }
        else if (detectedSrcID == GetP3ID() && !needP3Packet)
        {
            if (byteStagingQueue.length() >= nextP3Packet.GetPayload().length())
            {
                EntirePacketReceived = true;
                CurrentState = WaitingForSrc;
            }
        }

        break;

    case LookingForPacketStart:

        stxIndex = STXByteReceived();

        if (stxIndex > -1)
        {
            // Packet start found!
            CurrentState = WaitingForSrc;
            detectedSrcID = JAGID::Unknown;
        }
        else
        {
            detectedSrcID = JAGID::Garbage;
        }

        break;

    default:
        qDebug() << "Unknown state! Check out " << __FUNCTION__ << " to debug";
    }

    // Diff the bytes just received
    DiffPacketBytes(diffResult, NumBytesAdded);

    CleanupStagingQueue(diffResult.garbageDetected);      // Remove bytes as appropriate

    stxIndex = -1;     // Reset stxIndex to -1 (not found)

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

int JPacketChecker::STXByteReceived() const
{
    for (int i = 0; i < byteStagingQueue.length(); i++)
    {
        if (JPacket::ByteToIntValue(byteStagingQueue.at(i)) == MAVPACKET::STXVALUE)
            return i;
    }

    return -1;
}

int JPacketChecker::GetPacketSource() const
{
    qDebug() << "msp430 mode: " << MSP430Mode();
    qDebug() << "src index: " << GetSRCByteIndex();
    qDebug() << "byteStagingQueue len: " << byteStagingQueue.length();
    qDebug() << "byteStagingQueue: ";
    for (int i = 0; i < byteStagingQueue.length(); i++)
        qDebug() << JPacket::ByteToIntValue(byteStagingQueue[i]);
    qDebug() << "src byte: " << JPacket::ByteToIntValue(byteStagingQueue.at(GetSRCByteIndex()));
    return JPacket::ByteToIntValue(byteStagingQueue.at(GetSRCByteIndex()));
}

int JPacketChecker::GetPacketLength(const bool& includeMAVHeader /* = false */,
                                    const bool& includeJAGHeader /* = false */) const
{
    int headerLen = 0;

    if (includeMAVHeader)
        headerLen = 8;
    if (includeJAGHeader)
        headerLen = 10;

    return (headerLen + JPacket::ByteToIntValue(byteStagingQueue.at(GetLENByteIndex())));
}

int JPacketChecker::GetP2ID() const
{
    return p2id;
}

int JPacketChecker::GetP3ID() const
{
    return p3id;
}

bool JPacketChecker::IsGarbageSourceID(const int &JagID) const
{
    if (JagID != JAGID::GCS)
    {
        if (JagID != JAGID::MS)
        {
            if (JagID != JAGID::QC)
            {
                if (JagID != JAGID::Broadcast)
                    return true;
            }
        }
    }

    return false;
}

void JPacketChecker::DiffPacketBytes(JPacketDiffResults &DiffResults, const int &NumBytesAdded)
{
    JPTDiff jptdiff;

    if (DiffResults.garbageDetected)
    {    
        for (int i = 0; i < byteStagingQueue.length(); i++)
        {
            if (i <= GetSRCByteIndex())
            {
                // On garbageDetected event, throw away everything up to (and including) SRC
                jptdiff.srcID = JAGID::Garbage;
            }
            else
            {
                // Mark as unknown
                jptdiff.srcID = JAGID::Unknown;
            }

            jptdiff.byte = byteStagingQueue.at(i);

            DiffResults.diffs.append(jptdiff);
        }
    }
    else if (detectedSrcID == JAGID::Garbage)
    {
        // Haven't found start of next packet yet. Throw everything away...
        for (int i = 0; i < byteStagingQueue.length(); i++)
        {
            jptdiff.srcID = JAGID::Garbage;
            jptdiff.byte = byteStagingQueue.at(i);
            DiffResults.diffs.append(jptdiff);
        }
    }
    else if (detectedSrcID == JAGID::Unknown)
    {
        int startIndex = byteStagingQueue.length() - NumBytesAdded;
        for (int i = startIndex; i < byteStagingQueue.length(); i++)
        {
            jptdiff.srcID = JAGID::Unknown;
            jptdiff.byte = byteStagingQueue.at(i);
            DiffResults.diffs.append(jptdiff);
        }
    }
    else if (detectedSrcID == GetP2ID())
    {
        if (DiffResults.packetDetected)
        {
            // On packetDetected event we need to check all rc'vd bytes
            DiffEntireQueueAgainstNextPacket(DiffResults, nextP2Packet, P2Finished, needP2Packet);
        }
        else
        {
            DiffAddedBytesAgainstNextPacket(DiffResults, nextP2Packet, P2Finished, needP2Packet, NumBytesAdded);
        }
    }
    else if (detectedSrcID == GetP3ID())
    {
        if (DiffResults.packetDetected)
        {
            // On packetDetected event we need to check all rc'vd bytes
            DiffEntireQueueAgainstNextPacket(DiffResults, nextP3Packet, P3Finished, needP3Packet);
        }
        else
        {
            DiffAddedBytesAgainstNextPacket(DiffResults, nextP3Packet, P3Finished, needP3Packet, NumBytesAdded);
        }
    }
    else
    {
        // Broadcast?
    }
    return;
}

bool JPacketChecker::ReadyForNextP2Packet() const
{
    return needP2Packet;
}

bool JPacketChecker::ReadyForNextP3Packet() const
{
    return needP3Packet;
}

void JPacketChecker::SetP2Done()
{
    P2Finished = true;
    return;
}

void JPacketChecker::SetP3Done()
{
    P3Finished = true;
    return;
}

void JPacketChecker::CleanupStagingQueue(const bool &GarbageJustDetected)
{
    int removeLen = 0;

    if (EntirePacketReceived)
    {
        removeLen = GetPacketLength(true, !MSP430Mode());  // Remove packet once it is fully received
        EntirePacketReceived = false;
    }
    else if (CurrentState == LookingForPacketStart)
    {
        // Still haven't found new stx byte...remove garbage bytes from queue
        if (GarbageJustDetected)
        {
            // Remove everything up to (and including) SRC byte
            removeLen = GetSRCByteIndex() + 1;
        }
        else
        {
            // Remove all bytes
            removeLen = byteStagingQueue.length();
        }
    }
    else if (stxIndex > -1)
    {
        // New packet start just detected. Remove garbage bytes just before it
        removeLen = stxIndex;
    }

    byteStagingQueue.remove(0, removeLen);

    return;
}

void JPacketChecker::DiffEntireQueueAgainstNextPacket(JPacketDiffResults &DiffResults, const JPacket& Packet,
                        const bool& Finished, const bool& NeedPacket)
{
    JPTDiff jptdiff;

    if (Finished)
        HandleDiffWhenFinished(DiffResults, Packet.GetSrc());
    else if (NeedPacket)
    {
        qDebug() << "ERROR: " << Packet.GetSrc() << " detected but need new packet to diff with!";
    }
    else
    {
        // We have valid packet to diff with
        int diffLength;

        if (EntirePacketReceived)
        {
            diffLength = Packet.GetPayload().length();    // If we have the whole thing, diff up to packet length

            if (Packet.GetSrc() == GetP2ID())
                needP2Packet = true;                                // (we'll need the next packet after this diff)
            else
                needP3Packet = true;
        }
        else
            diffLength = byteStagingQueue.length();     // ...otherwise, just diff up to what we have

        for (int i = 0; i < diffLength; i++)
        {
            jptdiff.srcID = Packet.GetSrc();
            jptdiff.byte = byteStagingQueue.at(i);

            // Do the actual diff! lol

            if (jptdiff.byte != Packet.GetPayload().at(i))
                jptdiff.pass = false;
            else
                jptdiff.pass = true;

            DiffResults.diffs.append(jptdiff);
        }
    }
    return;
}

void JPacketChecker::DiffAddedBytesAgainstNextPacket(JPacketDiffResults& DiffResults, const JPacket& Packet,
                                             const bool& Finished, const bool& NeedPacket, const int& NumBytesAdded)
{
    JPTDiff jptdiff;

    if (Finished)
        HandleDiffWhenFinished(DiffResults, Packet.GetSrc());
    else if (NeedPacket)
        qDebug() << "ERROR: " << Packet.GetSrc() << " detected but need new packet to diff with!";
    else
    {
        int startIndex = byteStagingQueue.length() - NumBytesAdded;
        for (int i = startIndex; i < byteStagingQueue.length(); i++)
        {
            jptdiff.srcID = Packet.GetSrc();
            jptdiff.byte = byteStagingQueue.at(i);

            // Do the actual diff! lol

            if (jptdiff.byte != Packet.GetPayload().at(i))
                jptdiff.pass = false;
            else
                jptdiff.pass = true;

            DiffResults.diffs.append(jptdiff);
        }
    }

    return;
}

bool JPacketChecker::LENByteMatchesExpectedLength(const int &jagID)
{
    if (jagID == GetP2ID())
    {
        if (GetPacketLength(true, true) == nextP2Packet.GetPayload().length())
            return true;
        else
            return false;
    }
    else if (jagID == GetP3ID())
    {
        if (GetPacketLength(true, true) == nextP3Packet.GetPayload().length())
            return true;
        else
            return false;
    }
    else
        return false;
}

void JPacketChecker::HandleDiffWhenFinished(JPacketDiffResults &DiffResults, const int &srcID)
{
    JPTDiff jptdiff;

    qDebug() << "Warning: " << srcID << " detected, but no more packets expected";

    // We should not be getting anything from src at this point, so mark all as garbage
    for (int i = 0; i < byteStagingQueue.length(); i++)
    {
        jptdiff.srcID = JAGID::Garbage;
        jptdiff.byte = byteStagingQueue.at(i);
        DiffResults.diffs.append(jptdiff);
    }

    return;
}
