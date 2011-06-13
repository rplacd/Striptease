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
    void UpdateWindowList();
    Ui::MainWindow *ui;
    QList<StripperWindow*> *windows;
};

#endif // MAINWINDOW_H
