#include "jptmainwindow.h"
#include "ui_jptmainwindow.h"

JPTMainWindow::JPTMainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::JPTMainWindow)
  , mainLayout(new QHBoxLayout())
  , outboxTabPacketListLayout(new QVBoxLayout())
  , outboxTabOutboxLayout(new QVBoxLayout())
  , outboxTabLayout(new QHBoxLayout())
  , leftLayout(new QVBoxLayout())
  , rightLayout(new QVBoxLayout())
  , jptestManager(new JPTestController(this))
  , workingDirectory(new QDir(GetInitialWorkingDir()))
  , notificationColor(QColor(Qt::blue))
  , testOptions(JPTestOptions())
  , startAction(new QAction(QIcon(":/images/images/play.png"), "Start", this))
  , stopAction(new QAction(QIcon(":/images/images/stop.png"), "Stop", this))
  , stepAction(new QAction(QIcon(":/images/images/step.png"), "Step", this))
  , loadAction(new QAction(QIcon(":/images/images/load.png"), "Load", this))
  , outgoingPacketIndex(-1)
  , packetBytesSent(0)
  , p2InboxPassFailIndex(-1)
  , p3InboxPassFailIndex(-1)
  , running(false)
{
    qRegisterMetaType<JPTestOptions>("JPTestOptions");
    qRegisterMetaType<QList<QByteArray> >("QList<QByteArray>");
    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaType<JPacketDiffResults>("JPacketDiffResults");

    ui->setupUi(this);

    // Icons
    const int iconWidth = 80;
    const int iconHeight = 30;

    ui->mainToolBar->setIconSize(QSize(iconWidth, iconHeight));
    ui->mainToolBar->addAction(startAction);
    ui->mainToolBar->addAction(stopAction);
    ui->mainToolBar->addAction(stepAction);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(loadAction);
    addToolBar(Qt::LeftToolBarArea, ui->mainToolBar);

    // Set up layouts...(have to hack bc designer being ANNOYING)
    leftLayout->addWidget(ui->leftTitleLabel);
    leftLayout->addWidget(ui->toolBox);

    // outboxTabPacketListLayout
    outboxTabPacketListLayout->addWidget(ui->packetOutboxLabel);
    outboxTabPacketListLayout->addWidget(ui->packetOutboxListWidget);

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
    ui->portListWidget->setAutoScroll(true);    // ATTN John: this is an exception to the magic # rule...
                                                // (see fct. name) ;)
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
    connect(this->startAction, SIGNAL(triggered()), this, SLOT(StartTest()));
    connect(this->stopAction, SIGNAL(triggered()), this, SLOT(StopTest()));
    connect(this->stepAction, SIGNAL(triggered()), this, SLOT(StepTest()));
    connect(this->ui->clearOutboxButton, SIGNAL(clicked()), ui->outboxTextBrowser, SLOT(clear()));
    connect(this->ui->clearMessagesButton, SIGNAL(clicked()), ui->messagesTextBrowser, SLOT(clear()));
    connect(this->loadAction, SIGNAL(triggered()), this, SLOT(LoadTest()));

    // Misc
    connect(this->ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(RemoveNotification(int)));
    connect(this->ui->JAGIDcomboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(UpdateJaguarIDS(QString)));
    connect(this->ui->portListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(UpdatePortSelection(QString)));
    connect(this->ui->noMSP430checkBox, SIGNAL(released()), this, SLOT(CacheTestOptions()));
    connect(this->ui->stepModeCheckBox, SIGNAL(released()), this, SLOT(CacheTestOptions()));

    // JPTest Controller
    connect(this->jptestManager, SIGNAL(UnableToStartTest()), SLOT(HandleUnableToStartTest()));
    connect(this->jptestManager, SIGNAL(TestEnded()), this, SLOT(HandleTestEnded()));
    connect(this->jptestManager, SIGNAL(OutboxChanged(QStringList)), this, SLOT(UpdateTestScript(QStringList)));
    connect(this->jptestManager, SIGNAL(P2InboxChanged(QStringList)), this, SLOT(UpdateP2Script(QStringList)));
    connect(this->jptestManager, SIGNAL(P3InboxChanged(QStringList)), this, SLOT(UpdateP3Script(QStringList)));
    connect(this->jptestManager, SIGNAL(DataSent(QByteArray,bool,int)), this, SLOT(AppendToOutbox(QByteArray, bool,int)));
    connect(this->jptestManager, SIGNAL(NewDiffResults(JPacketDiffResults)),
            this, SLOT(ProcessDiffResults(JPacketDiffResults)));
    // -------------------------------------------------------------------------------------------------//

    // Set initial states
    startAction->setEnabled(false);     // Disabled until test is loaded
    stepAction->setEnabled(false);      // Disabled until test is started (and in step mode)
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
    delete outboxTabPacketListLayout;
    delete outboxTabOutboxLayout;
    delete outboxTabLayout;
    delete leftLayout;
    delete rightLayout;
    delete mainLayout;
    delete ui;
    delete jptestManager;
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
    ui->garbageInbox->clear();
    outgoingPacketIndex = 0;
    packetBytesSent = 0;
    p2InboxPassFailIndex = 0;
    p3InboxPassFailIndex = 0;

    if (ui->jptestServerRButton->isChecked())
    {
        /* ----- Start Server ----- */

        if (ui->packetOutboxListWidget->count() > 0)
        {
            this->jptestManager->StartTest(ui->jptestServerRButton->isChecked());

            LogToMessageArea("Running " + testOptions.Filename + " in " + testOptions.GetRunModeString());
            running = true;
            stepAction->setEnabled(true);
        }
        // CLS - server needs to send at least 1 msg since that signals the clients to start!
        else ShowMessageBoxMessage(GetJptestFilename(false) + " has no outgoing packets for " + testOptions.GetJagIDString());
    }
    else
    {
        /* ----- Start Client ----- */

        this->jptestManager->StartTest(ui->jptestServerRButton->isChecked());
        LogToMessageArea("Client is listening for JPackets...");
        running = true;
    }

    return;
}

