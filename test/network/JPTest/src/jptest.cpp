#include "jptest.h"

JPTest::JPTest(QObject *parent) :
    QObject(parent)
  , port(new JPTestPort(this))
  , testOptions(new JPTestOptions())
  , jpacketLib(new QMap<QString, JPacket>())
  , jpacketPath(new QString())
  , packetOutbox(new QList<QString>())
  , inbox(new QByteArray())
  , delaySecs(-1)
  , isRunning(false)
  , testLoaded(false)
  , RemainingLoops(0)
  , MailReady(false)
  , UnreadBytes(0)
{
    connect(this->port, SIGNAL(youveGotMail()), this, SLOT(GetMailFromPort()));
}

JPTest::~JPTest()
{
    qDebug() << __FUNCTION__ << " called.";
    delete this->port;
    delete this->testOptions;
    delete this->jpacketLib;
    delete this->jpacketPath;
    delete this->packetOutbox;
    delete this->inbox;
}

/**
 * @brief JPTest::RunServer actually runs the test in [another] QThread
 * @param TestOptions
 */
void JPTest::RunServer(JPTestOptions TestOptions)
{
    qDebug() << "In " << __FUNCTION__;
    qDebug() << "TestOptions.port = " << TestOptions.PortName;

    // Initialize test, and check for errors
    JPTESTERROR initResult = InitNewServerRun(TestOptions);
    if (initResult != NO_ERROR)
    {
        qDebug() << ReportErrorCode(initResult);
        SetIsRunning(false);
    }

    // Set up jptestScript iterator, and begin loop
    QList<QString>::iterator currentTestPacket = packetOutbox->begin();

     // ---------- Run loop ---------- //
    while((currentTestPacket != packetOutbox->end()) && Running())
    {
        // Handle test mode functionality
        HandleTestMode();

        qDebug() << "packet: " << *currentTestPacket;
        if (port->SendData(GetJPktPayload(*currentTestPacket)))
        {
            emit PacketSent(GetJPktPayload(*currentTestPacket));
        }
        //CLS - TODO: refactor to HandlePacket() function (decide to send, delay, or move on)

        currentTestPacket++;

        if (currentTestPacket == packetOutbox->end() && (testOptions->NumLoops > -1))
        {
            // Completed one more run.
            currentTestPacket = packetOutbox->begin();  // Reset to beginning of test, in case we need to run again
            if (!RunTestAgain()) break;
        }
    }

    SetIsRunning(false);
    emit TestEnded();
    return;
}

void JPTest::RunClient(JPTestOptions testOptions)
{
    JPTESTERROR initResult = InitNewClientRun(testOptions);

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
        emit TestEnded();
        return;
    }

    while(Running())
    {
        // Do client loop here...assume this is running much faster than we are receiving packets...?
        if (MailReady)
        {
            QByteArray::iterator mailByteIter = inbox->begin();
            QByteArray newPacket;
            for (int i = 0; i < UnreadBytes; i++)
            {
                if (i == MAVPACKET::SRCBYTE)
                {
                    qDebug() << "Source: " << *mailByteIter;
                }
                newPacket.append(*mailByteIter);
            }
            qDebug() << "newPacket: " << newPacket;
        }
    }

    emit TestEnded();
    return;
}

// Construct test script from .jptest file
bool JPTest::LoadTestScript()
{
    bool success = false;
    QFile jptestfile(testOptions->Filename);

    if (jptestfile.open(QIODevice::ReadOnly))
    {
        ParseJPTestFile(jptestfile);
        success = true;
    }
    else qDebug() << "Unable to open .jptest file " + testOptions->Filename;

    return success;
}

/**
 * @brief JPTest::ParseJPTestFile is the .jptest file interpreter. Change this to change .jptest file format
 * @param JPTestFile
 */
