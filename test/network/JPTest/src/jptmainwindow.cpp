#include "jptmainwindow.h"
#include "ui_jptmainwindow.h"

JPTMainWindow::JPTMainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::JPTMainWindow)
  , mainLayout(new QHBoxLayout())
  , commandButtonLayout(new QHBoxLayout())
  , outboxTabPacketListLayout(new QVBoxLayout())
  , outboxTabOutboxLayout(new QVBoxLayout())
  , outboxTabLayout(new QHBoxLayout())
  , leftLayout(new QVBoxLayout())
  , rightLayout(new QVBoxLayout())
  , jptestManager(new JPTestController(this))
  , workingDirectory(new QDir(GetInitialWorkingDir()))
  , notificationColor(QColor(Qt::blue))
  , testOptions(JPTestOptions())
{
    qRegisterMetaType<JPTestOptions>("JPTestOptions");
    qRegisterMetaType<QList<QByteArray> >("QList<QByteArray>");

    ui->setupUi(this);

    // Set up layouts...(have to hack bc designer being ANNOYING)
    leftLayout->addWidget(ui->leftTitleLabel);
    leftLayout->addWidget(ui->toolBox);

    // commandButtonLayout
    commandButtonLayout->addWidget(ui->startTestButton);
    commandButtonLayout->addWidget(ui->stopTestButton);
    commandButtonLayout->addWidget(ui->stepTestButton);

    // outboxTabPacketListLayout
    outboxTabPacketListLayout->addWidget(ui->packetOutboxLabel);
    outboxTabPacketListLayout->addWidget(ui->packetOutboxListWidget);
    outboxTabPacketListLayout->addLayout(commandButtonLayout);

    // outboxTabOutboxLayout
    outboxTabOutboxLayout->addWidget(ui->outboxLabel);
    outboxTabOutboxLayout->addWidget(ui->outboxTextBrowser);
    outboxTabOutboxLayout->addWidget(ui->clearOutboxButton);

    // outboxTabLayout
    outboxTabLayout->addLayout(outboxTabPacketListLayout, 1);
    outboxTabLayout->addLayout(outboxTabOutboxLayout, 3);
    ui->tabWidget->widget(0)->setLayout(outboxTabLayout);

    //ui->tabWidget
    rightLayout->addWidget(ui->tabWidget);
    rightLayout->addWidget(ui->msgsLabel);
    rightLayout->addWidget(ui->messagesTextBrowser);
    rightLayout->addWidget(ui->clearMessagesButton);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 3);

    ui->centralWidget->setLayout(mainLayout);

    // Set up port settings page
    ui->portListWidget->setAutoScroll(true);    // ATTN John: this is an exception to the magic # rule...(see fct. name) ;)

    // Set up test settings page
    ui->workingDirLineEdit->setText(workingDirectory->path());
    ui->jptestServerRButton->setChecked(true);

    ui->JAGIDcomboBox->addItem("GCS");
    ui->JAGIDcomboBox->addItem("MS");
    ui->JAGIDcomboBox->addItem("QC");
    ui->JAGIDcomboBox->addItem("Broadcast");

    // ---------------------------- Connect signals/slots -------------------------------------------- //

    // Buttons
    connect(this->ui->refreshPortsButton, SIGNAL(clicked()), this, SLOT(RefreshPortList()));
    connect(this->ui->browseWkDirButton, SIGNAL(clicked()), this, SLOT(BrowseWkDirSlot()));
    connect(this->ui->startTestButton, SIGNAL(clicked()), this, SLOT(StartTest()));
    connect(this->ui->stopTestButton, SIGNAL(clicked()), this, SLOT(StopTest()));
    connect(this->ui->clearOutboxButton, SIGNAL(clicked()), ui->outboxTextBrowser, SLOT(clear()));
    connect(this->ui->clearMessagesButton, SIGNAL(clicked()), ui->messagesTextBrowser, SLOT(clear()));
    connect(this->ui->loadTestButton, SIGNAL(clicked()), this, SLOT(LoadTest()));

    // Misc
    connect(this->ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(RemoveNotification(int)));
    connect(this->ui->JAGIDcomboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(UpdateJaguarIDS(QString)));
    connect(this->ui->portListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(UpdatePortSelection(QString)));
    connect(this->ui->noMSP430checkBox, SIGNAL(released()), this, SLOT(CacheTestOptions()));

    // JPTest Controller
    connect(this->jptestManager, SIGNAL(OutboxChanged(QList<QByteArray>)), this, SLOT(UpdateTestScript(QList<QByteArray>)));
    connect(this->jptestManager, SIGNAL(P2InboxChanged(QList<QByteArray>)), this, SLOT(UpdateP2Script(QList<QByteArray>)));
    connect(this->jptestManager, SIGNAL(P3InboxChanged(QList<QByteArray>)), this, SLOT(UpdateP3Script(QList<QByteArray>)));
    connect(this->jptestManager, SIGNAL(PacketSent(QByteArray)), this, SLOT(AppendToOutbox(QByteArray)));
    connect(this->jptestManager, SIGNAL(RawByteReceived(char)), this, SLOT(AppendToRawByteInbox(char)));

    connect(this->jptestManager, SIGNAL(P2PacketReceived(QByteArray,QList<int>)),
            this, SLOT(AppendToP2Inbox(QByteArray,QList<int>)));

    connect(this->jptestManager, SIGNAL(P3PacketReceived(QByteArray,QList<int>)),
            this, SLOT(AppendToP3Inbox(QByteArray,QList<int>)));

    connect(this->jptestManager, SIGNAL(GarbagePacketReceived(QByteArray)), this, SLOT(HandleGarbage(QByteArray)));
    // -------------------------------------------------------------------------------------------------//

    // Set initial states
    ui->startTestButton->setEnabled(false);     // Disabled until test is loaded
    ui->toolBox->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);

    RefreshPortList();
    RefreshTestList();

    UpdateJaguarIDS(ui->JAGIDcomboBox->currentText());
    ShowStatusBarMessage("Ready");

    qDebug() << workingDirectory->path();
}

