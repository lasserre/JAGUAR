#include "jptestcontroller.h"

JPTestController::JPTestController(QObject *parent) :
    QObject(parent)
  , currentTest(new JPTest())   // CLS - don't set a parent, since we are moving to another thread!
  , testThreadController(new QThread(this))
  , testIsRunning(false)
{
    this->currentTest->moveToThread(testThreadController); // Put currentTest in its own thread

    connect(this->testThreadController, SIGNAL(finished()), this->currentTest, SLOT(deleteLater()));
    connect(this, SIGNAL(StartServerTestSignal(JPTestOptions)), this->currentTest, SLOT(RunServer(JPTestOptions)));
    connect(this, SIGNAL(StartClientTestSignal(JPTestOptions)), this->currentTest, SLOT(RunClient(JPTestOptions)));
    connect(this,SIGNAL(StopTestSignal()), this->currentTest, SLOT(EndTestEarly()), Qt::DirectConnection);
    connect(this->currentTest, SIGNAL(TestEnded()), this, SLOT(TestEndedHandler()));
    connect(this->currentTest, SIGNAL(OutboxLoaded(QList<QByteArray>)), this, SLOT(NewOutboxHandler(QList<QByteArray>)));
    connect(this, SIGNAL(LoadTestSignal(JPTestOptions)), this->currentTest, SLOT(LoadTest(JPTestOptions)));
    connect(this->currentTest, SIGNAL(PacketSent(QByteArray)), this, SLOT(PacketSentHandler(QByteArray)));

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
        if (TestOptions.RunServer)
            emit StartServerTestSignal(TestOptions);
        else
            emit StartClientTestSignal(TestOptions);
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

void JPTestController::NewOutboxHandler(QList<QByteArray> newOutbox)
{
    emit OutboxChanged(newOutbox);
    return;
}

void JPTestController::LoadTest(const JPTestOptions& Options)
{
    emit LoadTestSignal(Options);
    return;
}

void JPTestController::PacketSentHandler(QByteArray packet)
{
    qDebug() << "In " << __FUNCTION__;
    emit PacketSent(packet);
    return;
}
