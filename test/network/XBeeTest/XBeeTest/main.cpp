#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <xbee_api_basetypes.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    qDebug() << XbeeAPI::Frame::TransmitRequest;

    return a.exec();
}
