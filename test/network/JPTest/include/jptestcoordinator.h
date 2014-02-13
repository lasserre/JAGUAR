#ifndef JPTESTCOORDINATOR_H
#define JPTESTCOORDINATOR_H

#include <QStringList>
#include "jpacket.h"
#include "jptestoptions.h"
#include "jptestfilereader.h"
#include "jpoutbox.h"
#include "jpinbox.h"
#include "jptestport.h"

class JPTestCoordinator_UT;

class JPTestCoordinator : public QObject
{
    Q_OBJECT

    friend class JPTestCoordinator_UT;

public:
    explicit JPTestCoordinator(QObject* parent = 0);
    ~JPTestCoordinator();
    QList<QStringList> LoadTest(const JPTestOptions& Options);
    bool WaitForDataReceived(const int &msecs);
    QByteArray SendNextPacket();
    JPacketDiffResults CheckMail();
    bool MoreToSend(bool &newPacketStart, int &packetLength);
    bool MoreToReceive() const;

public slots:
    void GetMailFromPort();

signals:
    void ByteReceived(char);

protected:
    QStringList* myOutbox;
    QStringList* P2Inbox;
    QStringList* P3Inbox;
    QMap<QString, JPacket>* jpacketLib;
    JPTestOptions* testOptions;
    JPOutbox* outbox;
    JPInbox* inbox;
    JPTestPort* port;

    // Protected methods
    void FilterInboxList(QStringList& inbox);
    QList<QStringList> LoadTestScript(QFile& JPTestFile);
    bool SetUpPort();
    JPacket GetJPkt(const QString &PacketFilename);
    JPacket GetNextOutgoingPacket();
    JPacket GetNextP2IncomingPacket();
    JPacket GetNextP3IncomingPacket();
    void UpdateInbox();
};

#endif // JPTESTCOORDINATOR_H
