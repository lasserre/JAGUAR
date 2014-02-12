#ifndef TST_JPACKETCHECKER_UT_H
#define TST_JPACKETCHECKER_UT_H

#include <QTest>
#include "jpacketchecker.h"

class JPacketChecker_UT : public QObject
{
    Q_OBJECT

public:
    JPacketChecker_UT();

private Q_SLOTS:
    void CheckMail_TestDiffs();
};

#endif // TST_JPACKETCHECKER_UT_H
