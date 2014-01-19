#include "jptmainwindow.h"
#include "ui_jptmainwindow.h"

JPTMainWindow::JPTMainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::JPTMainWindow)
    , port(new JPTestPort(this))
{
    ui->setupUi(this);
    foreach (const QString& name, port->GetAvailablePortNames())
    {
        qDebug() << "after GetAvailablePortNames()... " + name;
    }

    port->OpenPort();   // See what happens when port won't open...
}

JPTMainWindow::~JPTMainWindow()
{
    delete port;
    delete ui;
}
