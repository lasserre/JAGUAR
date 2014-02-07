#include "tst_jptestcoordinator_ut.h"

JPTestCoordinator_UT::JPTestCoordinator_UT()
{
}

// ----------- GetNextOutgoingPacket tests -------------

void JPTestCoordinator_UT::GetNextOutgoingPacket_EmptyOutgoingPacketList_EmptyJPacketPayload()
{
    JPTestCoordinator jptcoord;
    JPacket jp = jptcoord.GetNextOutgoingPacket();

    QVERIFY2(jp.GetPayload().size() == 0, "Packet payload is not empty!");

    return;
}

void JPTestCoordinator_UT::GetNextOutgoingPacket_OnePacketInList_ReturnsPacketThenReturnsEmptyJPacketPayload()
{
    JPTestOptions options;
    JPTestCoordinator jptcoord;

    // Set test filename and load test
    options.Filename = "../../../jptestfiles/CorrectJptestFile.jptest";
    options.JPacketPath = "../../../jptestfiles/jpackets/";
    jptcoord.LoadTest(options);

    JPacket jp = jptcoord.GetNextOutgoingPacket();

    QVERIFY2(jp.GetPayload().size() > 0, "No payload in first packet!");

    jp = jptcoord.GetNextOutgoingPacket();

    QVERIFY2(jp.GetPayload().size() == 0, "Packet payload is not empty!");

    return;
}

// ----------- GetNextP2IncomingPacket tests -----------

void JPTestCoordinator_UT::GetNextP2IncomingPacket_EmptyP2PacketList_EmptyJPacketPayload()
{
    JPTestCoordinator jptcoord;
    JPacket jp = jptcoord.GetNextP2IncomingPacket();

    QVERIFY2(jp.GetPayload().size() == 0, "Packet payload is not empty!");

    return;
}

void JPTestCoordinator_UT::GetNextP2IncomingPacket_OneP2PacketInList_ReturnsPacketThenReturnsEmptyJPacketPayload()
{
    JPTestOptions options;
    JPTestCoordinator jptcoord;

    // Set test filename and load test
    options.Filename = "../../../jptestfiles/CorrectJptestFile.jptest";
    options.JPacketPath = "../../../jptestfiles/jpackets/";
    jptcoord.LoadTest(options);

    JPacket jp = jptcoord.GetNextP2IncomingPacket();

    QVERIFY2(jp.GetPayload().size() > 0, "No payload in first packet!");

    jp = jptcoord.GetNextP2IncomingPacket();

    QVERIFY2(jp.GetPayload().size() == 0, "Packet payload is not empty!");

    return;
}

// ----------- GetNextP3IncomingPacket tests -----------

void JPTestCoordinator_UT::GetNextP3IncomingPacket_EmptyP3PacketList_EmptyJPacketPayload()
{
    JPTestCoordinator jptcoord;
    JPacket jp = jptcoord.GetNextP3IncomingPacket();

    QVERIFY2(jp.GetPayload().size() == 0, "Packet payload is not empty!");

    return;
}

void JPTestCoordinator_UT::GetNextP3IncomingPacket_OneP2PacketInList_ReturnsPacketThenReturnsEmptyJPacketPayload()
{
    JPTestOptions options;
    JPTestCoordinator jptcoord;

    // Set test filename and load test
    options.Filename = "../../../jptestfiles/CorrectJptestFile.jptest";
    options.JPacketPath = "../../../jptestfiles/jpackets/";
    jptcoord.LoadTest(options);

    JPacket jp = jptcoord.GetNextP3IncomingPacket();

    QVERIFY2(jp.GetPayload().size() > 0, "No payload in first packet!");

    jp = jptcoord.GetNextP3IncomingPacket();

    QVERIFY2(jp.GetPayload().size() == 0, "Packet payload is not empty!");

    return;
}
