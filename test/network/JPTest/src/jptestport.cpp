#include "jptestport.h"

JPTestPort::JPTestPort(QObject* parent /*= 0*/) : QObject(parent)
  , serialPort(new QSerialPort(parent))
{
    this->SetPortName("UNSET_PORT_NAME");   // Help ourselves out for debugging!

    connect(this->serialPort, SIGNAL(readyRead()), this, SLOT(DataReceivedHandler()));
}

JPTestPort::~JPTestPort()
{
    delete this->serialPort;
}

bool JPTestPort::OpenPort()
{
    // Always open in R/W mode
    if (this->serialPort->open(QSerialPort::ReadWrite))
    {
        qDebug() << "Opened serial port: " + this->serialPort->portName();

        // Configure serialPort
        this->serialPort->setBaudRate(SERIAL_CONFIG::BAUD);
        this->serialPort->setDataBits(SERIAL_CONFIG::DATA_BITS);
        this->serialPort->setParity(SERIAL_CONFIG::PARITY);
        this->serialPort->setStopBits(SERIAL_CONFIG::STOP_BITS);
        this->serialPort->setFlowControl(SERIAL_CONFIG::FLOW_CTRL);

        return true;
    }
    else
    {
        qDebug() << "Failed attempt to open serial port: " + this->serialPort->portName();
        return false;
    }
}

void JPTestPort::SetPortName(const QString &PortName)
{
    this->serialPort->setPortName(PortName);
    return;
}

QList<QString> JPTestPort::GetAvailablePortNames()
{
    QList<QString> portNames;
    foreach(const QSerialPortInfo& portInfo, QSerialPortInfo::availablePorts())
    {
        portNames.append(portInfo.portName());
    }
    return portNames;
}

bool JPTestPort::SendData(const QByteArray& payload)
{
    this->serialPort->write(payload);
    return this->serialPort->waitForBytesWritten(SERIAL_CONFIG::WRITETIMEOUT_MSEC);
}

QByteArray JPTestPort::ReadData()
{
    return this->serialPort->readAll();
}

// --------------- SLOTS -------------------

void JPTestPort::DataReceivedHandler()
{
    emit youveGotMail();  // Complete with dial-up sound effects!
    return;
}
