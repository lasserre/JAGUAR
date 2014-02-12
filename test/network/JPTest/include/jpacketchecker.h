#ifndef JPACKETCHECKER_H
#define JPACKETCHECKER_H

#include "jpacket.h"

namespace JAGID
{
    const int MS = JAGPACKET::MS;
    const int QC = JAGPACKET::QC;
    const int GCS = JAGPACKET::GCS;
    const int Broadcast = JAGPACKET::BROADCAST;
    const int Unknown = 111;        // Arbitrary values that won't interfere with real data...
    const int Garbage = 112;
}

struct JPTDiff
{
    int srcID;
    bool pass;
    char byte;

    JPTDiff() : srcID(JAGID::Unknown)
      , pass(false)
      , byte(0x00)
    {
    }
};

struct JPacketDiffResults
{
    bool packetDetected;
    bool garbageDetected;
    QList<JPTDiff> diffs;         // Map of bytes to results

    JPacketDiffResults() : packetDetected(false)
      , garbageDetected(false)
      , diffs(QList<JPTDiff>())
    {
    }
};

enum JPCheckerState
{
    EmptyQueue,                      // Starting state - nothing in staging queue
    WaitingForSrc,              // Bytes in, but not SRC byte yet
    WaitingForPacketFinish,     // Packet detected, but not fully received
    LookingForPacketStart       // Garbage packet detected! Trying to find start of next good data
};

class JPacketChecker
{
public:
    JPacketChecker();

    /* ----- Run functions ----- */
    void SetNextExpectedP2Packet(const JPacket& NextP2Packet);
    void SetNextExpectedP3Packet(const JPacket& NextP3Packet);
    JPacketDiffResults CheckNewBytes(const QByteArray& Bytes);
    void SetP2Done();
    void SetP3Done();

    // Polling
    bool ReadyForNextP2Packet() const;
    bool ReadyForNextP3Packet() const;

    /* ----- Setup functions ----- */
    void SetMSP430Mode(const bool& MSP430ModeOn = true);
    bool MSP430Mode() const;   
    void SetP2ID(const int& P2ID);
    void SetP3ID(const int& P3ID);

protected:
    /* ----- Protected variables ----- */
    JPacket nextP2Packet;
    JPacket nextP3Packet;
    QByteArray byteStagingQueue;
    JPCheckerState CurrentState;
    bool msp430mode;
    int p2id;
    int p3id;
    bool needP2Packet;
    bool needP3Packet;
    bool P2Finished;
    bool P3Finished;
    bool EntirePacketReceived;
    int stxIndex;
    int detectedSrcID;

    /* ----- Protected methods ----- */
    JPacketDiffResults ReevaluateState(const int &NumBytesAdded);
    void DiffPacketBytes(JPacketDiffResults& DiffResults, const int& NumBytesAdded);
    void CleanupStagingQueue(const bool& GarbageJustDetected);
    void DiffEntireQueueAgainstNextPacket(JPacketDiffResults& DiffResults, const JPacket& Packet,
                                    const bool& Finished, const bool& NeedPacket);
    void DiffAddedBytesAgainstNextPacket(JPacketDiffResults& DiffResults, const JPacket& Packet,
                                    const bool& Finished, const bool& NeedPacket, const int& NumBytesAdded);
    void HandleDiffWhenFinished(JPacketDiffResults& DiffResults, const int& srcID);

    // Checks
    bool SRCByteReceived() const;
    bool LENByteReceived() const;
    int STXByteReceived() const;
    bool LENByteMatchesExpectedLength(const int& jagID);

    // Getters
    int GetSRCByteIndex() const;
    int GetLENByteIndex() const;
    int GetPacketSource() const;
    int GetPacketLength() const;
    int GetP2ID() const;
    int GetP3ID() const;

    bool IsGarbageSourceID(const int& JagID) const;
};

#endif // JPACKETCHECKER_H
