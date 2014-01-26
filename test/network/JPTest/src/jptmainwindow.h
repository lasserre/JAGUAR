#ifndef JPTMAINWINDOW_H
#define JPTMAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
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
    void ShowStatusMessage(const QString& msg);

public slots:
    void RefreshPortList();
    void BrowseWkDirSlot();
    void RefreshTestList();
    void StartTest();
    void StopTest();
    void EnableDisableStartButton();
    void RemoveNotification(int tabIndex);
    
private:
    Ui::JPTMainWindow *ui;
    QHBoxLayout* mainLayout;
    QHBoxLayout* upperRightLayout;
    QVBoxLayout* leftLayout;
    QVBoxLayout* rightLayout;

protected:
    JPTestController* jptestManager;
    QDir* workingDirectory;
    QColor notificationColor;

    // Methods
    QDir GetInitialWorkingDir();    // CLS - needed to account for OS X .app files
};

#endif // JPTMAINWINDOW_H
