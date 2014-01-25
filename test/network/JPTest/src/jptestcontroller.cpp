#include "jptestcontroller.h"

JPTestController::JPTestController(QObject *parent) :
    QObject(parent)
  , currentTest(new JPTest())   // CLS - don't set a parent, since we are moving to another thread!
  , testThreadController(new QThread(this))
  , testIsRunning(false)
{
    this->currentTest->moveToThread(testThreadController); // Put currentTest in its own thread

    connect(this->testThreadController, SIGNAL(finished()), this->currentTest, SLOT(deleteLater()));
    connect(this, SIGNAL(StartTestSignal(JPTestOptions)), this->currentTest, SLOT(Run(JPTestOptions)));
    connect(this,SIGNAL(StopTestSignal()), this->currentTest, SLOT(EndTestEarly()), Qt::DirectConnection);
    connect(this->currentTest, SIGNAL(TestEnded()), this, SLOT(TestEndedHandler()));

    this->testThreadController->start();   // Start thread. Don't call quit() until destructor!
}

JPTestController::~JPTestController()
{
    this->testThreadController->quit();

    if (!this->testThreadController->wait(2000))    // wait 2000 ms, then kill
    {
        this->testThreadController->terminate();
        this->testThreadController->wait();
    }

    delete this->testThreadController;  // testThreadController lives in the main thread (so should be deleted)
}

void JPTestController::StartTest(const JPTestOptions &TestOptions)
{
    if (!testIsRunning)
    {
        emit StartTestSignal(TestOptions);
        testIsRunning = true;
    }
    else qDebug() << "Test already in progress!";
    return;
}

void JPTestController::StopTest()
{
    if (testIsRunning)
    {
        emit StopTestSignal();
    }
    else qDebug() << "No test to stop!";
    return;
}

void JPTestController::TestEndedHandler()
{
    testIsRunning = false;
    return;
}
