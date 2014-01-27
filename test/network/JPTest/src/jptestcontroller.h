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
    void StartTest(const JPTestOptions &TestOptions);
    void StopTest();
    void LoadTest(const JPTestOptions &Options);

signals:
    void StartServerTestSignal(const JPTestOptions& TestOptions);
    void StartClientTestSignal(const JPTestOptions& TestOptions);
    void StopTestSignal();
    void LoadTestSignal(const JPTestOptions& TestOptions);
    void OutboxChanged(QList<QByteArray>);
    void PacketSent(QByteArray);

public slots:
    void TestEndedHandler();
    void NewOutboxHandler(QList<QByteArray> newOutbox);
    void PacketSentHandler(QByteArray);

protected:
    JPTest* currentTest;
    QThread* testThreadController;
    bool testIsRunning;
};

#endif // JPTESTCONTROLLER_H
