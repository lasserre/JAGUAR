#ifndef JPTMAINWINDOW_H
#define JPTMAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include "jptestcontroller.h"

const QString JAG_COLOR = "blue";
const QString MAV_COLOR = "green";
const QString PAYLOAD_COLOR = "black";

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
    //void EnableDisableStartButton();
    void LoadTest();
    void RemoveNotification(int tabIndex);
    void UpdateJaguarID(QString JAGID);
    void UpdateTestScript(QList<QByteArray> newScript);
    void UpdatePortSelection(const QString &UnusedPortVar);
    void AppendToOutbox(QByteArray);
    
private:
    Ui::JPTMainWindow *ui;
    QHBoxLayout* mainLayout;
    QHBoxLayout* commandButtonLayout;
    QVBoxLayout* outboxTabPacketListLayout;
    QVBoxLayout* outboxTabOutboxLayout;
    QHBoxLayout* outboxTabLayout;
    QVBoxLayout* leftLayout;
    QVBoxLayout* rightLayout;

protected:
    JPTestController* jptestManager;
    QDir* workingDirectory;
    QColor notificationColor;
    JPTestOptions testOptions;

    // Methods
    QDir GetInitialWorkingDir();    // CLS - needed to account for OS X .app files
    QString GetJptestFilename(bool IncludeWorkingDir = true);
    QString GetJPacketPath();
    QString GetPortName();
    void CacheTestOptions();
    void ShowStatusBarMessage(const QString& msg);
    void ShowMessageBoxMessage(const QString& msg);
    void LogToMessageArea(const QString& msg);
    QString GetHtmlString(const QString& text, const QString& color);
    void UpdateP2P3(const QString& p2Name, const QString& p3Name);
    void PostNotification(const int& tabIndex);
};

#endif // JPTMAINWINDOW_H
