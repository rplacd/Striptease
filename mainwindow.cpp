#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    windows = StripperWindow::SystemGetAllWindows((QTreeWidget*)ui->treeWidget);

    //setting up the tableview's look and feel
    ui->treeWidget->setHeaderLabels(QStringList() << "" << "" << "Stripped?");
    UpdateWindowList();
}

void MainWindow::UpdateWindowList()
{
    for(QList<StripperWindow*>::const_iterator it = windows->begin();
        it != windows->end();
        ++it) {
            delete *it;
    }
    ui->treeWidget->clear();
    windows = StripperWindow::SystemGetAllWindows((QTreeWidget*)ui->treeWidget);
    for(QList<StripperWindow*>::const_iterator it = windows->begin();
        it != windows->end();
        ++it) {
            ui->treeWidget->addTopLevelItem(*it);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    //ui does a fine job of deleting all the items, apparently
    delete windows;
}
