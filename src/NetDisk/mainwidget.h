#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QListWidgetItem>
#include <qscrollarea.h>
#include <QresizeEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <Translist/translist.h>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include "Pathview/pathview.h"
//#include "Netsync/netsync.h"
#include "filespanel.h"
#include "loading.h"
#include "login.h"
#include "configpanel.h"

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
    void scrollValueUpdate(int value);
    void on_viewCut_toggled(bool checked);
    void nextPage(bool);
    void aheadPage(bool);
    void pageUpdate(bool _isFirst, bool _isLast, int _pageNum, int _totalPageNum);
    void actOpenPanel(bool);//打开主面板
    void actOpenWebsite(bool);//打开企业网盘网站
    void actOpenDownloadDir(bool);//打开下载目录
    void actOpenDiskSet(bool);//打开网盘设置
    void actLogout(bool);//注销
    void actQuit(bool);//退出

    void on_functionList_clicked(const QModelIndex &index);

public slots:
    void historyEnabled(bool backEnable, bool aheadEnable);

private:
    Ui::MainWidget *ui;
    loading loadingUi;
    login loginUi;
    QScrollArea* scrollFolder;
    QHBoxLayout* pageLayout;
    QPushButton* page_ahead;
    QPushButton* page_next;
    QLabel* page_info;
    QSystemTrayIcon* sysTray;
    QMenu* sysTrayMenu;
    FilesPanel* diskPanel;
    PathView* pathView;
    TransList* transList;
    ConfigPanel* diskConfig;
//    NetSync* diskSync;
    bool wMoveable;
    bool isLogin;
    QPoint dragPosition;
    int fType;
    //系统托盘菜单项
    QAction* act_openPanel;//打开主面板
    QAction* act_openWebsite;//打开企业网盘网站
    QAction* act_openDownloadDir;//打开下载目录
    QAction* act_openDiskSet;//打开网盘设置
    QAction* act_logout;//注销
    QAction* act_quit;//退出


    void initSysTray();
    void initPageWidgets();
    void initSearch();
    void initSysMenu();
    void initSilidebar();
    void initFunctionList();
    void setSysMenu();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // MAINWIDGET_H
