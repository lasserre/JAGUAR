#ifndef JPTMAINWINDOW_H
#define JPTMAINWINDOW_H

#include <QMainWindow>

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
};

#endif // JPTMAINWINDOW_H
