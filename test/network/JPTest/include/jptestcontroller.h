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
    // Signals to JPTest on worker thread
    void StartServerTestSignal();
    void StartClientTestSignal();
    void StopTestSignal();
    void LoadTestSignal(const JPTestOptions& TestOptions);

    // Signals to parent...
    void OutboxChanged(QStringList);
    void P2InboxChanged(QStringList);
    void P3InboxChanged(QStringList);
    void PacketSent(QByteArray);
    void RawByteReceived(char);
    void NewDiffResults(JPacketDiffResults);
    void TestEnded();

public slots:
    void TestEndedHandler();
    void NewOutboxHandler(QStringList newOutbox);
    void NewP2InboxHandler(QStringList newP2Inbox);
    void NewP3InboxHandler(QStringList newP3Inbox);
    void PacketSentHandler(QByteArray);
    void ByteReceivedHandler(char);
    void HandleDiffResults(JPacketDiffResults);

protected:
    JPTest* currentTest;
    QThread* testThreadController;
    bool testIsRunning;
};

#endif // JPTESTCONTROLLER_H
