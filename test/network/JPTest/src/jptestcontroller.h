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

signals:
    void StartTestSignal(const JPTestOptions& TestOptions);
    void StopTestSignal();

public slots:
    void TestEndedHandler();

protected:
    JPTest* currentTest;
    QThread* testThreadController;
    bool testIsRunning;
};

#endif // JPTESTCONTROLLER_H