void JPTest::ParseJPTestFile(QFile &JPTestFile)
{
    packetOutbox->clear();  // Get rid of existing data

    // Create list from comma-separated-values in our JAGID's section
    QList<QByteArray> jptestlist = GetServerPacketList(JPTestFile);

    // Put list of packets we must send into packetOutbox
    QList<QByteArray>::iterator testIter = jptestlist.begin();
    while (testIter != jptestlist.end())
    {
        this->packetOutbox->append(QString(*testIter));
        testIter++;
    }

    // Send list to GUI
    emit OutboxLoaded(jptestlist);

    return;
}

QList<QByteArray> JPTest::GetServerPacketList(QFile &JPTestFile)
{
    QList<QByteArray> packetList = QList<QByteArray>();
    QByteArray line;
    QString jagID = testOptions->GetJagIDString();
    bool found = false;

    // Find <[JAGID]> line that matches our ID
    while (!JPTestFile.atEnd() && !found)
    {
        line = JPTestFile.readLine();   // Get a line

        if (line[0] != '<') continue;   // We want to find <GCS> or similar...
        else
        {
            bool match = true;

            for (int i = 0; i < jagID.length(); i++)
            {
                if (line.at(i+1) != jagID.at(i))
                {
                    match = false;
                    break;
                }
            }

            if (match) found = true;
        }
    }

    // Get lines below it...
    if (found)
    {
        while(!JPTestFile.atEnd())
        {
            line = JPTestFile.readLine();
            if (line[0] != '<')     // Not next ID's section. Take out spaces, split at commas, and keep!
            {
                QByteArray noNewlinesLine = RemoveAllOccurrences(line, '\n');
                QByteArray noSpacesLine = RemoveAllOccurrences(noNewlinesLine, ' ');
                packetList.append(noSpacesLine.split(','));             // Split at commas, and add to list
            }
            else break;     // If we hit a line starting with '<', we're done.
        }
    }

    return packetList;
}

QByteArray JPTest::RemoveAllOccurrences(QByteArray stream, const char& deleteChar)
{
    QList<QByteArray> characterRemovedList = stream.split(deleteChar);
    QByteArray noDeleteChars;
    for (int i = 0; i < characterRemovedList.count(); i++)
        noDeleteChars.append(characterRemovedList.at(i));
    return noDeleteChars;
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

JPTESTERROR JPTest::InitNewServerRun(const JPTestOptions &TestOptions)
{
    SetIsRunning();

    if (!SetUpPort()) return PORT_ERROR;    // change to const int PORT_ISSUE;

    if (!testLoaded)
    {
        qDebug() << "WARNING: Loading test from JPTest::InitNewServerRun...is this intended?";

        *this->testOptions = TestOptions;
        if (!LoadTestScript()) return JPTESTFILE_ERROR;      // End test if unable to load script
    }

    // Assign NumLoops since this won't get checked until after first loop through
    this->RemainingLoops = testOptions->NumLoops;

    return NO_ERROR;   // CLS - TODO: take out magic #'s!!
}

JPTESTERROR JPTest::InitNewClientRun(const JPTestOptions& TestOptions)
{
    SetIsRunning();

    *this->testOptions = TestOptions;

    if (!SetUpPort()) return PORT_ERROR;

    return NO_ERROR;
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

void JPTest::GetMailFromPort()
{
    qDebug() << "In " << __FUNCTION__;
    QByteArray mail = port->ReadData();
    inbox->append(mail);
    MailReady = true;
    UnreadBytes += mail.count();
    return;
}

/**
 * @brief JPTest::WaitForServerStart waits for data to come in from the server.
 * @return true if data received, false if run stopped by user
 */
bool JPTest::WaitForServerStart()
{
    while (this->Running())
    {
        if (!port->WaitForData(1000)) continue;
        else return true;
    }
    return false;
}

void JPTest::LoadTest(JPTestOptions Options)
{
    *this->testOptions = Options;
    if (LoadTestScript()) this->testLoaded = true;
    return;
}
