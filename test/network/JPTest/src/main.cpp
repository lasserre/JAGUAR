#include "jptmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    JPTMainWindow w;
    w.show();
    
    return a.exec();
}
