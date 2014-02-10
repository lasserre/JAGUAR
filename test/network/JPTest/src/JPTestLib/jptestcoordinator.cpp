#include "jptestcoordinator.h"

JPTestCoordinator::JPTestCoordinator(QObject *parent) : myOutbox(new QStringList())
  , P2Inbox(new QStringList())
  , P3Inbox(new QStringList())
  , jpacketLib(new QMap<QString, JPacket>())
  , testOptions(new JPTestOptions())
  , outbox(new JPOutbox(this))
  , inbox(new JPInbox(this))
  , port(new JPTestPort(this))
{
    connect(this->port, SIGNAL(youveGotMail()), this, SLOT(GetMailFromPort()));
}

JPTestCoordinator::~JPTestCoordinator()
{
    delete myOutbox;
    delete P2Inbox;
    delete P3Inbox;
    delete jpacketLib;
    delete testOptions;
    delete outbox;
    delete inbox;
    delete port;
}

QByteArray JPTestCoordinator::SendNextPacket()
{
    QByteArray nextPacket = outbox->GetSendNextPacket();
    port->SendData(nextPacket);

    return nextPacket;
}

JPacketDiffResults JPTestCoordinator::CheckMail()
{
    UpdateInbox();      // Make sure inbox is up-to-date

    return inbox->CheckMail();
}

bool JPTestCoordinator::MoreToSend()
{
    bool MoreDataToSend = false;

    if (outbox->MoreToSend())   // Data still ready to be sent from outbox
        MoreDataToSend =  true;
    else
    {
        JPacket nextPacket = GetNextOutgoingPacket();
        if (!nextPacket.GetPayload().isEmpty())
        {
            MoreDataToSend = true;
            outbox->SetNextPacket(nextPacket.GetPayload());
        }
    }

    return MoreDataToSend;
}

bool JPTestCoordinator::MoreToReceive() const
{
    bool MoreDataToReceive;

    // If inbox is still waiting for incoming data, there's more to receive

    if (inbox->WaitingOnP2Data() || inbox->WaitingOnP3Data())
        MoreDataToReceive = true;
    else
    {
        // Inbox not waiting on anybody...check internal testlist
        if (this->P2Inbox->empty() && this->P3Inbox->empty())
            MoreDataToReceive = false;
        else
            MoreDataToReceive = true;
    }

    return MoreDataToReceive;
}

QList<QStringList> JPTestCoordinator::LoadTest(const JPTestOptions &Options)
{
    QList<QStringList> LoadedMailboxes;

    QFile jptestfile(Options.Filename);

    if (jptestfile.open(QIODevice::ReadOnly))
    {
        *testOptions = Options;     // Save test options

        if (SetUpPort())
            LoadedMailboxes = LoadTestScript(jptestfile);
    }
    else qDebug() << "Unable to open .jptest file " + Options.Filename;

    return LoadedMailboxes;
}

QList<QStringList> JPTestCoordinator::LoadTestScript(QFile &JPTestFile)
{
    // Get rid of existing items, if any
    myOutbox->clear();
    P2Inbox->clear();
    P3Inbox->clear();

    // Load inboxes and outbox
    *myOutbox = JPTestFileReader::GetPacketList(testOptions->GetJagIDString(), JPTestFile);
    *P2Inbox = JPTestFileReader::GetPacketList(testOptions->GetP2IDString(), JPTestFile);
    *P3Inbox = JPTestFileReader::GetPacketList(testOptions->GetP3IDString(), JPTestFile);

    QList<QStringList> mailboxes;
    mailboxes << *myOutbox << *P2Inbox << *P3Inbox;

    return mailboxes;
}

bool JPTestCoordinator::SetUpPort()
{
    if (port->IsOpen())   // Port can't be changed once is open (for now)
        return true;

    // Set port name
    port->SetPortName(this->testOptions->PortName);

    // Open port
    return port->OpenPort();
}

JPacket JPTestCoordinator::GetNextOutgoingPacket()
{
    if (myOutbox->empty())
        return JPacket();

    // Return the packet from the front of the list, and then remove from list
    JPacket nextOutgoingPacket = GetJPkt(myOutbox->at(0));
    myOutbox->removeFirst();

    return nextOutgoingPacket;
}

JPacket JPTestCoordinator::GetNextP2IncomingPacket()
{
    if (P2Inbox->empty())
        return JPacket();

    // Return the packet from the front of the list, and then remove from list
    JPacket nextP2IncomingPacket = GetJPkt(P2Inbox->at(0));
    P2Inbox->removeFirst();

    return nextP2IncomingPacket;
}

JPacket JPTestCoordinator::GetNextP3IncomingPacket()
{
    if (P3Inbox->empty())
        return JPacket();

    // Return the packet from the front of the list, and then remove from list
    JPacket nextP3IncomingPacket = GetJPkt(P3Inbox->at(0));
    P3Inbox->removeFirst();

    return nextP3IncomingPacket;
}

// If packet exists in RAM, returns payload. Otherwise, load into library and return payload
JPacket JPTestCoordinator::GetJPkt(const QString &PacketFilename)
{
    if (this->jpacketLib->contains(PacketFilename))
    {
        return jpacketLib->value(PacketFilename);
    }
    else
    {
        // Load from file
        JPacket packet(testOptions->JPacketPath + PacketFilename);
        jpacketLib->insert(PacketFilename, packet);
        return packet;
    }
}

void JPTestCoordinator::GetMailFromPort()
{
    QByteArray mail = port->ReadData();
    inbox->AddToInbox(mail);

    for (int i = 0; i < mail.count(); i++)
        ;
        //emit ByteReceived(mail.at(i));
        //CLS - where to put this???? Inbox needs to emit with diff per byte...

    return;
}

bool JPTestCoordinator::WaitForDataReceived(const int& msecs)
{
    if (port->WaitForData(msecs)) return true;
    return false;
}

void JPTestCoordinator::UpdateInbox()
{
    if (!P2Inbox->empty() && !inbox->WaitingOnP2Data())
        inbox->SetNextP2ExpectedPacket(GetNextP2IncomingPacket());
    if (!P3Inbox->empty() && !inbox->WaitingOnP3Data())
        inbox->SetNextP3ExpectedPacket(GetNextP3IncomingPacket());
    return;
}
