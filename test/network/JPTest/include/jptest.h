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
#include "jpinbox.h"
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

protected:
    // ----- Protected members ----- //
    JPTestPort* port;
    JPTestOptions* testOptions;
    JPTestCoordinator* testCoordinator;
    JPOutbox* outbox;
    JPInbox* inbox;

    int JaguarHeaderOffset;

    int delaySecs;
    bool isRunning;
    mutable QMutex isRunningMutex;   // Used to protect acces

    // ----- Protected methods ----- //
    JPTESTERROR InitNewRun();
    bool WaitForServerStart();

    // Running...
    void StartRunLoop();
    void HandleTestMode();

    void SetIsRunning(const bool& IsRunning = true);
    bool Running();

    bool SetUpPort();
    QString ReportErrorCode(const JPTESTERROR& error);
};

#endif // JPTEST_H
