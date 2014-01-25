#ifndef JPTMAINWINDOW_H
#define JPTMAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
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

public slots:
    void RefreshPortList();
    void BrowseWkDirSlot();
    void RefreshTestList();
    void StartTest();
    void StopTest();
    void EnableDisableStartButton();
    
private:
    Ui::JPTMainWindow *ui;

protected:
    JPTestController* jptestManager;
    QDir* workingDirectory;

    QDir GetInitialWorkingDir();    // CLS - needed to account for OS X .app files
};

#endif // JPTMAINWINDOW_H
