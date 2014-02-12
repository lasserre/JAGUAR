#include "tst_jptestfilereader_ut.h"
#include "tst_jptestcoordinator_ut.h"
#include "tst_jpacket_ut.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    JPTestFileReader_UT fileReaderUT;
    QTest::qExec(&fileReaderUT, argc, argv);

    JPTestCoordinator_UT testCoordinatorUT;
    QTest::qExec(&testCoordinatorUT, argc, argv);

    JPacket_UT jpacketUT;
    QTest::qExec(&jpacketUT, argc, argv);

    return 0;
}
