#ifndef JPTEST_H
#define JPTEST_H

#include <QObject>
#include <QMap>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include "jptestport.h"
#include "jpacket.h"

/**
 * @brief The JPTESTMODE enum specifies the test mode for a given JPTest.
 * @enum RUN mode is normal run mode. Packets are sent out sequentially as normal.
 * @enum STEP mode allows the user to step through test, packet by packet.
 * @enum DELAY mode inserts a specified delay in between the sending of each packet
 */
enum JPTESTMODE
{
    RUN,
    STEP,
    DELAY
};

enum JPTESTERROR
{
    NO_ERROR,
    PORT_ERROR,
    JPTESTFILE_ERROR
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
      , PortName("UnsetPortName")
      , NumLoops(-1)
      , DelaySecs(-1)
    {
    }

    JPTestOptions(const JPTestOptions& other) : RunMode(other.RunMode)
      , Filename(other.Filename)
      , JPacketPath(other.JPacketPath)
      , PortName(other.PortName)
      , NumLoops(other.NumLoops)
      , DelaySecs(other.DelaySecs)
    {
    }

    ~JPTestOptions()
    {
    }

    JPTESTMODE RunMode;
    QString Filename;
    QString JPacketPath;
    QString PortName;
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
    void TestEnded();

public slots:
    void Run(JPTestOptions TestOptions);
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
    JPTESTERROR InitNewRun(const JPTestOptions &TestOptions);
    bool LoadTestScript();
    void ParseJPTestFile(QFile &JPTestFile);
    QByteArray GetJPktPayload(const QString& PacketFilename);
    void SetIsRunning(const bool& IsRunning = true);
    bool Running();
    bool SetUpPort();
    void HandleTestMode();
    bool RunTestAgain();    // "Looping" test mode handler
    QString ReportErrorCode(const JPTESTERROR& error);
};

#endif // JPTEST_H
