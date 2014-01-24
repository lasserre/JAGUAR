#ifndef JPTMAINWINDOW_H
#define JPTMAINWINDOW_H

#include <QMainWindow>
#include "jptestcontroller.h"

namespace Ui {
class JPTMainWindow;
}

class JPTMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit JPTMainWindow(QWidget *parent = 0);
    ~JPTMainWindow();

protected:
    void RefreshPortList();
    
private:
    Ui::JPTMainWindow *ui;
    JPTestController* jptestManager;
};

#endif // JPTMAINWINDOW_H
