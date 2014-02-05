#ifndef JPTEST_H
#define JPTEST_H

#include <QObject>
#include <QMap>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include "jptestport.h"
#include "jpacket.h"
#include "jptestoptions.h"
#include "jpoutbox.h"
#include "jptestcoordinator.h"


class JPTest : public QObject
{
    Q_OBJECT

public:
    explicit JPTest(QObject *parent = 0);
    ~JPTest();

signals:
    void TestEnded();
    void OutboxLoaded(QList<QByteArray>);
    void P2InboxLoaded(QList<QByteArray>);
    void P3InboxLoaded(QList<QByteArray>);
    void PacketSent(QByteArray);
    void ByteReceived(char);
    void P2PacketReceived(QByteArray, QList<int>);
    void P3PacketReceived(QByteArray, QList<int>);
    void GarbagePacketReceived(QByteArray);

public slots:
    void LoadTest(JPTestOptions Options);
    void RunServer();
    void RunClient();
    void EndTestEarly();
    void GetMailFromPort();

protected:
    // ----- Protected members ----- //
    JPTestPort* port;
    JPTestOptions* testOptions;
    QMap<QString, JPacket>* jpacketLib;
    QString* jpacketPath;
    JPTestCoordinator* testCoordinator;
    JPOutbox* outbox;

    // Inbox/Outbox members
    QList<QString>* packetOutbox;   // Packet inbox/outboxes
    QList<QString>* P2packetInbox;
    QList<QString>* P3packetInbox;
    int P2nextPacket;
    int P3nextPacket;

    QByteArray* inbox;              // Raw bytes inbox
    bool MailReady;
    int JaguarHeaderOffset;
    int currentPacketSrc;
    int currentPacketLen;

    int delaySecs;
    bool isRunning;
    mutable QMutex isRunningMutex;   // Used to protect acces
    int RemainingLoops;

    // ----- Protected methods ----- //
    JPTESTERROR InitNewRun();
    bool WaitForServerStart();

    // Loading from files...
    bool LoadTestScript();
    void ParseJPTestFile(QFile &JPTestFile);
    QByteArray GetJPktPayload(const QString& PacketFilename);

    // Running...
    void StartRunLoop();
    void HandleTestMode();
    bool RunTestAgain();    // "Looping" test mode handler

    // Get mail...
    void CheckMail();
    void ProcessInbox();

    // Interpret incoming bytes...
    void ProcessCurrentPacket();
    QList<int> DiffByteArrays(const QByteArray& first, const QByteArray& second);
    void FindGoodPacketStart();

    void SetIsRunning(const bool& IsRunning = true);
    bool Running();

    bool SetUpPort();
    QString ReportErrorCode(const JPTESTERROR& error);
};

#endif // JPTEST_H