void JPTMainWindow::StopTest()
{
    QString role = "server";

    this->jptestManager->StopTest();

    if (ui->jptestClientRButton->isChecked())
        role = "client";

    LogToMessageArea("Stopping " + role + " test");
    return;
}

void JPTMainWindow::StepTest()
{
    if (!running)
        LogToMessageArea("Click the start test button before you can step.");
    else
        jptestManager->StepTest();

    return;
}

void JPTMainWindow::LoadTest()
{
    CacheTestOptions();

    jptestManager->LoadTest(this->testOptions);

    ShowStatusBarMessage(GetJptestFilename(false) + " test loaded");
    startAction->setEnabled(true);
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
    if (startAction->isEnabled()) LoadTest();

    LogToMessageArea("Configured as " + testOptions.GetJagIDString() + " (" + testOptions.JaguarID + ")");

    return;
}

void JPTMainWindow::UpdateTestScript(QStringList newScript)
{
    this->ui->packetOutboxListWidget->clear();
    for (int i = 0; i < newScript.count(); i++)
        ui->packetOutboxListWidget->addItem(newScript.at(i));
    return;
}

void JPTMainWindow::UpdateP2Script(QStringList newP2Script)
{
    ui->p2packetInboxListWidget->clear();
    for (int i = 0; i < newP2Script.count(); i++)
        ui->p2packetInboxListWidget->addItem(newP2Script.at(i));
    return;
}

void JPTMainWindow::UpdateP3Script(QStringList newP3Script)
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
    if (startAction->isEnabled())
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

