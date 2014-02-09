#ifndef JPACKETCHECKER_H
#define JPACKETCHECKER_H

#include "jpacket.h"

namespace JAGID
{
    const int MS = JAGPACKET::MS;
    const int QC = JAGPACKET::QC;
    const int GCS = JAGPACKET::GCS;
    const int Broadcast = JAGPACKET::BROADCAST;
    const int Garbage = 111;        // Arbitrary values that won't interfere with real data...
    const int Unknown = 112;
}

struct JPacketDiffResults
{
    bool packetDetected;
    int jaguarID;                   // Need to use const variables above (in JAGID namespace)
    QMap<char, bool> diffs;         // Map of bytes to results

    JPacketDiffResults() : packetDetected(false)
      , jaguarID(JAGID::Unknown)
      , diffs(QMap<char, bool>())
    {
    }
};

enum JPCheckerState
{
    Ready,                      // Starting state - nothing in staging queue
    WaitingForSrc,              // Bytes in, but not SRC byte yet
    WaitingForPacketFinish,     // Packet detected, but not fully received
    LookingForPacketStart       // Garbage packet detected! Trying to find start of next good data
};

class JPacketChecker
{
public:
    JPacketChecker();
    void SetNextExpectedP2Packet(const JPacket& NextP2Packet);
    void SetNextExpectedP3Packet(const JPacket& NextP3Packet);
    JPacketDiffResults CheckNewBytes(const QByteArray& Bytes);
    void SetMSP430Mode(const bool& MSP430ModeOn = true);
    bool MSP430Mode() const;
    void SetP2ID(const int& P2ID);
    void SetP3ID(const int& P3ID);

protected:
    // Protected variables
    JPacket nextP2Packet;
    JPacket nextP3Packet;
    QByteArray byteStagingQueue;
    JPCheckerState CurrentState;
    bool msp430mode;
    int p2id;
    int p3id;

    // Protected methods
    JPacketDiffResults ReevaluateState(const int &NumBytesAdded);
    int GetSRCByteIndex() const;
    int GetLENByteIndex() const;
    bool SRCByteReceived() const;
    bool LENByteReceived() const;
    int GetPacketSource() const;
    int GetPacketLength() const;
    int GetP2ID() const;
    int GetP3ID() const;
    bool IsValidSourceID(const int& JagID) const;
    void DiffPacketBytes(JPacketDiffResults& DiffResults);
};

#endif // JPACKETCHECKER_H
