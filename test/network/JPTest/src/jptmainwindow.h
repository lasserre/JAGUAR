#ifndef JPTMAINWINDOW_H
#define JPTMAINWINDOW_H

#include <QMainWindow>
#include "jptestport.h"

namespace Ui {
class JPTMainWindow;
}

class JPTMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit JPTMainWindow(QWidget *parent = 0);
    ~JPTMainWindow();
    
private:
    Ui::JPTMainWindow *ui;
    JPTestPort* port;
};

#endif // JPTMAINWINDOW_H
