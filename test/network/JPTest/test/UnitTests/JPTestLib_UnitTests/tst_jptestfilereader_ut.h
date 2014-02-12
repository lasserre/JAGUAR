#ifndef TST_JPTESTFILEREADER_UT_H
#define TST_JPTESTFILEREADER_UT_H

#include <QString>
#include <QStringList>
#include <QtTest>
#include "jptestfilereader.h"

class JPTestFileReader_UT : public QObject
{
    Q_OBJECT

public:
    JPTestFileReader_UT();

private Q_SLOTS:
    void GetPacketList_CorrectJptestFile_OneListPerJAGID();
    void GetPacketList_CorrectJptestFile_OneListPerJAGID_data();
    void GetPacketList_CompactNotation();
    void GetPacketList_CompactNotation_data();
};

#endif // TST_JPTESTFILEREADER_UT_H
