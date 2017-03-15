#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QListWidgetItem>
#include <qscrollarea.h>
#include <QresizeEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <Translist/translist.h>
#include "Pathview/pathview.h"
#include "filespanel.h"
#include "loading.h"
#include "login.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

signals:
    void functionNew();
    void functionUpload();
    void functionDonwload();
    void functionDelete();

private slots:
    void on_wMin_clicked();
    void on_wClose_clicked();
    void on_wMax_toggled(bool checked);
    void founctionListClicked(QListWidgetItem* item);
    void functionBtnClicked(QModelIndex index);
    void on_back_clicked();
    void on_forward_clicked();
    void on_refresh_clicked();
    void on_translist_toggled(bool checked);
    void searchTypeChanged(int);
    void on_searchBtn_clicked();
    void isLoading(bool);
    void reload();
    void netLogin();
    void netClose();
    void loginRst(bool);

    void on_viewCut_toggled(bool checked);

public slots:
    void historyEnabled(bool backEnable, bool aheadEnable);

private:
    Ui::MainWidget *ui;
    loading loadingUi;
    login loginUi;
    QScrollArea* scrollFolder;
    QSystemTrayIcon* sysTray;
    FilesPanel* diskPanel;
    PathView* pathView;
    TransList* transList;
    bool wMoveable;
    QPoint dragPosition;
    int fType;
    //系统托盘菜单项
    QAction* act_openPanel;//打开主面板
    QAction* act_openWebsite;//打开企业网盘网站
    QAction* act_openDownloadUrl;//打开下载目录
//    Qaction* act_open


    void initSysTray();
    void initSearch();
    void initSysMenu();
    void initSilidebar();
    void initFunctionList();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // MAINWIDGET_H
