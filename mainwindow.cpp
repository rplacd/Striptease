#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <climits>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setting up the main UI
    windows = StripperWindow::SystemGetAllWindows((QTreeWidget*)ui->treeWidget);
    connect(ui->refreshButton, SIGNAL(released()), this, SLOT(UpdateWindowList()));

    //setting up the tableview's look and feel
    ui->treeWidget->setHeaderLabels(QStringList() << "" << "" << "Stripped?");
    UpdateWindowList();

    //rigging up the extend panel button
    ui->extraOptions->setVisible(false);
    connect(ui->revealButton, SIGNAL(toggled(bool)), this, SLOT(SetExtraOptionVisiblity(bool)));

    //riggin up the extend panel's widgets.
    connect(ui->enableAutomaticButton, SIGNAL(toggled(bool)), this, SLOT(SetAutomaticRefreshState(bool)));
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

void MainWindow::SetExtraOptionVisiblity(bool reveal_p)
{
    ui->extraOptions->setVisible(reveal_p);
}

//automatic refresh skunkworks.

void MainWindow::AutomaticRefresh()
{
    UpdateWindowList();
}

void MainWindow::SetAutomaticRefreshState(bool enabled_p)
{
    SetManualInteractionDisabled(enabled_p);
    if(enabled_p) {
    } else {
    }
}

void MainWindow::SetManualInteractionDisabled(bool enabled_p)
{
    ui->refreshButton->setEnabled(!enabled_p);
    ui->treeWidget->setEnabled(!enabled_p);
}

MainWindow::~MainWindow()
{
    delete ui;
    //ui does a fine job of deleting all the items, apparently
    delete windows;
}
