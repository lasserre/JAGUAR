#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort>
#include <QDebug>

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
    connect(ui->clearMsgsButton, SIGNAL(clicked()), ui->messageDisplay, SLOT(clear()));
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
            LogMessage("Connected to: " + port->portName());
        }
        else
            LogMessage("Unable to connect to " + ui->listWidget->selectedItems().at(0)->text());
    }
    else
        LogMessage("Unable to connect: No items selected!");
    return;
}

void MainWindow::SendFrame()
{
    QByteArray data = GetTxReqXBFrame();

    LogMessage("ATTEMPTING TO SEND:");
    DebugQByteArray(data);

    if (port->write(data) > 0)
    {
        LogMessage("Sent " + QString::number(data.count()) + " bytes. Data: ");
        DebugQByteArray(data);
    }
    else
        LogMessage("Unable to send data");
    return;
}

void MainWindow::ReceiveFrame()
{
    QByteArray ReceivedBytes = port->readAll();

    frameBuffer.append(ReceivedBytes);    // Add received bytes to buffer

    LogMessage("\nReceived data");
    DebugQByteArray(ReceivedBytes);

    XBFrame::FrameByteArray frame;
    frame.FrameStart = (uint8_t*)frameBuffer.data();        // Point FrameStart to start of buffer
    frame.SetFrameSize(frameBuffer.size());                 // Set Frame size appropriately

    XBFrame::Type type;
    XBFrame::ParseResult res = FrameParser::ParseFrameType(frame, type);

    XBFrame::ParseResCategory resCategory = FrameParser::GetParseResultCategory(res);

    if (resCategory == XBFrame::PRC_Success)
    {
        switch (type)
        {
        case XBFrame::TransmitStatus:
            HandleTxStatus(&frame);
            break;

        case XBFrame::ReceivePacket:
            HandleRxPacket(&frame);
            break;

        default:
            LogMessage("Unhandled frame type of: " + QString::number(type));
        }
    }
    else
    {
        QString resString = QString::fromStdString(XbeeAPI::FrameParser::GetParseResultString(res));
        LogMessage("ParseFrameType res = " + resString);
    }

    return;
}

QByteArray MainWindow::GetTxReqXBFrame()
{

    // Construct payload ------
    XbeeAPI::TxRequest options;
    options.DestAddress = BROADCAST_ADDRESS;
    options.PayloadLength = 10;
    options.PayloadStart = new uint8_t[options.PayloadLength];
    strcpy((char*)options.PayloadStart, "HELLOWORLD");

    // Construct frame ---------
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
    QString out = "||---------------->\n";

    for (int i = 0; i < 2*Array.count(); i++)
    {
      out += Array.toHex()[i];
      out += Array.toHex()[++i];
      out += " ";
    }

    out += "\n<----------------||";

    LogMessage(out);

    return;
}

void MainWindow::LogMessage(const QString &Message)
{
    ui->messageDisplay->append(Message);
    return;
}

void MainWindow::HandleTxStatus(XBFrame::FrameByteArray *TxStatusFrame)
{
    LogMessage("TransmitStatus frame received!");

    TxStatus status;
    XBFrame::ParseResult res = FrameParser::ParseTxStatus(*TxStatusFrame, status);
    XBFrame::ParseResCategory resCategory = FrameParser::GetParseResultCategory(res);

    switch (resCategory)
    {
    case XBFrame::PRC_Success:

        LogMessage("TxRetryCount: " + QString::number(status.TxRetryCount));
        LogMessage("DelivStatus: " + QString::fromStdString(status.GetDelivStatString()));
        LogMessage("DiscvStatus: " + QString::fromStdString(status.GetDiscvStatString()));

        if (res == XBFrame::PR_ParseSuccess)
            frameBuffer.clear();
        else
            frameBuffer.remove(0, TxStatusFrame->FrameSize());   // We might have extra bytes at the end!
        break;

    default:
        QString resString = QString::fromStdString(XbeeAPI::FrameParser::GetParseResultString(res));
        LogMessage("ParseTxStatus ERROR: " + resString);
    }
}

void MainWindow::HandleRxPacket(XBFrame::FrameByteArray *RxPacketFrame)
{
    LogMessage("ReceivePacket frame received!");

    RxPacket packet;

    XBFrame::ParseResult res = FrameParser::ParseRxPacket(*RxPacketFrame, packet);
    XBFrame::ParseResCategory resCategory = FrameParser::GetParseResultCategory(res);

    switch (resCategory)
    {
    case XBFrame::PRC_Success:

        LogMessage("RxPacketFrame Frame Size: " + QString::number(RxPacketFrame->FrameSize()));
        LogMessage("Actual Frame Size: " + QString::number(packet.ActualFrameSize));
        LogMessage("Expected Payload Length: " + QString::number(packet.ExpctdPayloadLen));
        LogMessage("Source address: " + QString::number(packet.SourceAddress));
        LogMessage("Payload: " + QByteArray((char*)packet.PayloadStart, packet.ExpctdPayloadLen));
        LogMessage("Receive Options: " + QString::fromStdString(packet.GetRcvOptsString()));

        if (res == XBFrame::PR_ParseSuccess)
            frameBuffer.clear();
        else
            frameBuffer.remove(0, RxPacketFrame->FrameSize());

        break;

    default:
        QString resString = QString::fromStdString(FrameParser::GetParseResultString(res));
        LogMessage("ParseRxPacket ERROR: " + resString);
    }
}
