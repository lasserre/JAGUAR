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
    // Verify our current directory is in the test/UnitTests/bin directory
    qDebug() << QDir::currentPath();

    QFile jptestfile("../../../jptestfiles/CorrectJptestFile.jptest");
    jptestfile.open(QFile::ReadOnly);

    //JPTestFileReader fileReader;

    QFETCH(QString, JAGID);
    QFETCH(QStringList, Result);

    // QVERIFY2()
    QCOMPARE(JPTestFileReader::GetPacketList(JAGID, jptestfile), Result);
}

//QTEST_APPLESS_MAIN(JPTestFileReader_UT)

//#include "tst_jptestfilereader_ut.moc"
