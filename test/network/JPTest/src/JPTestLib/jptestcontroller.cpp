#include "jptestcontroller.h"

JPTestController::JPTestController(QObject *parent) :
    QObject(parent)
  , currentTest(new JPTest())   // CLS - don't set a parent, since we are moving to another thread!
  , testThreadController(new QThread(this))
  , testIsRunning(false)
{
    this->currentTest->moveToThread(testThreadController); // Put currentTest in its own thread

    connect(this->testThreadController, SIGNAL(finished()), this->currentTest, SLOT(deleteLater()));
    connect(this, SIGNAL(StartServerTestSignal()), this->currentTest, SLOT(RunServer()));
    connect(this, SIGNAL(StartClientTestSignal()), this->currentTest, SLOT(RunClient()));
    connect(this,SIGNAL(StopTestSignal()), this->currentTest, SLOT(EndTestEarly()), Qt::DirectConnection);
    connect(this, SIGNAL(StepTestSignal()), this->currentTest, SLOT(HandleStepSignal()), Qt::DirectConnection);
    connect(this, SIGNAL(LoadTestSignal(JPTestOptions)), this->currentTest, SLOT(LoadTest(JPTestOptions)));

    // Connect JPTest signals to controller
    connect(this->currentTest, SIGNAL(TestStarted()), this, SLOT(TestStartedHandler()));
    connect(this->currentTest, SIGNAL(UnableToStartTest()), this, SLOT(UnableToStartTestHandler()));
    connect(this->currentTest, SIGNAL(FinishedSending()), this, SLOT(FinishedSendingHandler()));
    connect(this->currentTest, SIGNAL(TestEnded()), this, SLOT(TestEndedHandler()));
    connect(this->currentTest, SIGNAL(OutboxLoaded(QStringList)), this, SLOT(NewOutboxHandler(QStringList)));
    connect(this->currentTest, SIGNAL(P2InboxLoaded(QStringList)), this, SLOT(NewP2InboxHandler(QStringList)));
    connect(this->currentTest, SIGNAL(P3InboxLoaded(QStringList)), this, SLOT(NewP3InboxHandler(QStringList)));
    connect(this->currentTest, SIGNAL(DataSent(QByteArray,bool,int)), this, SLOT(DataSentHandler(QByteArray,bool,int)));
    connect(this->currentTest, SIGNAL(SendDiffResults(JPacketDiffResults)),
            this, SLOT(HandleDiffResults(JPacketDiffResults)));

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

void JPTestController::StartTest(const bool& RunServer)
{
    if (!testIsRunning)
    {
        if (RunServer)
            emit StartServerTestSignal();
        else
            emit StartClientTestSignal();
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

void JPTestController::StepTest()
{
    if (testIsRunning)
        emit StepTestSignal();
    else
        qDebug() << "Can't step because test is not running!";
    return;
}

void JPTestController::TestStartedHandler()
{
    emit TestStarted();
    return;
}

void JPTestController::TestEndedHandler()
{
    testIsRunning = false;
    emit TestEnded();
    return;
}

void JPTestController::NewOutboxHandler(QStringList newOutbox)
{
    emit OutboxChanged(newOutbox);
    return;
}

void JPTestController::NewP2InboxHandler(QStringList newP2Inbox)
{
    emit P2InboxChanged(newP2Inbox);
    return;
}

void JPTestController::NewP3InboxHandler(QStringList newP3Inbox)
{
    emit P3InboxChanged(newP3Inbox);
    return;
}

void JPTestController::LoadTest(const JPTestOptions& Options)
{
    emit LoadTestSignal(Options);
    return;
}

void JPTestController::DataSentHandler(QByteArray packet, bool newPacketStart, int packetLength)
{
    // qDebug() << "In " << __FUNCTION__;
    emit DataSent(packet, newPacketStart, packetLength);
    return;
}

void JPTestController::ByteReceivedHandler(char byte)
{
    emit RawByteReceived(byte);
    return;
}

void JPTestController::HandleDiffResults(JPacketDiffResults results)
{
    emit NewDiffResults(results);
    return;
}

void JPTestController::UnableToStartTestHandler()
{
    testIsRunning = false;
    emit UnableToStartTest();
    return;
}

void JPTestController::FinishedSendingHandler()
{
    emit FinishedSending();
    return;
}
