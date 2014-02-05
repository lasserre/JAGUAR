#include "tst_jptestfilereader_ut.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    JPTestFileReader_UT fileReaderUT;
    QTest::qExec(&fileReaderUT, argc, argv);

    return 0;
}
