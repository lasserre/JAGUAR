#include "jptest.h"

JPTest::JPTest(QObject *parent) :
    QObject(parent)
  , testOptions(new JPTestOptions())
  , testCoordinator(new JPTestCoordinator())
  , JaguarHeaderOffset(0)
  , delaySecs(-1)
  , testLoaded(false)
  , isRunning(false)
{
    //connect(this->testCoordinator, SIGNAL(ByteReceived(char)), this, SLOT(PassBytesReceived(char)));
}

JPTest::~JPTest()
{
    delete this->testOptions;
    delete this->testCoordinator;
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

        FinishRun();
        return;
    }

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

        FinishRun();
        return;
    }

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

    // Send/receive
    while(Running() && testCoordinator->MoreToSend())
    {
        // Do delays/step/etc...
        HandleTestMode();

        // Send packets
        testCoordinator->SendNextPacket();

        // Get all incoming data and send outgoing data
        QCoreApplication::processEvents();

        // Check the mail
        emit SendDiffResults(testCoordinator->CheckMail());
    }

    // Finish receiving
    while (Running() && testCoordinator->MoreToReceive())
    {
        QCoreApplication::processEvents();
        emit SendDiffResults(testCoordinator->CheckMail());
    }

    return;
}

/**
 * @brief JPTest::LoadTest assigns the testOptions from Options, and loads the .jptest file.
 * @param Options
 */
void JPTest::LoadTest(JPTestOptions Options)
{
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
    FinishRun();
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

void JPTest::HandleTestMode()
{
    switch(testOptions->RunMode)
    {
    case RUN:
        break;
    case STEP:
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

//QString JPTest::ReportErrorCode(const JPTESTERROR &error)
//{
//    QString errMsg = "JPTest error: ";

//    switch(error)
//    {
//    case NO_ERROR:
//        errMsg.append("NO_ERROR");
//        break;
//    case PORT_ERROR:
//        errMsg.append("PORT_ERROR");
//        break;
//    case JPTESTFILE_ERROR:
//        errMsg.append("JPTESTFILE_ERROR");
//        break;
//    default:
//        errMsg.append("UNKNOWN_ERROR_TYPE");
//    }

//    return errMsg;
//}

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

//void JPTest::PassBytesReceived(char byte)
//{
//    emit ByteReceived(byte);
//    return;
//}
