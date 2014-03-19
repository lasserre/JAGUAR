#include "jptestport.h"

JPTestPort::JPTestPort(QObject* parent /*= 0*/) : QObject(parent)
  , portType(TestPort::SERIAL_PORT)
  , port(ConstructTestPort())
{
    connect(port, SIGNAL(readyRead()), this, SLOT(DataReceivedHandler()), Qt::DirectConnection);
}

JPTestPort::~JPTestPort()
{
    delete this->port;
}

bool JPTestPort::OpenPort()
{
    if (portType == TestPort::SERIAL_PORT)
    {
        QSerialPort* serialPort = static_cast<QSerialPort*>(port);

        if (serialPort->isOpen()) return true;

        // Always open in R/W mode
        if (serialPort->open(QSerialPort::ReadWrite))
        {
            qDebug() << "Opened serial port: " + serialPort->portName();

            // Configure serialPort
            serialPort->setBaudRate(SERIAL_CONFIG::BAUD);
            serialPort->setDataBits(SERIAL_CONFIG::DATA_BITS);
            serialPort->setParity(SERIAL_CONFIG::PARITY);
            serialPort->setStopBits(SERIAL_CONFIG::STOP_BITS);
            serialPort->setFlowControl(SERIAL_CONFIG::FLOW_CTRL);

            return true;
        }
        else
        {
            qDebug() << "Failed attempt to open serial port: " + serialPort->portName();
            return false;
        }
    }
}

bool JPTestPort::IsOpen() const
{
    return port->isOpen();
}

void JPTestPort::SetPortName(const QString &PortName)
{
    if (portType == TestPort::SERIAL_PORT)
    {
        QSerialPort* serialPort = static_cast<QSerialPort*>(port);
        serialPort->setPortName(PortName);
    }
    return;
}

QList<QString> JPTestPort::GetAvailableSerialPortNames()
{
    QList<QString> portNames;
    foreach(const QSerialPortInfo& portInfo, QSerialPortInfo::availablePorts())
        portNames.append(portInfo.portName());

    return portNames;
}

bool JPTestPort::SendData(const QByteArray& payload)
{
    port->write(payload);
    return port->waitForBytesWritten(SERIAL_CONFIG::WRITETIMEOUT_MSEC);
}

QByteArray JPTestPort::ReadData()
{
    return port->readAll();
}

// --------------- SLOTS -------------------

void JPTestPort::DataReceivedHandler()
{
    emit youveGotMail();  // Complete with dial-up sound effects!
    return;
}

bool JPTestPort::WaitForData(const int &msecs)
{
    return port->waitForReadyRead(msecs);
}

QIODevice* JPTestPort::ConstructTestPort()
{
    QIODevice* portPointer;

    switch (portType)
    {
    case TestPort::SERIAL_PORT:
    {
        QSerialPort* serialPort = new QSerialPort(this);
        SetPortName("UNSET_PORT_NAME");   // Help ourselves out for debugging!
        portPointer = serialPort;
        break;
    }
    case TestPort::UDP_SOCKET:
    {
        QUdpSocket* udpSocket = new QUdpSocket(this);
        udpSocket->bind(QHostAddress::LocalHost, 5534);
        portPointer = udpSocket;
        break;
    }
    default:
    {
        qDebug() << "Unrecognized TestPort::TestPortType!";
        break;
    }
    }

    return portPointer;
}
