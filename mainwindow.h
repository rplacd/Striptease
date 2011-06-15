#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "stripperwindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<StripperWindow*> *windows;

public slots:
    void UpdateWindowList();
    void SetExtraOptionVisiblity(bool);

    //automatic refresh skunkworks.
    void AutomaticRefresh();
    void SetAutomaticRefreshState(bool);
    void SetManualInteractionDisabled(bool);
};

#endif // MAINWINDOW_H
