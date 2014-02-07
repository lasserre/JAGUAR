#ifndef JPTESTCOORDINATOR_H
#define JPTESTCOORDINATOR_H

#include <QStringList>
#include "jpacket.h"
#include "jptestoptions.h"
#include "jptestfilereader.h"

class JPTestCoordinator : public QObject
{
    Q_OBJECT

public:
    explicit JPTestCoordinator(QObject* parent = 0);
    ~JPTestCoordinator();
    bool LoadTest(const JPTestOptions& Options);
    JPacket GetNextOutgoingPacket();
    JPacket GetNextP2IncomingPacket();
    JPacket GetNextP3IncomingPacket();

signals:
    void OutboxLoaded(QStringList);
    void P2InboxLoaded(QStringList);
    void P3InboxLoaded(QStringList);

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
