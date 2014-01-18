#ifndef JPTESTPORT_H
#define JPTESTPORT_H

#include <QObject>
#include <QDebug>
#include <QtSerialPort/QtSerialPort>

// CLS - we can modify this later to be set in a config file, but just putting here for now
namespace SERIAL_CONFIG
{
    const QSerialPort::BaudRate BAUD = QSerialPort::Baud57600;
    const QSerialPort::DataBits DATA_BITS = QSerialPort::Data8;
    const QSerialPort::Parity PARITY = QSerialPort::NoParity;
    const QSerialPort::StopBits STOP_BITS = QSerialPort::OneStop;
    const QSerialPort::FlowControl FLOW_CTRL = QSerialPort::NoFlowControl;
}

class JPTestPort : public QObject
{

public:
    explicit JPTestPort(QObject* parent = 0);
    ~JPTestPort();
    bool OpenPort();
    void SetPortName(const QString& PortName);
    QList<QString> GetAvailablePortNames();

protected:
    QSerialPort* serialPort;
};

#endif // JPTESTPORT_H
