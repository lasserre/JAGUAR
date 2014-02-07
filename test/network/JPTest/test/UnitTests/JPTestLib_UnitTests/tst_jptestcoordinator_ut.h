#ifndef TST_JPTESTCOORDINATOR_UT_H
#define TST_JPTESTCOORDINATOR_UT_H

#include <QTest>
#include "jptestcoordinator.h"
#include "jpacket.h"

class JPTestCoordinator_UT : public QObject
{
    Q_OBJECT

public:
    JPTestCoordinator_UT();

private Q_SLOTS:
    // GetNextOutgoingPacket tests
    void GetNextOutgoingPacket_EmptyOutgoingPacketList_EmptyJPacketPayload();
    void GetNextOutgoingPacket_OnePacketInList_ReturnsPacketThenReturnsEmptyJPacketPayload();

    // GetNextP2IncomingPacket tests
    void GetNextP2IncomingPacket_EmptyP2PacketList_EmptyJPacketPayload();
    void GetNextP2IncomingPacket_OneP2PacketInList_ReturnsPacketThenReturnsEmptyJPacketPayload();

    // GetNextP3IncomingPacket tests
    void GetNextP3IncomingPacket_EmptyP3PacketList_EmptyJPacketPayload();
    void GetNextP3IncomingPacket_OneP2PacketInList_ReturnsPacketThenReturnsEmptyJPacketPayload();
};

#endif // TST_JPTESTCOORDINATOR_UT_H
