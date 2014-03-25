#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort>
#include <QDebug>
#include "xbee_frameconstructor.h"
#include "xbee_frame_params.h"

const int BAUD = 9600;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , port(new QSerialPort(this))
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
    QByteArray Frame = port->readAll();
    qDebug() << "Received data: " << Frame;
    return;
}

QByteArray MainWindow::GetTestXBFrame()
{
    QByteArray testFrame;

    // START
    testFrame.append(char(0x7e));

    // LENGTH
    testFrame.append(char(0x00));
    testFrame.append(0x1a);

    // TYPE
    testFrame.append(0x11);

    // FRAME ID
    testFrame.append(0x01);

    // 8B (64-bit) DEST ADDRESS
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));
    testFrame.append(0xff);
    testFrame.append(0xff);

    // RESERVED
    testFrame.append(0xff);
    testFrame.append(0xfe);

    // SRC ENDPOINT
    testFrame.append(0xe8);

    // DST ENDPOINT
    testFrame.append(0xe8);

    // CID
    testFrame.append(char(0x00));
    testFrame.append(0x12);

    // PROFILE ID
    testFrame.append(0xc1);
    testFrame.append(0x05);

    // BROADCAST RADIUS
    testFrame.append(char(0x00));

    // TX OPTIONS
    testFrame.append(char(0x00));

    // PAYLOAD
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));
    testFrame.append(char(0x00));

    // CHECKSUM
    testFrame.append(char(0x00));

    return testFrame;
}

QByteArray MainWindow::GetTrRequest()
{
    QByteArray txRequest;

    // Start
    txRequest.append(0x7e);

    // Length
    txRequest.append(char(0x00));
    txRequest.append(0x16);

    // Type
    txRequest.append(0x10);

    // His address = 0013a200 408dd526

    // Frame id
    txRequest.append(char(0x01));

    // Dest Addr
   txRequest.append(char(0x00));
   txRequest.append(char(0x00));
   txRequest.append(char(0x00));
   txRequest.append(char(0x00));

   txRequest.append(char(0x00));
   txRequest.append(char(0x00));
    txRequest.append(0xff);
    txRequest.append(0xff);

    // Reserved
    txRequest.append(0xff);
    txRequest.append(0xfe);

    // Broadcast Rad
   txRequest.append(char(0x00));

    // Tx Options
    txRequest.append(char(0x00));

    // Payload
    txRequest.append(0x54);
    txRequest.append(0x78);
    txRequest.append(0x44);
    txRequest.append(0x61);
    txRequest.append(0x74);
    txRequest.append(0x61);
    txRequest.append(0x30);
    txRequest.append(0x41);

    char sum = 0;
    for (int i = 3; i < txRequest.length(); i++)
        sum += txRequest.at(i);
    char checksum = 255 - sum;
    qDebug() << "checksum: " << (unsigned int)checksum;
    qDebug() << "sum: " << (uint) (sum & 0x000000ff);
    qDebug() << "Check: " << (unsigned int)((sum + checksum) & 0x000000ff);
    // Checksum
    txRequest.append(checksum);

    return txRequest;
}

QByteArray MainWindow::GetATCommand()
{
    QByteArray atCmd;

    // Start
    atCmd.append(0x7e);

    return atCmd;
}

QByteArray MainWindow::GetTxReqXBFrame()
{

    XbeeAPI::TxRequestParams options;
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
