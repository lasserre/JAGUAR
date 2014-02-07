#ifndef JPTESTCOORDINATOR_H
#define JPTESTCOORDINATOR_H

#include <QStringList>
#include "jpacket.h"
#include "jptestoptions.h"
#include "jptestfilereader.h"

class JPTestCoordinator
{
public:
    JPTestCoordinator();
    ~JPTestCoordinator();
    bool LoadTest(const JPTestOptions& Options);
    JPacket GetNextOutgoingPacket();
    JPacket GetNextP2IncomingPacket();
    JPacket GetNextP3IncomingPacket();

protected:
    QStringList* myOutbox;
    QStringList* P2Inbox;
    QStringList* P3Inbox;
    QMap<QString, JPacket>* jpacketLib;
    JPTestOptions* testOptions;

    // Protected methods
    void LoadTestScript(const JPTestOptions &Options, QFile& JPTestFile);
    JPacket GetJPkt(const QString &PacketFilename);
};

#endif // JPTESTCOORDINATOR_H
