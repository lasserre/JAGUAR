#include "jptest.h"

JPTest::JPTest(QObject *parent) :
    QObject(parent)
  , port(new JPTestPort(this))
  , testOptions(new JPTestOptions())
  , testCoordinator(new JPTestCoordinator())
  , outbox(new JPOutbox())
  , packetOutbox(new QList<QString>())
  , P2packetInbox(new QList<QString>())
  , P3packetInbox(new QList<QString>())
  , P2nextPacket(0)
  , P3nextPacket(0)
  , inbox(new QByteArray())
  , MailReady(false)
  , JaguarHeaderOffset(0)
  , currentPacketSrc(-1)
  , currentPacketLen(-1)
  , delaySecs(-1)
  , isRunning(false)
  , RemainingLoops(0)
{
    connect(this->port, SIGNAL(youveGotMail()), this, SLOT(GetMailFromPort()), Qt::DirectConnection);

}

JPTest::~JPTest()
{
    delete this->port;
    delete this->testOptions;
    delete this->testCoordinator;
    delete this->outbox;
    delete this->packetOutbox;
    delete this->P2packetInbox;
    delete this->P3packetInbox;
    delete this->inbox;
}

JPTESTERROR JPTest::InitNewRun()
{
    SetIsRunning();

    if (!SetUpPort()) return PORT_ERROR;    // change to const int PORT_ISSUE;

    // Assign NumLoops since this won't get checked until after first loop through
    this->RemainingLoops = testOptions->NumLoops;

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
    QList<QString>::iterator currentTestPacket = packetOutbox->begin();
    bool IsLoopingRun = (testOptions->NumLoops > -1);

    while(Running() && (currentTestPacket != packetOutbox->end()))
    {
        // Do delays/step/etc...
        HandleTestMode();

        // Send packets
        //if (port->SendData(GetJPktPayload(*currentTestPacket)))
          //  emit PacketSent(GetJPktPayload(*currentTestPacket));

        // Check the mail
        QCoreApplication::processEvents();
        CheckMail();

        // Increment packet ptr
        currentTestPacket++;

        if (currentTestPacket == packetOutbox->end() && IsLoopingRun)
        {
           // Handle looping
           currentTestPacket = packetOutbox->begin();  // Reset to beginning of test, in case we need to run again
           if (!RunTestAgain()) break;
        }
    }

    // If we're done sending, we may not be done receiving...
    while (Running())
    {
        QCoreApplication::processEvents();
        CheckMail();
        if (P2nextPacket == P2packetInbox->count() && P3nextPacket == P3packetInbox->count())
            break;
    }

    return;
}

void JPTest::CheckMail()
{
    if (MailReady) ProcessInbox();

    MailReady = false;

    return;
}

void JPTest::ProcessInbox()
{
    if (currentPacketSrc == -1)
    {
        // Get src byte of packet, if it has come in yet
        if (inbox->count() > (MAVPACKET::SRCBYTE + JaguarHeaderOffset))
        {
            currentPacketSrc = (unsigned char) inbox->at(JaguarHeaderOffset + MAVPACKET::SRCBYTE);
            currentPacketLen = (unsigned char) inbox->at(JaguarHeaderOffset + MAVPACKET::LENBYTE);  // Get payload length
            currentPacketLen += MAVPACKET::MVHDR_OFFSET + MAVPACKET::tail_CS_OFFSET;    // Account for header and tail
        }
        else return;    // Not enough data in yet...wait until we get at least up to the SRC byte
    }

    ProcessCurrentPacket();

    return;
}

