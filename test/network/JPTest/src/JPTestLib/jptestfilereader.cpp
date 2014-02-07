#include "jptestfilereader.h"

JPTestFileReader::JPTestFileReader()
{
}

/**
 * @brief JPTestFileReader::GetPacketList returns the list of packet names for a given JAGID and .jptest file.
 * Note that GetPacketList resets the QFile& that is passed in back to the beginning of the file and does not
 * close it.
 * @param JAGID
 * @param JPTestFile
 * @return QStringList of packets
 */
QStringList JPTestFileReader::GetPacketList(const QString &JAGID, QFile &JPTestFile)
{
    QList<QByteArray> linesToParse = GetFileLinesForJAGID(JAGID, JPTestFile);
    QStringList packetList = ConvertFileLinesToPacketList(linesToParse);

    JPTestFile.seek(0);     // Reset to beginning of file for future function calls.
    if (JPTestFile.atEnd())
        qDebug() << "File at end after seek() call!";

    return packetList;
}

/**
 * @brief JPTestFileReader::GetFileLinesForJAGID reads the lines listing the outgoing packets for a given JAGUAR ID.
 * @param JAGID
 * @param JPTestFile
 * @return A QList<QByteArray> of raw file lines that need to be parsed
 */
QList<QByteArray> JPTestFileReader::GetFileLinesForJAGID(const QString &JAGID, QFile &JPTestFile)
{
    QList<QByteArray> lines;
    QByteArray lineBuffer;
    bool found = false;

    // Find <[JAGID]> line that matches our ID
    while (!JPTestFile.atEnd() && !found)
    {
        lineBuffer = JPTestFile.readLine();   // Get next line

        if ((lineBuffer[0] == '<') && (lineBuffer.size() > JAGID.size()))
            // If the lineBuffer length isn't at least 1 char longer than our string ('<' + string), it's not a match!
        {
            bool match = true;

            for (int i = 0; i < JAGID.length(); i++)
            {
                if (lineBuffer.at(i+1) != JAGID.at(i))
                {
                    match = false;
                    break;
                }
            }

            if (match) found = true;
        }
    }

    // Get lines below it...
    if (found)
    {
        while(!JPTestFile.atEnd())
        {
            lineBuffer = JPTestFile.readLine();

            if (lineBuffer[0] == '<') break;    // If we hit a line starting with '<', we're done.
            else lines << lineBuffer;
        }
    }

    return lines;
}

QStringList JPTestFileReader::ConvertFileLinesToPacketList(const QList<QByteArray> &FileLines)
{
    QStringList packetList;

    for (int i = 0; i < FileLines.count(); i++)
    {
        //QByteArray lineWithoutSpaces = RemoveAllOccurrences(FileLines[i], ' ');
        packetList << ParseLine(FileLines[i]);
    }

    return packetList;
}

QStringList JPTestFileReader::ParseLine(const QByteArray& FileLine)
{
    QStringList packetsFromLine;
    bool compactNotation = false;

    if (FileLine.contains(':')) compactNotation = true;

    QList<QByteArray> csvList = FileLine.split(',');
    for (int i = 0; i < csvList.length(); i++)
        csvList[i] = csvList[i].trimmed();      // Remove all whitespace characters (including \n, \t, etc.)

    if (compactNotation)
    {
        QList<QByteArray> compactArgSplit;

        for (int i = 0; i < csvList.length(); i++)
        {
            compactArgSplit = csvList[i].split(':');
            if (compactArgSplit.length() == 1)
            {
                // No ':' separator found. => Normal arg, so add to packet list
                packetsFromLine << csvList[i];
            }
            else if (compactArgSplit.length() == 2)
            {
                // Compact notation found. Proper form is JP:n where JP is JPacket name and n is # times to send it
                int n = compactArgSplit[1].toInt();

                for (int i = 0; i < n; i++)          // Add packet n times
                    packetsFromLine << compactArgSplit[0];
            }
        }
    }
    else
    {
        // Only normal notation. Split packets by commas only
        for (int i = 0; i < csvList.length(); i++)
            packetsFromLine << csvList[i];
    }

    return packetsFromLine;
}
