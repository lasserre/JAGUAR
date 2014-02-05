#ifndef JPTESTFILEREADER_H
#define JPTESTFILEREADER_H

#include <QString>
#include <QDebug>
#include <QFile>
#include <QStringList>

class JPTestFileReader
{
public:
    JPTestFileReader();
    static QStringList GetPacketList(const QString& JAGID, QFile& JPTestFile);

protected:
    static QByteArray RemoveAllOccurrences(QByteArray stream, const char &deleteChar);
    static QList<QByteArray> GetFileLinesForJAGID(const QString& JAGID, QFile& JPTestFile);
    static QStringList ConvertFileLinesToPacketList(const QList<QByteArray>& FileLines);
    static QStringList ParseLine(const QByteArray &LineWithoutSpaces);
};

#endif // JPTESTFILEREADER_H