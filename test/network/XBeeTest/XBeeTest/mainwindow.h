#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>
#include "xbee_framewriter.h"
#include "xbee_frameparser.h"
#include "xbee_frame_structs.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void SelectPort();
    void SendFrame();
    void ReceiveFrame();

private:
    Ui::MainWindow *ui;
    QSerialPort* port;
    QByteArray frameBuffer;

    QByteArray GetTxReqXBFrame();
    QByteArray GetExAddrXBFrame();

    void LogMessage(const QString& Message);
    void DebugQByteArray(const QByteArray& Array);

    void HandleTxStatus(XbeeAPI::XBFrame::FrameByteArray* TxStatusFrame);
    void HandleRxPacket(XbeeAPI::XBFrame::FrameByteArray* RxPacketFrame);
    void HandleExRxIndicator(XbeeAPI::XBFrame::FrameByteArray* ExRxIndFrame);
};

#endif // MAINWINDOW_H
