#include "jptestcontroller.h"

JPTestController::JPTestController(QObject *parent) :
    QObject(parent)
  , currentTest(new JPTest())   // CLS - don't set a parent, since we are moving to another thread!
  , testRunThread(new QThread(this))
{
    this->currentTest->moveToThread(testRunThread); // Put currentTest in its own thread

    connect(this->testRunThread, SIGNAL(finished()), this->currentTest, SLOT(deleteLater()));
    connect(this, SIGNAL(StartTest(JPTestOptions)), this->currentTest, SLOT(Run(JPTestOptions)));
    connect(this,SIGNAL(StopTest()), this->currentTest, SLOT(EndTestEarly()), Qt::DirectConnection);

    this->testRunThread->start();   // Start thread. Don't call quit() until destructor!
}

JPTestController::~JPTestController()
{
    this->testRunThread->quit();
    if (!this->testRunThread->wait(2000))    // wait 2000 ms, then kill
    {
        this->testRunThread->terminate();
        this->testRunThread->wait();
    }
    delete this->currentTest;
    // Let qt take care of testRunThread (connected to deleteLater())
}
