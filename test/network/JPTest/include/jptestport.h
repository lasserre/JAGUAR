#ifndef JPTESTPORT_H
#define JPTESTPORT_H

#include <QObject>
#include <QIODevice>
#include <QDebug>
#include <QtSerialPort/QtSerialPort>
#include <QtNetwork>
#include <QUdpSocket>
#include <QMessageBox>

// CLS - we can modify this later to be set in a config file, but just putting here for now
namespace SERIAL_CONFIG
{
    const QSerialPort::BaudRate BAUD = QSerialPort::Baud57600;
    const QSerialPort::DataBits DATA_BITS = QSerialPort::Data8;
    const QSerialPort::Parity PARITY = QSerialPort::NoParity;
    const QSerialPort::StopBits STOP_BITS = QSerialPort::OneStop;
    const QSerialPort::FlowControl FLOW_CTRL = QSerialPort::NoFlowControl;
    const int WRITETIMEOUT_MSEC = 1000;     // Length of time to wait for a write to serial port to return successfully
}

namespace TestPort {

enum TestPortTypes
{
    SERIAL_PORT,
    UDP_SOCKET
};

}

class JPTestPort : public QObject
{
    Q_OBJECT

signals:
    void youveGotMail();

public slots:
    void DataReceivedHandler();

public:
    explicit JPTestPort(QObject* parent = 0);
    ~JPTestPort();
    bool OpenPort();
    bool IsOpen() const;
    void SetPortName(const QString& PortName);
    bool SendData(const QByteArray &payload);
    QByteArray ReadData();
    bool WaitForData(const int& msecs);

    static QList<QString> GetAvailableSerialPortNames();

protected:
    TestPort::TestPortTypes portType;
    QIODevice* port;

    QIODevice* ConstructTestPort();
};

#endif // JPTESTPORT_H