void JPTMainWindow::AppendToOutbox(QByteArray packet, bool newPacketStart, int packetLength)
{
    QString htmlString;

    if (newPacketStart && packetBytesSent != 0)
    {
        // Start handling next packet (after first has been sent)
        ui->outboxTextBrowser->insertPlainText("\n");

        // If last packet wasn't fully sent, indicate with yellow highlight
        if (ui->packetOutboxListWidget->item(outgoingPacketIndex)->backgroundColor() != Qt::green)
            ui->packetOutboxListWidget->item(outgoingPacketIndex)->setBackgroundColor(Qt::darkYellow);
        outgoingPacketIndex++;

        packetBytesSent = 0;
    }

    QByteArray hexPacket = packet.toHex();     // hexPacket.count() == # nibbles (bc of encoding hex to ascii chars)
    int byteCount = 0;

    for (int i = 0; i < hexPacket.count(); i++)
    {
        QString hexValue(hexPacket.at(i));      // Get first nibble
        hexValue.append(hexPacket.at(i+1));     // Get second nibble

        // Format output to highlight protocols
        if ((byteCount + packetBytesSent) < MAVPACKET::JHDR_OFFSET)
            // Format JAGUAR header
            htmlString = GetHtmlString(hexValue.toUpper(), JAG_COLOR);
        else if ((byteCount + packetBytesSent) < (MAVPACKET::JHDR_OFFSET + MAVPACKET::MVHDR_OFFSET))
            // Forma MAVELINK header
            htmlString = GetHtmlString(hexValue.toUpper(), MAV_COLOR);
        else
        {
            // Format payload and checksum
            if ((byteCount + packetBytesSent) < (packetLength - MAVPACKET::tail_CS_OFFSET))
                htmlString = GetHtmlString(hexValue.toUpper(), PAYLOAD_COLOR);
            else
                htmlString = GetHtmlString(hexValue.toUpper(), MAV_COLOR);
        }

        ui->outboxTextBrowser->insertHtml(htmlString);
        ui->outboxTextBrowser->insertPlainText(" ");
        i++;        // Account for grabbing first and second nibble...
        byteCount++;
    }

    // Increment # packet bytes sent by the number of bytes we just now sent out
    packetBytesSent += byteCount;

    // Set success color if we have sent out all the bytes
    if (packetBytesSent == packetLength)
        ui->packetOutboxListWidget->item(outgoingPacketIndex)->setBackgroundColor(Qt::green);

    // Notify if not on outbox tab
    if (ui->tabWidget->currentIndex() != 0)
        PostNotification(0);

    return;
}


void JPTMainWindow::AppendToStagingArea(char newByte)
{
    int newByteASCIIValue(newByte);

    QString hexByte = QString::number(newByteASCIIValue, 16);
    if (hexByte.length() < 2) hexByte.prepend('0');

    ui->byteStagingLineEdit->setText(ui->byteStagingLineEdit->text() + hexByte.toUpper() + " ");

    return;
}

void JPTMainWindow::AppendToP2Inbox(const char& byte, const bool& pass)
{
    QString hex = FormatByteToHexString(byte);
    QString colorString;

    if (pass)
        colorString = "green";
    else
    {
        colorString = "red";
        ui->p2packetInboxListWidget->item(p2InboxPassFailIndex)->setBackgroundColor(Qt::red);
    }

    ui->p2Inbox->insertHtml(GetHtmlString(hex.append(" "), colorString));

    return;
}

void JPTMainWindow::AppendToP3Inbox(const char& byte, const bool& pass)
{
    QString hex = FormatByteToHexString(byte);
    QString colorString;

    if (pass)
        colorString = "green";
    else
    {
        colorString = "red";
        ui->p3packetInboxListWidget->item(p3InboxPassFailIndex)->setBackgroundColor(Qt::red);
    }

    ui->p3Inbox->insertHtml(GetHtmlString(hex.append(" "), colorString));

    return;
}