void JPTest::ProcessCurrentPacket()
{
    if (inbox->count() < (currentPacketLen + JaguarHeaderOffset)) return;

    QList<int> conflictList;
    QByteArray receivedPacket = inbox->mid(JaguarHeaderOffset, currentPacketLen);

    if (currentPacketSrc == testOptions->P2ID && P2nextPacket < P2packetInbox->count())
    {
        //conflictList = DiffByteArrays(GetJPktPayload(P2packetInbox->at(P2nextPacket++)), receivedPacket);
        emit P2PacketReceived(receivedPacket, conflictList);
    }
    else if ((currentPacketSrc == testOptions->P3ID) && P3nextPacket < P3packetInbox->count())
    {
            //conflictList = DiffByteArrays(GetJPktPayload(P3packetInbox->at(P3nextPacket++)), receivedPacket);
            emit P3PacketReceived(receivedPacket, conflictList);
    }
    else
    {
        // Packet SRC is wrong!! Handle garbage here (send bytes up to LEN byte marked as garbage)
        emit GarbagePacketReceived(inbox->mid(JaguarHeaderOffset, MAVPACKET::LENBYTE));

        FindGoodPacketStart();

        currentPacketLen = -1;
        currentPacketSrc = -1;

        return;
    }

    inbox->remove(0, JaguarHeaderOffset + receivedPacket.length());
    currentPacketLen = -1;
    currentPacketSrc = -1;

    return;
}

void JPTest::FindGoodPacketStart()
{
    int goodPacketIndex = -1;

    for (int i = 0; i < (inbox->count() - 2); i++)
    {
        int jagStx = (unsigned char) inbox->at(i);
        if (jagStx == MAVPACKET::STXVALUE)
        {
            int mavStx = (unsigned char) inbox->at(i+2);
            if (mavStx == MAVPACKET::STXVALUE)
            {
                goodPacketIndex = i;
                break;
            }
        }
    }

    if (goodPacketIndex != -1)
        inbox->remove(0, (inbox->length() - ++goodPacketIndex));
    else
        inbox->clear();
    return;
}

QList<int> JPTest::DiffByteArrays(const QByteArray &first, const QByteArray &second)
{
    QList<int> conflictIndices;
    int maxDiffLength = first.count();

    if (second.count() != first.count())
    {
        qDebug() << "Number of bytes in packet diff do NOT match!";
        if (second.count() < first.count()) maxDiffLength = second.count();
    }

    for (int i = 0; i < maxDiffLength; i++)
    {
        if (first.at(i) != second.at(i))
            conflictIndices.append(i);
    }

    return conflictIndices;
}

void JPTest::GetMailFromPort()
{
    qDebug() << "In " << __FUNCTION__;

    QByteArray mail = port->ReadData();
    inbox->append(mail);

    MailReady = true;

    for (int i = 0; i < mail.count(); i++)
        emit ByteReceived(mail.at(i));

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

// Construct test script from .jptest file
/**
 * @brief JPTest::LoadTestScript
 * @return true if Load succesfully
 */
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
    P2packetInbox->clear();
    P3packetInbox->clear();

    qDebug() << "p3packetInbox.count(): " << P3packetInbox->count();
    qDebug() << "p3idstring: " << testOptions->GetP3IDString();

    // Create list from comma-separated-values in our JAGID's section
    QList<QByteArray> jptestlist;// = GetPacketList(testOptions->GetJagIDString(), JPTestFile);
    QList<QByteArray> p2testlist;// = GetPacketList(testOptions->GetP2IDString(), JPTestFile);
    QList<QByteArray> p3testlist;// = GetPacketList(testOptions->GetP3IDString(), JPTestFile);

    qDebug() << "p3testlist.count(): " << p3testlist.count();

    // Put list of packets we must send into packetOutbox
    QList<QByteArray>::iterator testIter = jptestlist.begin();
    QList<QByteArray>::iterator p2Iter = p2testlist.begin();
    //QList<QByteArray>::iterator p3Iter = p3testlist.begin();

    while (testIter != jptestlist.end())
        this->packetOutbox->append(QString(*testIter++));
    while (p2Iter != p2testlist.end())
        this->P2packetInbox->append(QString(*p2Iter++));
//    while (p3Iter != p3testlist.end())
//        this->P3packetInbox->append(QString(*p3Iter++));
    for (int i = 0; i < p3testlist.count(); i++)
        P3packetInbox->append(QString(p3testlist.at(i)));

    qDebug() << "p3packetInbox.count(): " << P3packetInbox->count();

    // Set packet ptrs to beginning of P2/P3 packet inboxes
    P2nextPacket = 0;
    P3nextPacket = 0;

    // Send list to GUI
    emit OutboxLoaded(jptestlist);
    emit P2InboxLoaded(p2testlist);
    emit P3InboxLoaded(p3testlist);

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
