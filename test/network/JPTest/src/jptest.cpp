#include "jptest.h"

JPTest::JPTest(QObject *parent) :
    QObject(parent)
  , port(new JPTestPort(this))
  , jpacketLib(new QMap<QString, JPacket>())
  , jptestScript(new QList<QString>())
  , jptestFilename(QString("NoFilenameSpecified"))
  , delaySecs(-1)
{
}

JPTest::~JPTest()
{
    delete this->port;
    delete this->jpacketLib;
    delete this->jptestScript;
}

JPTestResults JPTest::Run(const JPTestOptions &TestOptions)
{
    JPTestResults testResults;

    // Set filename and load .jptest file
    this->jptestFilename = TestOptions.Filename;
    if (!LoadTestScript()) return testResults;
    testResults.NumOutgoingMsgs = jptestScript->length();   //TEMP!! This assumes everything is outgoing!
    // ^^^^^^^^^^^^^^^^^^^ CHANGE ^^^^^^^^^^^^^^^^^^^^^^

    // Run loop
    QList<QString>::iterator currentTestPacket = jptestScript->begin();
    while(currentTestPacket != jptestScript->end())
    {
        // Run appropriate test mode
        switch(TestOptions.RunMode)
        {
        case RUN:
            break;
        case STEP:
            break;
        case DELAY:
            if (-1 != TestOptions.DelaySecs)
            {
                // Delay
                QThread::sleep(TestOptions.DelaySecs);
            }
            else
            {
                qDebug() << "Delay mode specified with invalid (or no) delay!";
                return testResults;
            }
            break;
        default:
            // Shouldn't get here!
            qDebug() << "Unrecognized test mode!";
            break;
        }

        if (port->SendData(GetJPktPayload(*currentTestPacket))) testResults.NumMsgsSent++;

        currentTestPacket++;
        if (currentTestPacket == jptestScript->end() && (TestOptions.NumLoops > -1))
        {
            // Completed one run. Handle looping here, if we are wanting to loop.
            static int RemainingLoops = TestOptions.NumLoops;   // Assign number of loops
            currentTestPacket = jptestScript->begin();
            if (TestOptions.NumLoops == 0) continue;    // NumLoops of 0 means loop forever (until manually stopped)
            else RemainingLoops--;      // Account for the loop just completed
            if (!RemainingLoops) break;
        }
    }

    return testResults;
}

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

QByteArray JPTest::GetJPktPayload(const QString &PacketFilename)
{
    if (this->jpacketLib->contains(PacketFilename))
    {
        return jpacketLib->value(PacketFilename).GetPayload();
    }
    else
    {
        // Load from file
        JPacket packet(PacketFilename);
        jpacketLib->insert(PacketFilename, packet);
        return packet.GetPayload();
    }
}
