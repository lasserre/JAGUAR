#include "jptmainwindow.h"
#include "ui_jptmainwindow.h"

JPTMainWindow::JPTMainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::JPTMainWindow)
    , jptestManager(new JPTestController(this))
  , workingDirectory(new QDir(GetInitialWorkingDir()))
{
    qRegisterMetaType<JPTestOptions>("JPTestOptions");
    ui->setupUi(this);

    // Set up port settings page
    ui->portListWidget->setAutoScroll(true);    // ATTN John: this is an exception to the magic # rule...(see fct. name) ;)

    // Set up test settings page
    ui->workingDirLineEdit->setText(workingDirectory->path());

    connect(this->ui->refreshPortsButton, SIGNAL(clicked()), this, SLOT(RefreshPortList()));
    connect(this->ui->browseWkDirButton, SIGNAL(clicked()), this, SLOT(BrowseWkDirSlot()));
    connect(this->ui->startTestButton, SIGNAL(clicked()), this, SLOT(StartTest()));
    connect(this->ui->stopTestButton, SIGNAL(clicked()), this, SLOT(StopTest()));
    connect(this->ui->jptestListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(EnableDisableStartButton()));

    RefreshPortList();
    RefreshTestList();

    qDebug() << workingDirectory->path();
}

JPTMainWindow::~JPTMainWindow()
{
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
    qDebug() << this->workingDirectory->path();
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
    qDebug() << "currentPath " << this->workingDirectory->currentPath();
    qDebug() << "path " << this->workingDirectory->path();

    QStringList nameFilters;
    nameFilters.append("*.jptest");

    this->ui->jptestListWidget->clear();
    this->ui->jptestListWidget->addItems(this->workingDirectory->entryList(nameFilters, QDir::Files, QDir::Name));

    // Don't allow start button to be pushed if no tests available!
    EnableDisableStartButton();
    return;
}

void JPTMainWindow::StartTest()
{
    JPTestOptions options;
    options.RunMode = RUN;
    // CLS - TODO: Change below line to support multiple tests...
    options.Filename = this->workingDirectory->path() + "/" + ui->jptestListWidget->selectedItems().at(0)->text();
    options.JPacketPath = this->workingDirectory->path() + "/jpackets/";
    options.PortName = this->ui->portListWidget->selectedItems().at(0)->text();     // Only one port may be selected!
    this->jptestManager->StartTest(options);
    return;
}

void JPTMainWindow::StopTest()
{
    this->jptestManager->StopTest();
    return;
}

void JPTMainWindow::EnableDisableStartButton()
{
    bool enableStartButton = false;
    if (ui->jptestListWidget->count() > 0)
    {
        if (ui->jptestListWidget->selectedItems().count() > 0) enableStartButton = true;
    }
    ui->startTestButton->setEnabled(enableStartButton);
    return;
}
