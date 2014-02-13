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
    void TestStarted();
    void TestEnded();
    void FinishedSending();
    void UnableToStartTest();
    void OutboxLoaded(QStringList);
    void P2InboxLoaded(QStringList);
    void P3InboxLoaded(QStringList);
    void DataSent(QByteArray, bool newPacketStart, int packetLength);
    //void ByteReceived(char);
    void SendDiffResults(JPacketDiffResults);

public slots:
    void LoadTest(JPTestOptions Options);
    void RunServer();
    void RunClient();
    void EndTestEarly();
    void HandleStepSignal();

protected:
    // ----- Protected members ----- //
    JPTestOptions* testOptions;
    JPTestCoordinator* testCoordinator;

    int JaguarHeaderOffset;
    int delaySecs;
    bool testLoaded;
    bool isRunning;
    bool stepReceived;
    mutable QMutex isRunningMutex;   // Used to protect acces
    mutable QMutex stepReceivedMutex;

    // ----- Protected methods ----- //
    bool InitNewRun();
    void FinishRun();
    bool WaitForServerStart();
    void WaitForStep();

    // Running...
    void StartRunLoop();
    void HandleTestMode();
    void CheckForMail();

    void SetIsRunning(const bool& IsRunning = true);
    bool Running();
    void SetStepReceived();
    void ClearStepReceived();
    bool StepReceived();

    //QString ReportErrorCode(const JPTESTERROR& error);
};

#endif // JPTEST_H
