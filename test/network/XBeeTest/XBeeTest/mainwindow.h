#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>

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

    void LogMessage(const QString& Message);
    void DebugQByteArray(const QByteArray& Array);
};

#endif // MAINWINDOW_H
