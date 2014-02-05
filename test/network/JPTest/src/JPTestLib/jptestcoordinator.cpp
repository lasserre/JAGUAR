#include "jptestcoordinator.h"

JPTestCoordinator::JPTestCoordinator() : myOutbox(new QStringList())
  , P2Inbox(new QStringList())
  , P3Inbox(new QStringList())
  , jpacketLib(new QMap<QString, JPacket>())
  , fileReader(new JPTestFileReader())
{
}

JPTestCoordinator::~JPTestCoordinator()
{
    delete myOutbox;
    delete P2Inbox;
    delete P3Inbox;
    delete jpacketLib;
    delete fileReader;
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
    // Get rid of existing items, if any
    myOutbox->clear();
    P2Inbox->clear();
    P3Inbox->clear();

    // Load inboxes and outbox
    *myOutbox = fileReader->GetPacketList(Options.GetJagIDString(), JPTestFile);
    *P2Inbox = fileReader->GetPacketList(Options.GetP2IDString(), JPTestFile);
    *P3Inbox = fileReader->GetPacketList(Options.GetP3IDString(), JPTestFile);

    return;
}
