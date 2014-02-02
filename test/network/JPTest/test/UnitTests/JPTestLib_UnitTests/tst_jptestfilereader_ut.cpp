#include <QString>
#include <QtTest>

class JPTestFileReader_UT : public QObject
{
    Q_OBJECT

public:
    JPTestFileReader_UT();

private Q_SLOTS:
    void testCase1();
};

JPTestFileReader_UT::JPTestFileReader_UT()
{
}

void JPTestFileReader_UT::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(JPTestFileReader_UT)

#include "tst_jptestfilereader_ut.moc"
