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
      , P2ID(JAGPACKET::MS)
      , P3ID(JAGPACKET::QC)
    {
    }

    JPTestOptions(const JPTestOptions& other) : RunMode(other.RunMode)
      , Filename(other.Filename)
      , JPacketPath(other.JPacketPath)
      , PortName(other.PortName)
      , NumLoops(other.NumLoops)
      , DelaySecs(other.DelaySecs)
      , JaguarID(other.JaguarID)
      , P2ID(other.P2ID)
      , P3ID(other.P3ID)
    {
    }

    ~JPTestOptions()
    {
    }

    /**
     * @brief GetJagIDString
     * @return the string of MY Jaguar ID
     */
    QString GetJagIDString()
    {
        return GetIDString(JaguarID);
    }

    QString GetP2IDString()
    {
        return GetIDString(P2ID);
    }

    QString GetP3IDString()
    {
        return GetIDString(P3ID);
    }

    JPTESTMODE RunMode;
    QString Filename;
    QString JPacketPath;
    QString PortName;
    int NumLoops;   // NumLoops -1 => don't loop, 0 => loop forever, n>0 => loop n times
    int DelaySecs;
    int JaguarID;
    int P2ID;
    int P3ID;

protected:
    QString GetIDString(const int& ID)
    {
        switch(ID)
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
    void ByteReceived(char);
    void P2InboxLoaded(QList<QByteArray>);
    void P3InboxLoaded(QList<QByteArray>);
    void P2PacketReceived(QByteArray);
    void P3PacketReceived(QByteArray);

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
    // Inbox/Outbox members
    QList<QString>* packetOutbox;   // Packet inbox/outboxes
    QList<QString>* P1packetInbox;
    QList<QString>* P2packetInbox;

    QByteArray* inbox;              // Raw bytes inbox
    bool MailReady;
    int UnreadBytes;

    int delaySecs;
    bool isRunning;
    bool testLoaded;
    mutable QMutex isRunningMutex;   // Used to protect acces
    int RemainingLoops;

    // ----- Protected methods ----- //
    JPTESTERROR InitNewRun();
    bool WaitForServerStart();
    bool LoadTestScript();
    void ParseJPTestFile(QFile &JPTestFile);
    QList<QByteArray> GetPacketList(const QString& jagIDString, QFile &JPTestFile);
    QByteArray RemoveAllOccurrences(QByteArray stream, const char &deleteChar);
    QByteArray GetJPktPayload(const QString& PacketFilename);

    void StartRunLoop();
    void HandleTestMode();
    bool RunTestAgain();    // "Looping" test mode handler
    void CheckMail();

    void SetIsRunning(const bool& IsRunning = true);
    bool Running();

    bool SetUpPort();
    QString ReportErrorCode(const JPTESTERROR& error);
};

#endif // JPTEST_H
