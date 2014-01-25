#include "jptest.h"

JPTest::JPTest(QObject *parent) :
    QObject(parent)
  , port(new JPTestPort(this))
  , testOptions(new JPTestOptions())
  , jpacketLib(new QMap<QString, JPacket>())
  , jpacketPath(new QString())
  , jptestScript(new QList<QString>())
  , jptestFilename(QString("NoFilenameSpecified"))
  , delaySecs(-1)
  , isRunning(false)
  , RemainingLoops(0)
{
}

JPTest::~JPTest()
{
    qDebug() << __FUNCTION__ << " called.";
    delete this->port;
    delete this->testOptions;
    delete this->jpacketLib;
    delete this->jpacketPath;
    delete this->jptestScript;
}

/**
 * @brief JPTest::Run actually runs the test in [another] QThread
 * @param TestOptions
 */
void JPTest::Run(JPTestOptions TestOptions)
{
    qDebug() << "In " << __FUNCTION__;

    // Initialize test, and check for errors
    JPTESTERROR initResult = InitNewRun(TestOptions);
    if (initResult != NO_ERROR)
    {
        qDebug() << ReportErrorCode(initResult);
        SetIsRunning(false);
    }

    // Set up jptestScript iterator, and begin loop
    QList<QString>::iterator currentTestPacket = jptestScript->begin();

     // ---------- Run loop ---------- //
    while((currentTestPacket != jptestScript->end()) && Running())
    {
        // Handle test mode functionality
        HandleTestMode();

        if (port->SendData(GetJPktPayload(*currentTestPacket)))
            ;   //CLS - TODO: emit progress for progress bar!!!
                //CLS - TODO: refactor to HandlePacket() function (decide to send, delay, or move on)

        currentTestPacket++;

        if (currentTestPacket == jptestScript->end() && (testOptions->NumLoops > -1))
        {
            // Completed one more run.
            currentTestPacket = jptestScript->begin();  // Reset to beginning of test, in case we need to run again
            if (!RunTestAgain()) break;
        }
    }

    SetIsRunning(false);
    qDebug() << __FUNCTION__ << ":" << __LINE__;
    jpacketLib->clear();
qDebug() << __FUNCTION__ << ":" << __LINE__;
    emit TestEnded();

    return;
}

// Construct test script from .jptest file
bool JPTest::LoadTestScript()
{
    bool success = false;
    QFile jptestfile(this->jptestFilename);

    if (jptestfile.open(QIODevice::ReadOnly))
    {
        ParseJPTestFile(jptestfile);
        success = true;
    }
    else qDebug() << "Unable to open .jptest file " + jptestFilename;

    return success;
}

/**
 * @brief JPTest::ParseJPTestFile is the .jptest file interpreter. Change this to change .jptest file format
 * @param JPTestFile
 */
void JPTest::ParseJPTestFile(QFile &JPTestFile)
{
    // Remove spaces in .csv file
    QList<QByteArray> removeSpaces = JPTestFile.readAll().split(' ');
    QByteArray noSpaces;
    for (int i = 0; i < removeSpaces.count(); i++)
        noSpaces.append(removeSpaces[i]);

    // Create list from comma-separated-values
    QList<QByteArray> jptestlist = noSpaces.split(',');
    QList<QByteArray>::iterator testIter = jptestlist.begin();
    while (testIter != jptestlist.end())
    {
        this->jptestScript->append(QString(*testIter));
        testIter++;
    }
    return;
}

// If packet exists in RAM, returns payload. Otherwise, load into library and return payload
QByteArray JPTest::GetJPktPayload(const QString &PacketFilename)
{
    if (this->jpacketLib->contains(PacketFilename))
    {
        return jpacketLib->value(PacketFilename).GetPayload();
    }
    else
    {
        // Load from file
        JPacket packet(testOptions->JPacketPath + PacketFilename);
        jpacketLib->insert(PacketFilename, packet);
        return packet.GetPayload();
    }
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

JPTESTERROR JPTest::InitNewRun(const JPTestOptions &TestOptions)
{
    SetIsRunning();

    *this->testOptions = TestOptions;

    // Assign NumLoops since this won't get checked until after first loop through
    this->RemainingLoops = testOptions->NumLoops;

    if (!SetUpPort()) return PORT_ERROR;    // change to const int PORT_ISSUE;

    // Set filename and load .jptest file
    this->jptestFilename = testOptions->Filename;
    if (!LoadTestScript()) return JPTESTFILE_ERROR;      // End test if unable to load script

    return NO_ERROR;   // CLS - TODO: take out magic #'s!!
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

/**
 * @brief JPTest::RunTestAgain
 * @return Returns true if we should run the test again, false otherwise.
 */
bool JPTest::RunTestAgain()
{
    bool loopAgain = false;

    if (testOptions->NumLoops == 0) return true;    // NumLoops of 0 means loop forever (until manually stopped)
    else RemainingLoops--;      // Account for the loop just completed
    if (RemainingLoops == 0) return false;

    return loopAgain;
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