JPTMainWindow::~JPTMainWindow()
{
    delete ui;
    delete jptestManager;
    delete mainLayout;
    delete commandButtonLayout;
    delete outboxTabPacketListLayout;
    delete outboxTabOutboxLayout;
    delete outboxTabLayout;
    delete leftLayout;
    delete rightLayout;
}

void JPTMainWindow::RefreshPortList()
{
    this->ui->portListWidget->clear();
    this->ui->portListWidget->addItems(JPTestPort::GetAvailablePortNames());
    this->ui->portListWidget->setCurrentRow(0);
    return;
}

QDir JPTMainWindow::GetInitialWorkingDir()
{
    QDir path;
#ifdef Q_OS_MACX
    path = QDir::currentPath();
    for (int i = 0; i < 3; i++)
        path.cdUp();
    path.setCurrent(path.path());
#else
    path = QDir::currentPath();
#endif
    return path;
}

void JPTMainWindow::BrowseWkDirSlot()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Select working directory", this->workingDirectory->path());
    if (directory != "")
    {
        this->ui->workingDirLineEdit->setText(directory);
        this->workingDirectory->setPath(ui->workingDirLineEdit->text());
        this->workingDirectory->setCurrent(ui->workingDirLineEdit->text());
    }
    RefreshTestList();
    return;
}

void JPTMainWindow::RefreshTestList()
{
    QStringList nameFilters;
    nameFilters.append("*.jptest");

    this->ui->jptestListWidget->clear();
    this->ui->jptestListWidget->addItems(this->workingDirectory->entryList(nameFilters, QDir::Files, QDir::Name));

    // Don't allow start button to be pushed if no tests available!

    return;
}

void JPTMainWindow::StartTest()
{   
    ui->outboxTextBrowser->clear();
    ui->rawByteInboxTextBrowser->clear();

    if (ui->jptestServerRButton->isChecked())
    {
        if (ui->packetOutboxListWidget->count() > 0)
        {
            this->jptestManager->StartTest(ui->jptestServerRButton->isChecked());
            LogToMessageArea("Running " + testOptions.Filename + "...");
        }
        // CLS - server needs to send at least 1 msg since that signals the clients to start!
        else ShowMessageBoxMessage(GetJptestFilename(false) + " has no outgoing packets for " + testOptions.GetJagIDString());
    }
    else
    {
        // Start test as client (begin listening)
        this->jptestManager->StartTest(ui->jptestServerRButton->isChecked());
        LogToMessageArea("Client is listening for JPackets...");
    }

    return;
}

void JPTMainWindow::StopTest()
{
    this->jptestManager->StopTest();
    return;
}

void JPTMainWindow::LoadTest()
{
    CacheTestOptions();

    jptestManager->LoadTest(this->testOptions);

    ShowStatusBarMessage(GetJptestFilename(false) + " test loaded");
    ui->startTestButton->setEnabled(true);
    return;
}

void JPTMainWindow::ShowStatusBarMessage(const QString &msg)
{
    this->ui->statusBar->showMessage(msg);
    return;
}

void JPTMainWindow::RemoveNotification(int tabIndex)
{
    if (ui->tabWidget->tabBar()->tabTextColor(tabIndex) == notificationColor)
    {
        ui->tabWidget->tabBar()->setTabTextColor(tabIndex, QColor(Qt::black));
    }
    return;
}

