#include "tst_jpacket_ut.h"

JPacket_UT::JPacket_UT()
{
}

void JPacket_UT::ByteToIntValue_InputHexValues_data()
{
    int zero = 0;
    int one = 1;
    int thirtythree = 33;
    int twofiftyfive = 255;

    QTest::addColumn<char>("Byte");
    QTest::addColumn<int>("Result");

    QTest::newRow("0x00") << (char)0x00 << zero;
    QTest::newRow("0x01") << (char)0x01 << one;
    QTest::newRow("0x21") << (char)0x21 << thirtythree;
    QTest::newRow("0xff") << (char)0xff << twofiftyfive;
}

void JPacket_UT::ByteToIntValue_InputHexValues()
{
    QFETCH(char, Byte);
    QFETCH(int, Result);

    QCOMPARE(JPacket::ByteToIntValue(Byte), Result);
    return;
}

void JPacket_UT::IntValueToChar_InputIntValues_data()
{
    char zero = (char) 0x00;
    char one = (char) 0x01;
    char thirtythree = (char) 0x21;
    char twofiftyfive = (char) 0xff;

    QTest::addColumn<int>("Int");
    QTest::addColumn<char>("Result");

    QTest::newRow("0") << (int) 0x00 << zero;
    QTest::newRow("1") << (int) 0x01 << one;
    QTest::newRow("33") << (int) 0x21 << thirtythree;
    QTest::newRow("255") << (int) 0xff << twofiftyfive;

    return;
}

void JPacket_UT::IntValueToChar_InputIntValues()
{
    QFETCH(int, Int);
    QFETCH(char, Result);

    QCOMPARE(JPacket::IntValueToChar(Int), Result);
    return;
}

void JPacket_UT::JPacket_GetValuesFromJPFile()
{
    QString A2Gfilename = "../../../jptestfiles/jpackets/simpleA2G";
    JPacket jp(A2Gfilename);

    QCOMPARE(jp.GetSrc(), JAGPACKET::MS);
    QCOMPARE(jp.GetDst(), JAGPACKET::GCS);
    return;
}
