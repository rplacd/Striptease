#ifndef STRIPPERWINDOW_H
#define STRIPPERWINDOW_H

#include "windows.h"
#include <QObject>
#include <QList>
#include <QString>
#include <QTreeWidget>

class StripperWindow : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
    QString title;
    QString process_name;
    bool borderless_p;
    HWND handle;
public:
    explicit StripperWindow(QTreeWidget *parent = 0);
    StripperWindow(QTreeWidget *parent = 0, HWND hwnd = 0);

    bool StrippedP();
    void SetStrippedP(bool stripped_p);

    virtual void setData(int column, int role, const QVariant &value);

    //sender keeps list.
    static QList<StripperWindow*> *SystemGetAllWindows(QTreeWidget *parent);

signals:

public slots:

};

#endif // STRIPPERWINDOW_H