void JPTMainWindow::UpdateJaguarIDS(QString JAGID)
{
    QString P2;
    QString P3;
    if (JAGID == "GCS")
    {
        this->testOptions.JaguarID = JAGPACKET::GCS;
        this->testOptions.P2ID = JAGPACKET::MS;
        this->testOptions.P3ID = JAGPACKET::QC;
        P2 = "MS";
        P3 = "QC";
    }
    else if (JAGID == "MS")
    {
        this->testOptions.JaguarID = JAGPACKET::MS;
        this->testOptions.P2ID = JAGPACKET::QC;
        this->testOptions.P3ID = JAGPACKET::GCS;
        P2 = "QC";
        P3 = "GCS";
    }
    else if (JAGID == "QC")
    {
        this->testOptions.JaguarID = JAGPACKET::QC;
        this->testOptions.P2ID = JAGPACKET::GCS;
        this->testOptions.P3ID = JAGPACKET::MS;
        P2 = "GCS";
        P3 = "MS";
    }
    else if (JAGID == "Broadcast")
    {
        this->testOptions.JaguarID = JAGPACKET::BROADCAST;
        this->testOptions.P2ID = JAGPACKET::GCS;
        this->testOptions.P3ID = JAGPACKET::QC;
        P2 = "GCS";     // CLS - picking random 2 for now...
        P3 = "QC";
    }
    else ui->messagesTextBrowser->append("Invalid JAGUAR ID selection!");

    UpdateJAGIDStrings(JAGID, P2, P3);

    // Reload test for new JAGUAR ID if already loaded from a selected file
    if (ui->startTestButton->isEnabled()) LoadTest();

    LogToMessageArea("Configured as " + testOptions.GetJagIDString());

    return;
}

void JPTMainWindow::UpdateTestScript(QList<QByteArray> newScript)
{
    this->ui->packetOutboxListWidget->clear();
    for (int i = 0; i < newScript.count(); i++)
        ui->packetOutboxListWidget->addItem(newScript.at(i));
    return;
}

void JPTMainWindow::UpdateP2Script(QList<QByteArray> newP2Script)
{
    ui->p2packetInboxListWidget->clear();
    for (int i = 0; i < newP2Script.count(); i++)
        ui->p2packetInboxListWidget->addItem(newP2Script.at(i));
    return;
}

void JPTMainWindow::UpdateP3Script(QList<QByteArray> newP3Script)
{
    ui->p3packetInboxListWidget->clear();
    for (int i = 0; i < newP3Script.count(); i++)
        ui->p3packetInboxListWidget->addItem(newP3Script.at(i));
    return;
}

/**
 * @brief JPTMainWindow::UpdatePortSelection
 * @param UnusedPortVar is simply included to match the signal payload type of the list widget
 */
void JPTMainWindow::UpdatePortSelection(const QString& UnusedPortVar)
{
    if (ui->startTestButton->isEnabled())
    {
        qDebug() << "Reloading test bc of port change...";
        LoadTest();
    }
    return;
}

QString JPTMainWindow::GetJptestFilename(bool IncludeWorkingDir /* = true*/)
{
    if (ui->jptestListWidget->selectedItems().empty()) return "JPTMainWindow_NOFILESELECTED";

    if (IncludeWorkingDir)
        return this->workingDirectory->path() + "/" + ui->jptestListWidget->selectedItems().at(0)->text();
    else
        return ui->jptestListWidget->selectedItems().at(0)->text();
}

void JPTMainWindow::AppendToOutbox(QByteArray packet)
{
    QByteArray hexPacket = packet.toHex();
    int totalBytes = hexPacket.count() / 2;     // hexPacket.count() == # nibbles (bc of encoding hex to ascii chars)
    int byteCount = 0;
    QString htmlString;
    for (int i = 0; i < hexPacket.count(); i++)
    {
        QString hexValue(hexPacket.at(i));      // Get first nibble
        hexValue.append(hexPacket.at(i+1));     // Get second nibble

        // Format output to highlight protocols
        if (byteCount < MAVPACKET::JHDR_OFFSET)
            // Format JAGUAR header
            htmlString = GetHtmlString(hexValue.toUpper(), JAG_COLOR);
        else if (byteCount < (MAVPACKET::JHDR_OFFSET + MAVPACKET::MVHDR_OFFSET))
            // Forma Mavelink header
            htmlString = GetHtmlString(hexValue.toUpper(), MAV_COLOR);
        else
        {
            // Format payload and checksum
            if (byteCount < (totalBytes - MAVPACKET::tail_CS_OFFSET))
                htmlString = GetHtmlString(hexValue.toUpper(), PAYLOAD_COLOR);
            else
                htmlString = GetHtmlString(hexValue.toUpper(), MAV_COLOR);
        }

        ui->outboxTextBrowser->insertHtml(htmlString);
        ui->outboxTextBrowser->insertPlainText(" ");
        i++;        // Account for grabbing first and second nibble...
        byteCount++;
    }
    ui->outboxTextBrowser->insertPlainText("\n");

    if (ui->tabWidget->currentIndex() != 1)
        PostNotification(1);  //Notify

    return;
}


