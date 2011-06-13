#ifndef STRIPPERWINDOW_H
#define STRIPPERWINDOW_H

#include "windows.h"
#include <QObject>
#include <QList>
#include <QString>
#include <QPixmap>
#include <QTreeWidget>

class StripperWindow : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
    QString title;
    QPixmap icon;
    bool borderless_p;
    HWND handle;
public:
    explicit StripperWindow(QTreeWidget *parent = 0);
    StripperWindow(QTreeWidget *parent = 0, HWND hwnd = 0);

    bool StrippedP();
    void SetStrippedP(bool stripped_p);

    //sender keeps list.
    static QList<StripperWindow*> *SystemGetAllWindows(QTreeWidget *parent);

signals:

public slots:

};

#endif // STRIPPERWINDOW_H
