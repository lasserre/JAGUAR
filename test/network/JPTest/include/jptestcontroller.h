#ifndef JPTESTCONTROLLER_H
#define JPTESTCONTROLLER_H

#include <QObject>
#include <QThread>
#include "jptest.h"

class JPTestController : public QObject
{
    Q_OBJECT
public:
    explicit JPTestController(QObject *parent = 0);
    ~JPTestController();
    void StartTest(const bool &RunServer);
    void StopTest();
    void LoadTest(const JPTestOptions &Options);

signals:
    void StartServerTestSignal();
    void StartClientTestSignal();
    void StopTestSignal();
    void LoadTestSignal(const JPTestOptions& TestOptions);
    void OutboxChanged(QStringList);
    void P2InboxChanged(QStringList);
    void P3InboxChanged(QStringList);
    void PacketSent(QByteArray);
    void RawByteReceived(char);
    void P2PacketReceived(QByteArray, QList<int>);
    void P3PacketReceived(QByteArray, QList<int>);
    void GarbagePacketReceived(QByteArray);
    //void NewDiffResults(JPacketDiffResults);

public slots:
    void TestEndedHandler();
    void NewOutboxHandler(QStringList newOutbox);
    void NewP2InboxHandler(QStringList newP2Inbox);
    void NewP3InboxHandler(QStringList newP3Inbox);
    void PacketSentHandler(QByteArray);
    void ByteReceivedHandler(char);
    void P2PacketReceivedHandler(QByteArray packet, QList<int> diffs);
    void P3PacketReceivedHandler(QByteArray, QList<int>);
    void GarbagePacketReceivedHandler(QByteArray);
    void HandleDiffResults(JPacketDiffResults);

protected:
    JPTest* currentTest;
    QThread* testThreadController;
    bool testIsRunning;
};

#endif // JPTESTCONTROLLER_H
