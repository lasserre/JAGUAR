#include "tst_jpacketchecker_ut.h"

JPacketChecker_UT::JPacketChecker_UT()
{
}

void JPacketChecker_UT::CheckMail_TestDiffs()
{
    JPacketChecker packetInspector;
    QString A2Gfilename = "../../../jptestfiles/jpackets/simpleA2G";
    JPacket a2gpacket(A2Gfilename);

    packetInspector.SetNextExpectedP2Packet(a2gpacket);

    int NumBytes = 3;
    QByteArray incomingData = a2gpacket.GetPayload().left(NumBytes);

    JPacketDiffResults results = packetInspector.CheckNewBytes(incomingData);

    QCOMPARE(results.diffs.count(), NumBytes);
    QCOMPARE(results.diffs[0].srcID, JAGID::Unknown);

    for (int i = 0; i < NumBytes; i++)
        QCOMPARE(results.diffs[i].byte, incomingData.at(i));

    return;
}
