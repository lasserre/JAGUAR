#include "jptest.h"

JPTest::JPTest(QObject *parent) :
    QObject(parent)
  , port(new JPTestPort(this))
  , testOptions(new JPTestOptions())
  , testCoordinator(new JPTestCoordinator())
  , outbox(new JPOutbox())
  , inbox(new JPInbox())
  , JaguarHeaderOffset(0)
  , delaySecs(-1)
  , isRunning(false)
{
    connect(this->port, SIGNAL(youveGotMail()), this, SLOT(GetMailFromPort()), Qt::DirectConnection);

}

JPTest::~JPTest()
{
    delete this->port;
    delete this->testOptions;
    delete this->testCoordinator;
    delete this->outbox;
    delete this->inbox;
}

JPTESTERROR JPTest::InitNewRun()
{
    SetIsRunning();

    if (!SetUpPort()) return PORT_ERROR;    // change to const int PORT_ISSUE;

    if (testOptions->MSP430ModeOn) JaguarHeaderOffset = 0;
    else JaguarHeaderOffset = MAVPACKET::JHDR_OFFSET;

    return NO_ERROR;   // CLS - TODO: take out magic #'s!!
}

/**
 * @brief JPTest::RunServer actually runs the test in [another] QThread
 * @param TestOptions
 */
void JPTest::RunServer()
{
    qDebug() << "In " << __FUNCTION__;

    // Initialize test, and check for errors
    JPTESTERROR initResult = InitNewRun();
    if (initResult != NO_ERROR)
    {
        qDebug() << ReportErrorCode(initResult);
        SetIsRunning(false);
    }

    StartRunLoop();

    // Cleanup/reset
    SetIsRunning(false);
    emit TestEnded();
    return;
}

void JPTest::RunClient()
{
    JPTESTERROR initResult = InitNewRun();

    if (initResult != NO_ERROR)
    {
        qDebug() << ReportErrorCode(initResult);
        SetIsRunning(false);
        emit TestEnded();
        return;
    }

    // Listen for server...
    if (!WaitForServerStart())
    {
        qDebug() << "Client run cancelled by user.";
        SetIsRunning(false);
        emit TestEnded();
        return;
    }

    StartRunLoop();

    SetIsRunning(false);
    emit TestEnded();
    return;
}

void JPTest::StartRunLoop()
{
    bool IsLoopingRun = (testOptions->NumLoops > -1);

    // Send/receive
    while(Running() && outbox->MoreToSend())
    {
        // Do delays/step/etc...
        HandleTestMode();

        // Send packets
        outbox->SendNextPacket();

        // Check the mail
        QCoreApplication::processEvents();
        inbox->CheckMail();
    }

    // Finish receiving
    while (Running() && inbox->MoreToReceive())
    {
        QCoreApplication::processEvents();
        inbox->CheckMail();
    }

    return;
}

/**
 * @brief JPTest::LoadTest assigns the testOptions from Options, and loads the .jptest file.
 * @param Options
 */
void JPTest::LoadTest(JPTestOptions Options)
{
    // Copy test options
    *this->testOptions = Options;

    // Load test
    testCoordinator->LoadTest(*testOptions);

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

bool JPTest::SetUpPort()
{
    // Set port name
    port->SetPortName(this->testOptions->PortName);

    // Open port
    return port->OpenPort();
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

QString JPTest::ReportErrorCode(const JPTESTERROR &error)
{
    QString errMsg = "JPTest error: ";

    switch(error)
    {
    case NO_ERROR:
        errMsg.append("NO_ERROR");
        break;
    case PORT_ERROR:
        errMsg.append("PORT_ERROR");
        break;
    case JPTESTFILE_ERROR:
        errMsg.append("JPTESTFILE_ERROR");
        break;
    default:
        errMsg.append("UNKNOWN_ERROR_TYPE");
    }

    return errMsg;
}

/**
 * @brief JPTest::WaitForServerStart waits for data to come in from the server.
 * @return true if data received, false if run stopped by user
 */
bool JPTest::WaitForServerStart()
{
    while (this->Running())
        if (port->WaitForData(1000)) return true;
    return false;
}
