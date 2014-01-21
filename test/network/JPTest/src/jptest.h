#ifndef JPTEST_H
#define JPTEST_H

#include <QObject>
#include <QMap>
#include <QDebug>
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
    int NumOutgoingMsgs;
    int NumMsgsSent;
    // int NumIncomingMsgs;
    int NumMsgsRcvd;
};

struct JPTestOptions
{
    JPTESTMODE RunMode;
    QString Filename;
    int NumLoops;
    int DelaySecs;

    JPTestOptions() : RunMode(RUN)
      , Filename("UnsetFilename")
      , NumLoops(-1)
      , DelaySecs(-1)
    {
    }
};

class JPTest : public QObject
{
    Q_OBJECT

public:
    explicit JPTest(QObject *parent = 0);
    ~JPTest();
    JPTestResults Run(const JPTestOptions& TestOptions);

signals:

public slots:

protected:
    JPTestPort* port;
    QMap<QString, JPacket>* jpacketLib;
    QList<QString>* jptestScript;
    QString jptestFilename;
    int delaySecs;

    bool LoadTestScript();
    void ParseJPTestFile(QFile &JPTestFile);
    QByteArray GetJPktPayload(const QString& PacketFilename);
};

#endif // JPTEST_H
