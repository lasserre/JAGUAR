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
    void OutboxChanged(QList<QByteArray>);
    void P2InboxChanged(QList<QByteArray>);
    void P3InboxChanged(QList<QByteArray>);
    void PacketSent(QByteArray);
    void RawByteReceived(char);

public slots:
    void TestEndedHandler();
    void NewOutboxHandler(QList<QByteArray> newOutbox);
    void NewP2InboxHandler(QList<QByteArray> newP2Inbox);
    void NewP3InboxHandler(QList<QByteArray> newP3Inbox);
    void PacketSentHandler(QByteArray);
    void ByteReceivedHandler(char);

protected:
    JPTest* currentTest;
    QThread* testThreadController;
    bool testIsRunning;
};

#endif // JPTESTCONTROLLER_H
