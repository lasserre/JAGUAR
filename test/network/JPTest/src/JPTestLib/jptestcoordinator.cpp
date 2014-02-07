#include "jptestcoordinator.h"

JPTestCoordinator::JPTestCoordinator(QObject *parent) : myOutbox(new QStringList())
  , P2Inbox(new QStringList())
  , P3Inbox(new QStringList())
  , jpacketLib(new QMap<QString, JPacket>())
  , testOptions(new JPTestOptions())
{
}

JPTestCoordinator::~JPTestCoordinator()
{
    delete myOutbox;
    delete P2Inbox;
    delete P3Inbox;
    delete jpacketLib;
    delete testOptions;
}

bool JPTestCoordinator::LoadTest(const JPTestOptions &Options)
{
    bool success = false;
    QFile jptestfile(Options.Filename);

    if (jptestfile.open(QIODevice::ReadOnly))
    {
        LoadTestScript(Options, jptestfile);
        success = true;
    }
    else qDebug() << "Unable to open .jptest file " + Options.Filename;

    return success;
}

void JPTestCoordinator::LoadTestScript(const JPTestOptions& Options, QFile &JPTestFile)
{
    *testOptions = Options;     // Save test options

    // Get rid of existing items, if any
    myOutbox->clear();
    P2Inbox->clear();
    P3Inbox->clear();

    // Load inboxes and outbox
    *myOutbox = JPTestFileReader::GetPacketList(testOptions->GetJagIDString(), JPTestFile);
    *P2Inbox = JPTestFileReader::GetPacketList(testOptions->GetP2IDString(), JPTestFile);
    *P3Inbox = JPTestFileReader::GetPacketList(testOptions->GetP3IDString(), JPTestFile);

    emit OutboxLoaded(*myOutbox);
    emit P2InboxLoaded(*P2Inbox);
    emit P3InboxLoaded(*P3Inbox);

    return;
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
