#include "jptest.h"

JPTest::JPTest(QObject *parent) :
    QObject(parent)
  , testOptions(new JPTestOptions())
  , testCoordinator(NULL)
  , JaguarHeaderOffset(0)
  , delaySecs(-1)
  , testLoaded(false)
  , isRunning(false)
  , stepReceived(false)
{
    //connect(this->testCoordinator, SIGNAL(ByteReceived(char)), this, SLOT(PassBytesReceived(char)));
}

JPTest::~JPTest()
{
    delete this->testOptions;
}

bool JPTest::InitNewRun()
{
    if (!testLoaded)
        return false;

    SetIsRunning();
    return true;
}

void JPTest::FinishRun()
{
    testLoaded = false;
    SetIsRunning(false);

    emit TestEnded();

    return;
}

/**
 * @brief JPTest::RunServer actually runs the test in [another] QThread
 * @param TestOptions
 */
void JPTest::RunServer()
{
    qDebug() << "In " << __FUNCTION__;

    // Initialize test, and check for errors
    if (!InitNewRun())
    {
        qDebug() << "Test not loaded!";

        SetIsRunning(false);
        emit UnableToStartTest();
        return;
    }

    emit TestStarted();

    StartRunLoop();

    // Cleanup/reset
    FinishRun();
    return;
}

void JPTest::RunClient()
{
    if (!InitNewRun())
    {
        qDebug() << "Test not loaded!";

        SetIsRunning(false);
        emit UnableToStartTest();
        return;
    }

    emit TestStarted();

    // Listen for server...
    if (!WaitForServerStart())
    {
        qDebug() << "Client run cancelled by user.";

        FinishRun();
        return;
    }

    StartRunLoop();

    FinishRun();
    return;
}

void JPTest::StartRunLoop()
{
    //bool IsLoopingRun = (testOptions->NumLoops > -1);
    bool newPacketStart = false;
    int packetLength = 0;

    // Send/receive
    while(Running() && testCoordinator->MoreToSend(newPacketStart, packetLength))
    {
        // Do delays/step/etc...
        HandleTestMode();

        if (!Running()) break;  // We might have killed the test while waiting for step signal

        // Send packets
        emit DataSent(testCoordinator->SendNextPacket(), newPacketStart, packetLength);

        CheckForMail();
    }

    // Let main window know we're done sending (for step mode)
    if (Running())
        emit FinishedSending();

    // Finish receiving
    while (Running() && testCoordinator->MoreToReceive())
    {
        CheckForMail();
    }

    return;
}

/**
 * @brief JPTest::LoadTest assigns the testOptions from Options, and loads the .jptest file.
 * @param Options
 */
void JPTest::LoadTest(JPTestOptions Options)
{
    // Initialize AFTER being moved to thread to avoid QObject error
    if (testCoordinator == NULL)
        testCoordinator = new JPTestCoordinator(this);

    testLoaded = false;

    // Copy test options
    *this->testOptions = Options;

    // Load test
    QList<QStringList> LoadedMailboxes = testCoordinator->LoadTest(*testOptions);

    if (!LoadedMailboxes.isEmpty())
    {
        emit OutboxLoaded(LoadedMailboxes.at(0));
        emit P2InboxLoaded(LoadedMailboxes.at(1));
        emit P3InboxLoaded(LoadedMailboxes.at(2));

        testLoaded = true;
    }

    return;
}

// If this is called, the user chose to stop the test before completion
// NOTE: Connect this to signal with Qt::DirectConnection to make sure our return value
// gets saved to variable in other thread before we try to use it!
void JPTest::EndTestEarly()
{
    qDebug() << "In " << __FUNCTION__;

    SetIsRunning(false);
    return;
}

void JPTest::SetIsRunning(const bool &IsRunning /*= true*/)
{
    QMutexLocker(&this->isRunningMutex);  // Locks mutex at construction, and unlocks at destruction (end of function).
    this->isRunning = IsRunning;
    return;
}

bool JPTest::Running()
{
    QMutexLocker(&this->isRunningMutex);
    return this->isRunning;
}

void JPTest::SetStepReceived()
{
    QMutexLocker(&this->stepReceivedMutex);
    this->stepReceived = true;
    return;
}

void JPTest::ClearStepReceived()
{
    QMutexLocker(&this->stepReceivedMutex);
    this->stepReceived = false;
    return;
}

bool JPTest::StepReceived()
{
    QMutexLocker(&this->stepReceivedMutex);
    return this->stepReceived;
}

void JPTest::HandleTestMode()
{
    switch(testOptions->RunMode)
    {
    case RUN:
        break;
    case STEP:
        WaitForStep();
        break;
    case DELAY:
        if (-1 != testOptions->DelaySecs)
        {
            // Delay
            QThread::sleep(testOptions->DelaySecs);
        }
        else qDebug() << "Delay mode specified with invalid (or no) delay!";
        break;
    default:
        // Shouldn't get here!
        qDebug() << "Unrecognized test mode!";
        break;
    }
    return;
}

void JPTest::CheckForMail()
{
    // Get all incoming data and send outgoing data
    QCoreApplication::processEvents();

    // Check the mail
    JPacketDiffResults results = testCoordinator->CheckMail();
    if (!results.diffs.isEmpty())
        emit SendDiffResults(results);

    return;
}

/**
 * @brief JPTest::WaitForServerStart waits for data to come in from the server.
 * @return true if data received, false if run stopped by user
 */

bool JPTest::WaitForServerStart()
{
    while (this->Running())
        if (testCoordinator->WaitForDataReceived(1000)) return true;
    return false;
}

void JPTest::WaitForStep()
{
    while (this->Running())
    {
        QThread::msleep(100);

        if (StepReceived())
        {
            ClearStepReceived();
            break;
        }

        CheckForMail();
    }
    return;
}

void JPTest::HandleStepSignal()
{
    SetStepReceived();
    return;
}
