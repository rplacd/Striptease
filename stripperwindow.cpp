#include "stripperwindow.h"
#include <cstdlib>
#include <QDebug>

StripperWindow::StripperWindow(QTreeWidget *parent) :
        QTreeWidgetItem(parent, QTreeWidgetItem::Type)
{
}

StripperWindow::StripperWindow(QTreeWidget *parent, HWND hwnd):
        QTreeWidgetItem(parent, QTreeWidgetItem::Type)
{
    //stubbed out.
    handle = hwnd;
    icon = QPixmap();

    LPWSTR lpwstr_buf = (WCHAR*)calloc(sizeof(WCHAR), 255);
    GetWindowText(hwnd, lpwstr_buf, 255);
    title = QString::fromWCharArray(lpwstr_buf, 255);
    free(lpwstr_buf);

    borderless_p = false;

    //this might seem like duplication...
    setData(1, Qt::DisplayRole, QVariant(title));
    setFont(1, QFont(QString("Verdana"), 14, QFont::Normal, false));
    setData(2, Qt::CheckStateRole, QVariant(StrippedP()));
}

bool StripperWindow::StrippedP()
{
    //is WS_CAPTION not set?
    LONG status_base = GetWindowLong(handle, GWL_STYLE);
    return !((status_base & WS_CAPTION) == WS_CAPTION);
}

void StripperWindow::SetStrippedP(bool stripped_p)
{
    LONG status_base = GetWindowLong(handle, GWL_STYLE);
    //set WS_CAPTION - if stripped_p is true, remove WS_CAPTION, etc.
    if(stripped_p) {
        SetWindowLong(handle, GWL_STYLE, status_base & (~WS_CAPTION));
    } else {
        SetWindowLong(handle, GWL_STYLE, status_base | WS_CAPTION);
    }
}

struct AddWindowData {
    QList<StripperWindow*> *list;
    QTreeWidget *parent;
};

BOOL CALLBACK AddWindowToQList(HWND hwnd, LPARAM data_ptr)
{
    //exit early if the hwnd isn't visible. (also - no parent?)
    WINDOWPLACEMENT *wp = (WINDOWPLACEMENT*)malloc(sizeof(WINDOWPLACEMENT));
    wp->length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hwnd, wp);
    if(wp->showCmd == SW_HIDE) {
        free(wp);
        return TRUE;
    } else {
        free(wp);
    }

    //if the hwnd has an empty title or one filled with nulls, quit early.
    LPWSTR lpwstr_buf = (WCHAR*)calloc(sizeof(WCHAR), 255);
    GetWindowText(hwnd, lpwstr_buf, 255);
    QString title = QString::fromWCharArray(lpwstr_buf, 255);
    if(title.isEmpty() || title.at(0) == QChar::Null) {
        free(lpwstr_buf);
        return TRUE;
    } else {
        free(lpwstr_buf);
    }

    // BEGIN TASKBAR ICON TESTS.

    //if the hwnd has an owner, quit early. (see http://www.xtremevbtalk.com/showthread.php?t=163142.)
    if(GetWindow(hwnd, GW_OWNER) != NULL)
        return TRUE;

    //if the hwnd has a parent, quit early.
    if(GetParent(hwnd) != NULL)
        return TRUE;

    //if the hwnd is not visible, quit early.
    LONG status_base = GetWindowLong(hwnd, GWL_STYLE);
    if(!((status_base & WS_VISIBLE) == WS_VISIBLE))
        return TRUE;

    //if the hwnd is WS_EX_TOOLWINDOW, quit early.
    LONG status_ex = GetWindowLong(hwnd, GWL_EXSTYLE);
    if((status_ex & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW)
        return TRUE;

    // END THEM HERE.

    //now add it to the list.
    struct AddWindowData *data = (struct AddWindowData*)data_ptr;
    data->list->insert(0, new StripperWindow(data->parent, hwnd));
    return TRUE;
}

QList<StripperWindow*> *StripperWindow::SystemGetAllWindows(QTreeWidget *parent)
{
    QList<StripperWindow*> *ret = new QList<StripperWindow*>();
    struct AddWindowData data = { ret, parent };
    EnumWindows(&AddWindowToQList, (LPARAM)&data);
    return ret;
}
