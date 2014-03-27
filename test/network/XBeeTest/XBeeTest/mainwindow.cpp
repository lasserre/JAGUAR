#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort>
#include <QDebug>
#include "xbee_frameconstructor.h"
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

    Frame::Type type;
    if (FrameParser::ParseFrameType(frameBuffer.data(), frameBuffer.count(), type))
    {
        if (type == Frame::TransmitStatus)
        {
            qDebug() << "TransmitStatus frame received!";

            TxStatus status;
            if (FrameParser::ParseTxStatus(frameBuffer.data(), frameBuffer.count(), status))
            {
                qDebug() << "TxRetryCount: " << status.TxRetryCount;
                qDebug() << "DelivStatus: " << status.DelivStat;
                qDebug() << "DiscvStatus: " << status.DiscvStat;

                frameBuffer.clear();
            }
        }
        else
        {
            frameBuffer.clear();
        }
    }

    return;
}

QByteArray MainWindow::GetTxReqXBFrame()
{

    XbeeAPI::TxRequest options;
    options.DestAddress = BROADCAST_ADDRESS;
    options.PayloadLength = 10;
    options.PayloadStart = new char[options.PayloadLength];
    strcpy(options.PayloadStart, "HELLOWORL");

    int frameLen = XbeeAPI::FrameConstructor::GetTxRequestFrameSize(options);

    char* frame = new char[frameLen];

    XbeeAPI::FrameConstructor::WriteTxRequestFrame(options, frame);

    QByteArray result(frame, frameLen);

    delete [] frame;
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