void JPTMainWindow::AppendToRawByteInbox(char newByte)
{
    int newByteASCIIValue(newByte);
    QString hexByte = QString::number(newByteASCIIValue, 16);
    if (hexByte.length() < 2) hexByte.prepend('0');
    ui->rawByteInboxTextBrowser->insertPlainText(hexByte.toUpper() + " ");
    return;
}

void JPTMainWindow::AppendToP2Inbox(QByteArray packet, QList<int> diffs)
{
    int asciiByte;
    QString hexByte;
    QString colorString;

    for (int i = 0; i < packet.count(); i++)
    {
        // Format into hex value string
        asciiByte = (unsigned char) packet.at(i);
        hexByte = QString::number(asciiByte, 16);

        // Check pass/fail
        if (!diffs.empty() && diffs.first() == i)
            colorString = "red";
        else
            colorString = "green";

        ui->p2Inbox->insertHtml(GetHtmlString(hexByte.toUpper(), colorString));
    }

    ui->p2Inbox->insertPlainText("\n");

    return;
}

void JPTMainWindow::AppendToP3Inbox(QByteArray packet, QList<int> diffs)
{
    int asciiByte;
    QString hexByte;
    QString colorString;

    for (int i = 0; i < packet.count(); i++)
    {
        // Format into hex value string
        asciiByte = (unsigned char) packet.at(i);
        hexByte = QString::number(asciiByte, 16);

        // Check pass/fail
        if (!diffs.empty() && diffs.first() == i)
            colorString = "red";
        else
            colorString = "green";

        ui->p3Inbox->insertHtml(GetHtmlString(hexByte.toUpper(), colorString));
    }

    ui->p3Inbox->insertPlainText("\n");

    return;
}

void JPTMainWindow::HandleGarbage(QByteArray garbagePacket)
{
    QByteArray hex = garbagePacket.toHex();
    QString outputString;

    for (int i = 0; i < hex.count(); i++)
    {
        QString hexValue(hex.at(i));
        hexValue.append(hex.at(i));
        outputString.append(hexValue.toUpper() + " ");
        i++;    // Account for second nibble
    }

    LogToMessageArea("Garbage packet detected: " + outputString);

    return;
}

QString JPTMainWindow::GetJPacketPath()
{
    return this->workingDirectory->path() + "/jpackets/";
}

QString JPTMainWindow::GetPortName()
{
    return this->ui->portListWidget->selectedItems().at(0)->text();
}

void JPTMainWindow::CacheTestOptions()
{
    testOptions.RunMode = RUN;
    testOptions.Filename = GetJptestFilename();
    testOptions.JPacketPath = GetJPacketPath();
    testOptions.PortName = GetPortName();
    testOptions.MSP430ModeOn = (!ui->noMSP430checkBox->isChecked());

    // CLS - all fields listed here so we can verify that we're handling them

    //testOptions.DelaySecs

    //testOptions.JaguarID      // Set at startup, and updated on every input change
    //testOptions.P2ID          // Set at startup, and updated on every input change
    //testOptions.P3ID          // Set at startup, and updated on every input change

    //testOptions.NumLoops

    return;
}

void JPTMainWindow::ShowMessageBoxMessage(const QString &msg)
{
    QMessageBox mb;
    mb.setText(msg);
    mb.exec();
    return;
}

void JPTMainWindow::LogToMessageArea(const QString &msg)
{
    ui->messagesTextBrowser->append(msg);
    return;
}

QString JPTMainWindow::GetHtmlString(const QString &text, const QString &color)
{
    return "<font color=\"" + color + "\">" + text + "</font>";
}

void JPTMainWindow::UpdateJAGIDStrings(const QString& myName, const QString &p2Name, const QString &p3Name)
{
    // Set myID strings
    this->setWindowTitle("JPTest - " + myName);
    ui->packetOutboxLabel->setText(myName + " Packet Outbox");

    // Set p2ID strings
    ui->p2InboxLabel->setText(p2Name + " Inbox:");
    ui->p2packetInboxLabel->setText(p2Name + " packets:");

    // Set p3ID strings
    ui->p3InboxLabel->setText(p3Name + " Inbox:");
    ui->p3packetInboxLabel->setText(p3Name + " packets:");

    return;
}

void JPTMainWindow::PostNotification(const int &tabIndex)
{
    ui->tabWidget->tabBar()->setTabTextColor(tabIndex, notificationColor);
    return;
}
