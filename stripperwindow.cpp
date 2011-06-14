#include "stripperwindow.h"
#include <cstdlib>
#include <cstdarg>
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

    HICON icon_handle = (HICON)GetClassLongPtr(handle, GCLP_HICON);
    //pre-empt the "failed to GetIconInfo() error" so it speeds through quickly.
    ICONINFO dummy;
    if(!GetIconInfo(icon_handle, &dummy)) {
        icon = QPixmap();
    } else {
        icon = QPixmap::fromWinHICON(icon_handle);
    }

    LPWSTR lpwstr_buf = (WCHAR*)calloc(sizeof(WCHAR), 255);
    GetWindowText(hwnd, lpwstr_buf, 255);
    title = QString::fromWCharArray(lpwstr_buf, 255);
    free(lpwstr_buf);

    borderless_p = false;

    //this might seem like duplication...
    setData(0, Qt::DecorationRole, QVariant(icon));
    setData(1, Qt::DisplayRole, QVariant(title));
    setFont(1, QFont(QString("Verdana"), 0, QFont::Bold, false));
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
    //don't unnecessarily go through setting the same state and then resizing!
    //we don't want extra resizing!
    if(StrippedP() == stripped_p)
        return;

    LONG status_base = GetWindowLong(handle, GWL_STYLE);
    //set WS_CAPTION - if stripped_p is true, remove WS_CAPTION, etc.
    LONG res;
    if(stripped_p) {
        res = SetWindowLong(handle, GWL_STYLE, status_base & (~WS_CAPTION));
    } else {
        res = SetWindowLong(handle, GWL_STYLE, status_base | WS_CAPTION);
    }

    if(res == 0) {
        LPWSTR msg_buf = (WCHAR*)calloc(sizeof(WCHAR), 512);
        DWORD err = GetLastError();
        va_list vl;
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, msg_buf, 512, &vl);
        QString error_text = QString::fromWCharArray(msg_buf, 255);

        qDebug() << error_text;

        //now trap Access is Denieds and make sure it disables further interaction.
        if(err == ERROR_ACCESS_DENIED)
            setDisabled(true);
    }

    //force a double window resize AND a redraw - don't just repaint, since it doesn't seem to actually update the position of the controls.
    RECT win_rect;
    GetWindowRect(handle, &win_rect);
    SetWindowPos(handle, NULL,
                 win_rect.left, win_rect.top,
                 win_rect.right - win_rect.left, //so we get some resizing done...
                 win_rect.bottom - win_rect.top - 1,
                 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    SetWindowPos(handle, NULL,
                 win_rect.left, win_rect.top,
                 win_rect.right - win_rect.left , //so we get some resizing done...
                 win_rect.bottom - win_rect.top + 1,
                 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    InvalidateRect(handle, NULL, TRUE);
}

void StripperWindow::setData(int column, int role, const QVariant &value)
{
    QTreeWidgetItem::setData(column, role, value);
    if(column == 2) {
        SetStrippedP(value.toBool());
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
