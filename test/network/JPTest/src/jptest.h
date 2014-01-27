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

struct JPTestOptions
{
    JPTestOptions() : RunMode(RUN)
      , Filename("UnsetFilename")
      , JPacketPath("UnsetJPacketPath")
      , PortName("UnsetPortName")
      , NumLoops(-1)
      , DelaySecs(-1)
      , JaguarID(JAGPACKET::GCS)
      , RunServer(true)
    {
    }

    JPTestOptions(const JPTestOptions& other) : RunMode(other.RunMode)
      , Filename(other.Filename)
      , JPacketPath(other.JPacketPath)
      , PortName(other.PortName)
      , NumLoops(other.NumLoops)
      , DelaySecs(other.DelaySecs)
      , JaguarID(other.JaguarID)
      , RunServer(other.RunServer)
    {
    }

    ~JPTestOptions()
    {
    }

    QString GetJagIDString()
    {
        switch(JaguarID)
        {
        case JAGPACKET::GCS:
            return "GCS";
        case JAGPACKET::MS:
            return "MS";
        case JAGPACKET::QC:
            return "QC";
        case JAGPACKET::BROADCAST:
            return "Broadcast";
        default:
            return "JPTestOptions: unrecognized JAGUAR ID!";
        }
    }

    JPTESTMODE RunMode;
    QString Filename;
    QString JPacketPath;
    QString PortName;
    int NumLoops;   // NumLoops -1 => don't loop, 0 => loop forever, n>0 => loop n times
    int DelaySecs;
    int JaguarID;
    bool RunServer;
};

class JPTest : public QObject
{
    Q_OBJECT

public:
    explicit JPTest(QObject *parent = 0);
    ~JPTest();

signals:
    void TestEnded();
    void OutboxLoaded(QList<QByteArray>);
    void PacketSent(QByteArray);
    void P2InboxLoaded(QList<QByteArray>);
    void P3InboxLoaded(QList<QByteArray>);
    void P2PacketReceived(QByteArray);
    void P3PacketReceived(QByteArray);

public slots:
    void LoadTest(JPTestOptions Options);
    void RunServer(JPTestOptions TestOptions);
    void RunClient(JPTestOptions testOptions);
    void EndTestEarly();
    void GetMailFromPort();

protected:
    // Protected members
    JPTestPort* port;
    JPTestOptions* testOptions;
    QMap<QString, JPacket>* jpacketLib;
    QString* jpacketPath;
    QList<QString>* packetOutbox;
    QByteArray* inbox;
    int delaySecs;
    bool isRunning;
    bool testLoaded;
    mutable QMutex isRunningMutex;   // Used to protect acces
    int RemainingLoops;
    bool MailReady;
    int UnreadBytes;

    // Protected methods
    JPTESTERROR InitNewServerRun(const JPTestOptions &TestOptions);
    JPTESTERROR InitNewClientRun(const JPTestOptions &TestOptions);
    bool WaitForServerStart();
    bool LoadTestScript();
    void ParseJPTestFile(QFile &JPTestFile);
    QList<QByteArray> GetServerPacketList(QFile &JPTestFile);
    QByteArray RemoveAllOccurrences(QByteArray stream, const char &deleteChar);
    QByteArray GetJPktPayload(const QString& PacketFilename);

    void HandleTestMode();
    bool RunTestAgain();    // "Looping" test mode handler

    void SetIsRunning(const bool& IsRunning = true);
    bool Running();

    bool SetUpPort();
    QString ReportErrorCode(const JPTESTERROR& error);
};

#endif // JPTEST_H
