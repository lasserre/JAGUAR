#include "jpacketchecker.h"

JPacketChecker::JPacketChecker()
{
}

//QList<int> JPTest::DiffByteArrays(const QByteArray &first, const QByteArray &second)
//{
//    QList<int> conflictIndices;
//    int maxDiffLength = first.count();

//    if (second.count() != first.count())
//    {
//        qDebug() << "Number of bytes in packet diff do NOT match!";
//        if (second.count() < first.count()) maxDiffLength = second.count();
//    }

//    for (int i = 0; i < maxDiffLength; i++)
//    {
//        if (first.at(i) != second.at(i))
//            conflictIndices.append(i);
//    }

//    return conflictIndices;
//}
