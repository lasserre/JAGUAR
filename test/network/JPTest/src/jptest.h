#ifndef JPTEST_H
#define JPTEST_H

#include <QObject>
#include <QMap>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include "jptestport.h"
#include "jpacket.h"

enum JPTESTMODE
{
    RUN,
    STEP,
    DELAY
};

struct JPTestResults
{
    JPTestResults() : NumOutgoingMsgs(0)
      , NumMsgsSent(0)
      , NumMsgsRcvd(0)
      , TestCompleted(false)
    {
    }

    int NumOutgoingMsgs;
    int NumMsgsSent;
    // int NumIncomingMsgs;
    int NumMsgsRcvd;
    bool TestCompleted;
};

struct JPTestOptions
{
    JPTestOptions() : RunMode(RUN)
      , Filename("UnsetFilename")
      , JPacketPath("UnsetJPacketPath")
      , NumLoops(-1)
      , DelaySecs(-1)
    {
    }

    JPTestOptions(const JPTestOptions& other) : RunMode(other.RunMode)
      , Filename(other.Filename)
      , JPacketPath(other.JPacketPath)
      , NumLoops(other.NumLoops)
      , DelaySecs(other.DelaySecs)
    {
    }

    JPTESTMODE RunMode;
    QString Filename;
    QString JPacketPath;
    int NumLoops;   // NumLoops -1 => don't loop, 0 => loop forever, n>0 => loop n times
    int DelaySecs;
};

class JPTest : public QObject
{
    Q_OBJECT

public:
    explicit JPTest(QObject *parent = 0);
    ~JPTest();

signals:

public slots:
    void Run(const JPTestOptions& TestOptions);
    void EndTestEarly();

protected:
    // Protected members
    JPTestPort* port;
    JPTestOptions* testOptions;
    QMap<QString, JPacket>* jpacketLib;
    QString* jpacketPath;
    QList<QString>* jptestScript;
    QString jptestFilename;
    int delaySecs;
    bool isRunning;
    mutable QMutex isRunningMutex;   // Used to protect acces
    int RemainingLoops;

    // Protected methods
    int InitNewRun(const JPTestOptions &TestOptions);
    bool LoadTestScript();
    void ParseJPTestFile(QFile &JPTestFile);
    QByteArray GetJPktPayload(const QString& PacketFilename);
    void SetIsRunning(const bool& IsRunning = true);
    bool Running();
    bool SetUpPort();
    void HandleTestMode();
    bool RunTestAgain();    // "Looping" test mode handler
};

#endif // JPTEST_H
