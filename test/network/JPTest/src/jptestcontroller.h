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

signals:
    void StartTest(const JPTestOptions& TestOptions);
    JPTestResults StopTest();

public slots:

protected:
    JPTest* currentTest;
    QThread* testRunThread;
};

#endif // JPTESTCONTROLLER_H
