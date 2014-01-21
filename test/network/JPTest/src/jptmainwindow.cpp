#include "jptmainwindow.h"
#include "ui_jptmainwindow.h"

JPTMainWindow::JPTMainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::JPTMainWindow)
{
    ui->setupUi(this);

    QByteArray testBytes;
    testBytes.append("HELLO JOHN!!!");
}

JPTMainWindow::~JPTMainWindow()
{
    delete ui;
}
