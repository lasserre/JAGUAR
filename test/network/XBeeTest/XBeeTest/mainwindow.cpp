#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort>
#include <QDebug>
#include "xbee_framewriter.h"
#include "xbee_frameparser.h"
#include "xbee_frame_structs.h"

const int BAUD = 9600;

using namespace XbeeAPI;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , port(new QSerialPort(this))
  , frameBuffer(QByteArray())
{
    ui->setupUi(this);

    QList<QString> portNames;
    foreach(const QSerialPortInfo& portInfo, QSerialPortInfo::availablePorts())
        ui->listWidget->addItem(portInfo.portName());

    connect(ui->selectButton, SIGNAL(clicked()), this, SLOT(SelectPort()));
    connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(SendFrame()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SelectPort()
{
    if (ui->listWidget->selectedItems().count() > 0)
    {
        port->setPortName(ui->listWidget->selectedItems().at(0)->text());

        if (port->open(QSerialPort::ReadWrite))
        {
            port->setBaudRate(BAUD);

            connect(this->port, SIGNAL(readyRead()), this, SLOT(ReceiveFrame()));
            qDebug() << "Connected to: " << port->portName();
        }
        else
            qDebug() << "Unable to connect to " << ui->listWidget->selectedItems().at(0)->text();
    }
    else
        qDebug() << "Unable to connect: No items selected!";
    return;
}

void MainWindow::SendFrame()
{
    QByteArray data = GetTxReqXBFrame();

    qDebug() << "ATTEMPTING TO SEND:";
    DebugQByteArray(data);

    if (port->write(data) > 0)
    {
        qDebug() << "Sent " << data.count() << " bytes. Data: ";
        DebugQByteArray(data);
    }
    else
        qDebug() << "Unable to send data";
    return;
}

void MainWindow::ReceiveFrame()
{
    QByteArray ReceivedBytes = port->readAll();

    frameBuffer.append(ReceivedBytes);    // Add received bytes to buffer

    qDebug() << "Received data: ";
    DebugQByteArray(ReceivedBytes);

    XBFrame::FrameByteArray frame;
    frame.FrameStart = (uint8_t*)frameBuffer.data();        // Point FrameStart to start of buffer
    frame.SetFrameSize(frameBuffer.size());                 // Set Frame size appropriately

    XBFrame::Type type;
    XBFrame::ParseResult res;

    if ((res = FrameParser::ParseFrameType(frame, type)) == XBFrame::PR_ParseSuccess)
    {
        if (type == XBFrame::TransmitStatus)
        {
            qDebug() << "TransmitStatus frame received!";

            TxStatus status;
            res = FrameParser::ParseTxStatus(frame, status);

            if (res == XBFrame::PR_ParseSuccess)
            {
                qDebug() << "TxRetryCount: " << status.TxRetryCount;
                qDebug() << "DelivStatus: " << status.DelivStat;
                qDebug() << "DiscvStatus: " << status.DiscvStat;

                frameBuffer.clear();
            }
            else
            {
                qDebug() << QString::fromStdString(XbeeAPI::FrameParser::GetParseResultString(res));
                // frameBuffer.clear();
            }
        }
        else
            qDebug() << "Unhandled frame type of: " << type;
    }
    else
    {
        qDebug() << "ParseFrameType res = " <<
                    QString::fromStdString(XbeeAPI::FrameParser::GetParseResultString(res));
    }

    return;
}

QByteArray MainWindow::GetTxReqXBFrame()
{

    XbeeAPI::TxRequest options;
    options.DestAddress = BROADCAST_ADDRESS;
    options.PayloadLength = 10;
    options.PayloadStart = new uint8_t[options.PayloadLength];
    strcpy((char*)options.PayloadStart, "HELLOWORLD");

    XbeeAPI::XBFrame::FrameByteArray frame;

    frame.SetFrameSize(XbeeAPI::FrameWriter::GetTxRequestFrameSize(options));   // Set size for TxRequest frame
    frame.FrameStart = new uint8_t[frame.FrameSize()];      // Allocate memory for frame

    XbeeAPI::FrameWriter::WriteTxRequestFrame(options, frame);

    // Platform-specific code
    QByteArray result((char*)frame.FrameStart, frame.FrameSize());
    /////////////////////////

    delete [] frame.FrameStart;
    delete [] options.PayloadStart;

    return result;
}

void MainWindow::DebugQByteArray(const QByteArray &Array)
{
    QString out;

    for (int i = 0; i < 2*Array.count(); i++)
    {
      out += Array.toHex()[i];
      out += Array.toHex()[++i];
      out += " ";
    }

    qDebug() << out;

    return;
}
