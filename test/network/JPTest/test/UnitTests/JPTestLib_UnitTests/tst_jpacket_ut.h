#ifndef TST_JPACKET_UT_H
#define TST_JPACKET_UT_H

#include <QTest>
#include "jpacket.h"

class JPacket_UT : public QObject
{
    Q_OBJECT
public:
    JPacket_UT();

private Q_SLOTS:
    void ByteToIntValue_InputHexValues();
    void ByteToIntValue_InputHexValues_data();
    void IntValueToChar_InputIntValues();
    void IntValueToChar_InputIntValues_data();
    void JPacket_GetValuesFromJPFile();
};

#endif // TST_JPACKET_UT_H
