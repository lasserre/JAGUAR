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
#include "jptestcoordinator.h"


class JPTest : public QObject
{
    Q_OBJECT

public:
    explicit JPTest(QObject *parent = 0);
    ~JPTest();

signals:
    void TestEnded();
    void OutboxLoaded(QStringList);
    void P2InboxLoaded(QStringList);
    void P3InboxLoaded(QStringList);
    void PacketSent(QByteArray);
    //void ByteReceived(char);
    void SendDiffResults(JPacketDiffResults);

public slots:
    void LoadTest(JPTestOptions Options);
    void RunServer();
    void RunClient();
    void EndTestEarly();
    //void PassBytesReceived(char);

protected:
    // ----- Protected members ----- //
    JPTestOptions* testOptions;
    JPTestCoordinator* testCoordinator;

    int JaguarHeaderOffset;
    int delaySecs;
    bool testLoaded;
    bool isRunning;
    mutable QMutex isRunningMutex;   // Used to protect acces

    // ----- Protected methods ----- //
    bool InitNewRun();
    void FinishRun();
    bool WaitForServerStart();

    // Running...
    void StartRunLoop();
    void HandleTestMode();

    void SetIsRunning(const bool& IsRunning = true);
    bool Running();

    //QString ReportErrorCode(const JPTESTERROR& error);
};

#endif // JPTEST_H