void JPTMainWindow::StartNewP2Packet(QByteArray packetStart, QList<bool> passList)
{
    // Innocent until proven guilty
    ui->p2packetInboxListWidget->item(++p2InboxPassFailIndex)->setBackgroundColor(Qt::green);

    //if (!ui->p2Inbox->textCursor().atStart())
      //  ui->p2Inbox->insertPlainText("\n");
    ui->p2Inbox->append("");

    if (packetStart.length() != passList.length())
    {
        LogToMessageArea("ERROR in StartNewP2Packet: packet length does not match diff length!");
        return;
    }

    for (int i = 0; i < packetStart.length(); i++)
        AppendToP2Inbox(packetStart.at(i), passList.at(i));

    return;
}

void JPTMainWindow::StartNewP3Packet(QByteArray packetStart, QList<bool> passList)
{
    // Innocent until proven guilty
    ui->p3packetInboxListWidget->item(++p3InboxPassFailIndex)->setBackgroundColor(Qt::green);

    //if (!ui->p3Inbox->textCursor().atStart())
        ui->p3Inbox->insertPlainText("\n");

    if (packetStart.length() != passList.length())
    {
        LogToMessageArea("ERROR in StartNewP2Packet: packet length does not match diff length!");
        return;
    }

    for (int i = 0; i < packetStart.length(); i++)
        AppendToP3Inbox(packetStart.at(i), passList.at(i));

    return;
}

QString JPTMainWindow::FormatByteToHexString(const unsigned char &byte)
{
    // Format into hex value string
    QString hexByte = QString::number(byte, 16);
    if (hexByte.length() < 2) hexByte.prepend("0");
    return hexByte.toUpper();
}

void JPTMainWindow::AppendToGarbageArea(const char& garbageByte)
{
    QString hex = FormatByteToHexString(garbageByte);
    ui->garbageInbox->insertPlainText(hex + " ");

    LogToMessageArea("Garbage detected");

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
    if (ui->stepModeCheckBox->isChecked())
        testOptions.RunMode = STEP;
    else
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

void JPTMainWindow::ProcessDiffResults(JPacketDiffResults results)
{
    if (results.garbageDetected || results.packetDetected)  // New packet or garbage just detected. Clear staging area
        ui->byteStagingLineEdit->clear();

    if (results.packetDetected)
    {
        QByteArray startOfNewPacket;
        QList<bool> passList;

        for (int i = 0; i < results.diffs.count(); i++)
        {
            startOfNewPacket.append(results.diffs[i].byte);
            passList << results.diffs[i].pass;
        }

        if (results.diffs[0].srcID == testOptions.P2ID)
            StartNewP2Packet(startOfNewPacket, passList);
        else if (results.diffs[0].srcID == testOptions.P3ID)
            StartNewP3Packet(startOfNewPacket, passList);
        else
            LogToMessageArea("ERROR: New packet detected, but src doesn't match P2 or P3!");

        return;
    }

    QList<JPTDiff>::iterator listIter = results.diffs.begin();

    while (listIter != results.diffs.end())
    {
        switch (listIter->srcID)
        {
        case JAGID::Unknown:
            AppendToStagingArea(listIter->byte);
            break;
        case JAGID::GCS:
        case JAGID::MS:
        case JAGID::QC:
            if (listIter->srcID == testOptions.P2ID)
                AppendToP2Inbox(listIter->byte, listIter->pass);
            else if (listIter->srcID == testOptions.P3ID)
                AppendToP3Inbox(listIter->byte, listIter->pass);
            else
                LogToMessageArea("Valid SRC ID doesn't match P2 or P3 ids!");
            break;
        case JAGID::Garbage:
            AppendToGarbageArea(listIter->byte);
            break;
        case JAGID::Broadcast:
            break;
        default:
            LogToMessageArea("Unrecognized srcID in ProcessDiffResults");
            break;
        }

        listIter++;
    }

    return;
}

void JPTMainWindow::HandleTestEnded()
{
    LogToMessageArea("JTest Ended.");
    running = false;
    stepAction->setEnabled(false);
    return;
}

void JPTMainWindow::HandleUnableToStartTest()
{
    LogToMessageArea("Unable to start test. (Did you load the test?)");
    running = false;
    stepAction->setEnabled(false);
    return;
}
