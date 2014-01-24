#include "jptmainwindow.h"
#include "ui_jptmainwindow.h"

JPTMainWindow::JPTMainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::JPTMainWindow)
    , jptestManager(new JPTestController(this))
{
    ui->setupUi(this);

    // Set up configure ports page
    ui->portListWidget->setAutoScroll(true);    // ATTN John: this is an exception to the magic # rule...(see fct. name) ;)
    ui->configurePortsButton->setEnabled(false);
    ui->testSettingsPage->setEnabled(false);  // Extend later...

    RefreshPortList();
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
