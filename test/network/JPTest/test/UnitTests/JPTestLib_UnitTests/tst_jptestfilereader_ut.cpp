#include "tst_jptestfilereader_ut.h"

JPTestFileReader_UT::JPTestFileReader_UT()
{
}

void JPTestFileReader_UT::GetPacketList_CorrectJptestFile_OneListPerJAGID_data()
{
    QStringList gcsResult("GCSoutPacket");
    QStringList msResult("MSoutPacket");
    QStringList qcResult("QCoutPacket");
    QStringList broadcastResult("BroadcastoutPacket");

    QTest::addColumn<QString>("JAGID");
    QTest::addColumn<QStringList>("Result");

    QTest::newRow("GCS PacketList") << "GCS" << gcsResult;
    QTest::newRow("MS PacketList") << "MS" << msResult;
    QTest::newRow("QC PacketList") << "QC" << qcResult;
    QTest::newRow("Broadcast PacketList") << "Broadcast" << broadcastResult;
}

void JPTestFileReader_UT::GetPacketList_CorrectJptestFile_OneListPerJAGID()
{
    QFile jptestfile("../../../jptestfiles/CorrectJptestFile.jptest");
    jptestfile.open(QFile::ReadOnly);

    //JPTestFileReader fileReader;

    QFETCH(QString, JAGID);
    QFETCH(QStringList, Result);

    // QVERIFY2()
    QCOMPARE(JPTestFileReader::GetPacketList(JAGID, jptestfile), Result);
}

void JPTestFileReader_UT::GetPacketList_CompactNotation_data()
{
    QStringList gcsResult;
    QStringList msResult("MSoutPacket");
    QStringList qcResult;
    QStringList broadcastResult("BroadcastoutPacket");

    for (int i = 0; i < 2; i++)
        gcsResult << "GCSoutPacket";
    for (int i = 0; i < 5; i++)
        qcResult << "QCoutPacket";

    QTest::addColumn<QString>("JAGID");
    QTest::addColumn<QStringList>("Result");

    QTest::newRow("GCS PacketList") << "GCS" << gcsResult;
    QTest::newRow("MS PacketList") << "MS" << msResult;
    QTest::newRow("QC PacketList") << "QC" << qcResult;
    QTest::newRow("Broadcast PacketList") << "Broadcast" << broadcastResult;

    return;
}

void JPTestFileReader_UT::GetPacketList_CompactNotation()
{
    QFile jptestfile("../../../jptestfiles/MultiplePacketTest.jptest");
    jptestfile.open(QFile::ReadOnly);

    //JPTestFileReader fileReader;

    QFETCH(QString, JAGID);
    QFETCH(QStringList, Result);

    // QVERIFY2()
    QCOMPARE(JPTestFileReader::GetPacketList(JAGID, jptestfile), Result);
    return;
}
