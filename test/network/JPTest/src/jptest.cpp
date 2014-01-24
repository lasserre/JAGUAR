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
    // CLS - getting JPTest malloc error when uncommented...am i trying to delete 2x?
    // Probably something to do with no parent assignment/moving to QThread...

//    delete this->port;
//    delete this->testOptions;
//    delete this->jpacketLib;
//    delete this->jpacketPath;
//    delete this->jptestScript;
}

/**
 * @brief JPTest::Run actually runs the test in [another] QThread
 * @param TestOptions
 */
void JPTest::Run(const JPTestOptions &TestOptions)
{
    InitNewRun(TestOptions);

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
    else qDebug() << "Unable to open file " + jptestFilename;

    return success;
}

/**
 * @brief JPTest::ParseJPTestFile is the .jptest file interpreter. Change this to change .jptest file format
 * @param JPTestFile
 */
void JPTest::ParseJPTestFile(QFile &JPTestFile)
{
    QList<QByteArray> jptestlist = JPTestFile.readAll().split(',');
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
    bool success = false;

    // Set port name
    // Open port

    return success;
}

int JPTest::InitNewRun(const JPTestOptions &TestOptions)
{
    SetIsRunning();

    *this->testOptions = TestOptions;

    // Assign NumLoops since this won't get checked until after first loop through
    this->RemainingLoops = testOptions->NumLoops;

    if (!SetUpPort()) return 1;    // change to const int PORT_ISSUE;

    // Set filename and load .jptest file
    this->jptestFilename = testOptions->Filename;
    if (!LoadTestScript()) return 2;      // End test if unable to load script

    return 0;   // CLS - TODO: take out magic #'s!!
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
