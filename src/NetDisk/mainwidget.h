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
#include <QPaintEvent>
#include <QLabel>
#include "Translist/synclist.h"
#include <qstackedlayout.h>
#include <qstackedwidget.h>
#include <qtimer.h>
#include "Pathview/pathview.h"
//#include "Netsync/netsync.h"
#include "filespanel.h"
#include "loading.h"
#include "login.h"
#include "configpanel.h"
#include "userinfopanel.h"

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
    void on_sliderbar_clicked(QModelIndex);
    void resetSyncBtn();

    //菜单槽
    void menuOpenWebsite();
    void menuOpenDownloadDir();
    void menuOpenSyncDir();
    void menuOpenDiskSet();
    void menuLogout();
    void menuQuit();

    void on_menu_activated(int index);

    void on_syncStart_clicked();

public slots:
    void historyEnabled(bool backEnable, bool aheadEnable);
    void getSyncNum(int upNum, int downNum);
    void getSyncNum(int upNum);
    void openDiskConfig();
    void diskInit();
    void setSyncState(int state);
    void userinfoUpdate(UserInfo);
    void funcStateEnable(bool);


private:
    Ui::MainWidget *ui;
    loading loadingUi;
    login loginUi;
    UserInfoPanel* userInfo;
    QScrollArea* scrollFolder;
    QStackedLayout* panelStack;
    QHBoxLayout* pageLayout;
    QPushButton* page_ahead;
    QPushButton* page_next;
    QLabel* page_info;
    QSystemTrayIcon* sysTray;
    QMenu* sysTrayMenu;
    QMenu* titleMenu;
    QTimer* syncTimer;
    FilesPanel* diskPanel;
    PathView* pathView;
    TransList* transList;
    ConfigPanel* diskConfig;
    QWidget* pageWidget;
    syncList* syncPanel;
//    NetSync* diskSync;
    bool wMoveable;
    bool isLogin;
    bool isInited;
    bool syncMsgState;
    bool selected;//文件面板是否有文件项被选中
    QPoint dragPosition;
    int fType;
    int syncState;//0:无同步,1:同步中,2:可同步,3:自动同步,4:同步检查中
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
    void initTitleMenu();
//    void setSyncState(bool isSyncing);
    void setPanelState(int state);
    void setFunctionState(bool hasSelectItem);
    void hidePanel();
    void setSysMenu();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void paintEvent(QPaintEvent *);
};

#endif // MAINWIDGET_H
