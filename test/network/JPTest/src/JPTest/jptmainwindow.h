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
    void StepTest();
    void LoadTest();

    void RemoveNotification(int tabIndex);
    void UpdateJaguarIDS(QString JAGID);
    void UpdateTestScript(QStringList newScript);
    void UpdateP2Script(QStringList newP2Script);
    void UpdateP3Script(QStringList newP3Script);
    void UpdatePortSelection(const QString &UnusedPortVar);
    void HandleTestListSelectionChanged();

    void AppendToOutbox(QByteArray data, bool newPacketStart, int packetLength);

    void CacheTestOptions();
    void ProcessDiffResults(JPacketDiffResults);
    void HandleFinishedSending();
    void HandleTestStarted();
    void HandleTestEnded();
    void HandleUnableToStartTest();
    
private:
    Ui::JPTMainWindow *ui;
    QHBoxLayout* mainLayout;
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
    QAction* startAction;
    QAction* stopAction;
    QAction* stepAction;
    QAction* loadAction;
    int outgoingPacketIndex;
    int packetBytesSent;
    int p2InboxPassFailIndex;
    int p3InboxPassFailIndex;
    bool running;
    bool doneSending;

    // Setup Methods
    void RegisterMetaTypes();
    void SetupLayouts();
    void SetInitialStates();
    void ConnectSignalsAndSlots();

    // Utility Methods
    QDir GetInitialWorkingDir();    // CLS - needed to account for OS X .app files
    QString GetJptestFilename(bool IncludeWorkingDir = true);
    QString GetJPacketPath();
    QString GetPortName();
    QString GetHtmlString(const QString& text, const QString& color);
    QString FormatByteToHexString(const unsigned char &byte);
    void SetRunning(const bool& SetRunning = true);

    // UI Methods
    void ShowStatusBarMessage(const QString& msg);
    void ShowMessageBoxMessage(const QString& msg);
    void LogToMessageArea(const QString& msg);
    void UpdateJAGIDStrings(const QString& myName, const QString& p2Name, const QString& p3Name);
    void PostNotification(const int& tabIndex);
    void ClearMailboxes();

    // Inboxes/outboxes methods
    void AppendToStagingArea(char);
    void AppendToP2Inbox(const char& byte, const bool& pass);
    void AppendToP3Inbox(const char& byte, const bool& pass);
    void AppendToGarbageArea(const char& garbageByte);
    void StartNewP2Packet(QByteArray packetStart, QList<bool> passList);
    void StartNewP3Packet(QByteArray packetStart, QList<bool> passList);
};

#endif // JPTMAINWINDOW_H
