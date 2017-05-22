#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPushButton>
#include <QLineEdit>
#include <QIcon>
#include <qscrollbar.h>
#include <qdesktopservices.h>
#include <QMessageBox>
#include "filespanel.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    wMoveable = false;
    fType = 0;
    isLogin = false;
    isInited = false;
    syncMsgState = false;
//    ui->viewCut->setHidden(true);

    //网盘设置
    diskConfig = new ConfigPanel();

    if(diskConfig->configIsFinished())
        diskInit();

    //用户信息
    userInfo = new UserInfoPanel();
    userInfo->hide();
    userInfo->installEventFilter(this);
    ui->userinfo->installEventFilter(this);

    //信号槽
    connect(&loginUi, SIGNAL(netLogin()), this, SLOT(netLogin()));
    connect(&loginUi, SIGNAL(diskSet()), this, SLOT(openDiskConfig()));
    connect(&loginUi, SIGNAL(winClose()), this, SLOT(on_wClose_clicked()));
    connect(diskConfig, SIGNAL(configOver()), this, SLOT(diskInit()));
    connect(diskConfig, SIGNAL(configOver()), &loginUi, SLOT(hideLoginWaring()));

    loginUi.show();
    loginUi.autoLogin();
}

MainWidget::~MainWidget()
{
    delete ui;
//    delete pageLayout;
//    delete diskConfig;
}

void MainWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainWidget::initSearch()
{
    QStringList l;
    l<<"全部"<<"图片"<<"视频"<<"文档";
    ui->searchFilter->addItems(l);
    ui->search->installEventFilter(this);
}

void MainWidget::initSysMenu()
{
    ui->menu->addItem("设置");
}

void MainWidget::initSilidebar()
{
    QSize isize = QSize(81,81);
    ui->sliderbar->setIconSize(QSize(36,36));
    QListWidgetItem* item;
    item = new QListWidgetItem(QIcon(":/imgs/slidebar/我的文件.png"),"我的文件");
    item->setTextAlignment(Qt::AlignHCenter);
    item->setSizeHint(isize);
    ui->sliderbar->addItem(item);
//    item = new QListWidgetItem(QIcon(":/imgs/slidebar/他人共享.png"),"他人共享");
//    item->setTextAlignment(Qt::AlignHCenter);
//    item->setSizeHint(isize);
//    ui->sliderbar->addItem(item);
//    item = new QListWidgetItem(QIcon(":/imgs/slidebar/链接管理.png"),"链接管理");
//    item->setTextAlignment(Qt::AlignHCenter);
//    item->setSizeHint(isize);
//    ui->sliderbar->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/slidebar/文件同步.png"),"文件同步");
    item->setTextAlignment(Qt::AlignJustify);
    item->setSizeHint(isize);
    ui->sliderbar->addItem(item);

    ui->sliderbar->setCurrentRow(0);
}

void MainWidget::initFunctionList()
{
    QListWidgetItem* item;
    QSize itemSize = QSize(77,47);
//    ui->functionList->setBatchSize();
    ui->functionList->setIconSize(QSize(47,47));

    item = new QListWidgetItem(QIcon(":/imgs/function/+-上传.png"),"上传");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-新建文件夹.png"),"新建文件夹");
    item->setSizeHint(QSize(110,47));
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-共享.png"),"共享");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-链接分享.png"),"链接分享");
    item->setSizeHint(QSize(90,47));
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-下载.png"),"下载");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-删除.png"),"删除");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-还原.png"),"还原");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);

    setFunctionState(false);

//    ui->searchFilter->addItem(" 全部");
    ui->showDelete->setText("显示已删文件");
    ui->search->setTextMargins(5,0,0,0);

    connect(ui->functionList, SIGNAL(clicked(QModelIndex)), this, SLOT(functionBtnClicked(QModelIndex)));
    //    connect(ui->functionList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(founctionListClicked(QListWidgetItem*)));
}

void MainWidget::initTitleMenu()
{
//    QStringList l;
//    l<<"打开默认下载目录"<<"打开主面板"<<"访问联瑞企业网盘网站"<<"设置"<<"注销"<<"退出";
    ui->menu->addItem("访问联瑞企业网盘网站");
    ui->menu->addItem("打开下载目录");
    ui->menu->addItem("打开同步目录");
    ui->menu->addItem("设置");
    ui->menu->addItem("注销");
    ui->menu->addItem("退出");
    ui->menu->view()->setFixedWidth(150);
}

void MainWidget::setPanelState(int state)
{

}

void MainWidget::setFunctionState(bool hasSelectItem)
{
    selected = hasSelectItem;
    ui->functionList->item(0)->setFlags((Qt::ItemIsEnabled));
    ui->functionList->item(1)->setFlags((Qt::ItemIsEnabled));
    if(hasSelectItem)
    {
        ui->functionList->item(2)->setFlags((Qt::ItemIsEnabled));
        ui->functionList->item(3)->setFlags((Qt::ItemIsEnabled));
        ui->functionList->item(4)->setFlags((Qt::ItemIsEnabled));
        ui->functionList->item(5)->setFlags( (Qt::ItemIsEnabled));
        ui->functionList->item(6)->setFlags((Qt::ItemIsEnabled));
    }
    else
    {
        ui->functionList->item(2)->setFlags(Qt::NoItemFlags);
        ui->functionList->item(3)->setFlags(Qt::NoItemFlags);
        ui->functionList->item(4)->setFlags(Qt::NoItemFlags);
        ui->functionList->item(5)->setFlags(Qt::NoItemFlags);
        ui->functionList->item(6)->setFlags(Qt::NoItemFlags);
    }
}

void MainWidget::setSyncState(int state)//isSyncing = checked
{
    qDebug()<<"sync state"<<state;
    syncState = state;
    ui->syncMsg->clear();
    switch(state)
    {
    case 0://无同步
        ui->syncMsg->setText("没有文件需要同步");
        ui->syncStart->setStyleSheet("#syncStart{border-image: url(:/imgs/一键同步.png);color: rgb(255, 255, 255);}\
                                     #syncStart:pressed{border-image: url(:/imgs/一键同步hover.png);color: rgb(255, 255, 255);}");
        break;
    case 1://同步中
        ui->syncStart->setStyleSheet("#syncStart{border-image: url(:/imgs/同步中.png);color: rgb(255, 255, 255);}\
                                     #syncStart:pressed{border-image: url(:/imgs/同步中hover.png);color: rgb(255, 255, 255);}");
        break;
    case 2://可同步
        ui->syncMsg->setText("可同步");
        ui->syncStart->setStyleSheet("#syncStart{border-image: url(:/imgs/一键同步.png);color: rgb(255, 255, 255);}\
                                     #syncStart:pressed{border-image: url(:/imgs/一键同步hover.png);color: rgb(255, 255, 255);}");
        break;
    case 3://自动同步
        ui->syncStart->setStyleSheet("#syncStart{border-image: url(:/imgs/一键同步.png);color: rgb(255, 255, 255);}\
                                     #syncStart:pressed{border-image: url(:/imgs/一键同步hover.png);color: rgb(255, 255, 255);}");
        break;
    default:
        break;
    }
//    if(state)
//    {
//        ui->syncStart->setEnabled(false);qDebug()<<"setfalse1";
//        ui->syncStart->setChecked(true);
//    }
//    else
//    {
//        ui->syncStart->setEnabled(true);qDebug()<<"settrue1";
//        ui->syncStart->setChecked(false);
    //    }
}

void MainWidget::userinfoUpdate(UserInfo info)
{
    ui->userinfo->setText(info.USER_NAME);
    userInfo->userinfoUpdate(info);
}

void MainWidget::funcStateEnable(bool enable)
{
    setFunctionState(enable);
}

void MainWidget::hidePanel()
{
    transList->hide();
    scrollFolder->hide();
    loadingUi.hide();
    pageWidget->hide();
    syncPanel->hide();
    ui->frame_sync->hide();
    ui->frame_function->hide();
}

void MainWidget::setSysMenu()
{
    if(isLogin)
    {
        act_logout->setEnabled(true);
        act_openDiskSet->setEnabled(true);
        act_openPanel->setEnabled(true);
        act_openDownloadDir->setEnabled(true);
        act_quit->setEnabled(true);
        act_openWebsite->setEnabled(true);
    }
    else
    {
        act_logout->setEnabled(false);
        act_openDiskSet->setEnabled(false);
        act_openPanel->setEnabled(false);
        act_openDownloadDir->setEnabled(true);
        act_quit->setEnabled(true);
        act_openWebsite->setEnabled(true);
    }
}


//鼠标事件
void MainWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        QRect rect = ui->title->frameRect();
        if(rect.contains(event->pos()))
        {
            wMoveable = true;
        }
        event->accept();
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && wMoveable)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent*)
{
    if(wMoveable)
    {
        wMoveable = false;
    }
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->search)
    {
        if(event->type() == QEvent::FocusIn)
        {
            ui->search->clear();
            ui->search->setStyleSheet("#search{\
                                      background-color: rgb(255, 255, 255);\
                                      border: 1px solid rgb(187, 187, 187);\
                                      border-left-width: 0px;\
                                      border-right-width: 0px;\
                                      color:black;\
                                  }");
        }
        else if(event->type() == QEvent::FocusOut)
        {
/*            ui->search->setText("搜索我的文件");
            ui->search->setStyleSheet("#search{\
                                      background-color: rgb(255, 255, 255);\
                                      border: 1px solid rgb(187, 187, 187);\
                                      border-left-width: 0px;\
                                      border-right-width: 0px;\
                                      color:gray;\
                                  }");
*/
        }

    }
    else if((watched == ui->userinfo) || (watched == userInfo))
    {
        if(event->type() == QEvent::Enter)
        {
            userInfo->move(this->geometry().x()+ui->userinfo->geometry().x(),this->geometry().y()+ui->userinfo->geometry().y()+ui->userinfo->geometry().height());
            userInfo->show();
        }
        if(event->type() == QEvent::Leave)
        {
            userInfo->hide();
        }
    }

    return QWidget::eventFilter(watched,event);
}

void MainWidget::on_wMin_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWidget::on_wClose_clicked()
{
//    qmessage
    qDebug("close");
    this->close();
}

void MainWidget::on_wMax_toggled(bool checked)
{
    if(checked)
    {
        this->setWindowState(Qt::WindowMaximized);
    }
    else
    {
        this->setWindowState(Qt::WindowNoState);

    }
    diskPanel->review();
}

void MainWidget::founctionListClicked(QListWidgetItem*)
{

}

void MainWidget::functionBtnClicked(QModelIndex index)
{
    int funcNum = index.row();

    if((!selected) && (funcNum>1))
        return;

    switch (funcNum)
    {
        case 0:
            qDebug("upload");break;
            diskPanel->fileUpload(); break;
        case 1:
            diskPanel->fileNew(); break;
        case 2:
            qDebug("share");
            diskPanel->panelShare();break;
        case 3:
            diskPanel->getSelectFid();
            qDebug("link");break;
        case 4:
            qDebug("download");break;
        case 5:
            qDebug("delete");break;
        case 6:
            qDebug("restore");break;

        default:
            break;
    }
}

void MainWidget::historyEnabled(bool backEnable, bool aheadEnable)
{
    ui->back->setEnabled(backEnable);
    ui->forward->setEnabled(aheadEnable);
}

void MainWidget::getSyncNum(int upNum, int downNum)
{
    QString strLocal = QString("本地更新文件%1个").arg(upNum);
    QString strHost = QString("云端更新文件%1个").arg(downNum);
    ui->msg_local->setText(strLocal);
    ui->msg_host->setText(strHost);
    if(!(upNum||downNum))
    {
//        ui->frame_sync->hide();
        if(syncMsgState)//同步状态发生跳变
            sysTray->showMessage(QString("文件同步"), "本地文件已和云端同步",QSystemTrayIcon::Information);
        setSyncState(0);
//        ui->syncStart->setEnabled(false);
        syncMsgState = false;
    }
    else
    {
            if(syncState == 2)//可同步
            {
                setSyncState(1);//变更为同步中
            }
            else if(syncState == 0)//无同步
            {
                setSyncState(2);//变更为可同步状态
            }
//            ui->frame_sync->show();
            if(!syncMsgState)//同步状态发生跳变
                sysTray->showMessage(QString("文件同步"), strHost+" "+strLocal,QSystemTrayIcon::Information);
            syncMsgState = true;
//        }
    }
}

void MainWidget::getSyncNum(int upNum)
{
    QString str = QString("本地更新文件%1个").arg(upNum);
//    ui->syncMsg->setText(str);
    sysTray->showMessage(QString("文件同步"), str,QSystemTrayIcon::Information);
}

void MainWidget::openDiskConfig()
{
    diskConfig->show();
}

void MainWidget::diskInit()
{
    if(netConf->autoSyncDir())
        setSyncState(0);
    else if(syncMsgState)
        setSyncState(1);
    else
        setSyncState(0);

    if(isInited)
    {
//        scrollFolder->hide();
//        loadingUi.show();
//        syncPanel->hide();
//        ui->frame_sync->hide();
//        ui->frame_function->hide();
//        diskPanel->panelRefresh();
        return;
    }

    isInited = true;

    netConf->manager = new QNetworkAccessManager(this);

    //系统托盘图标
    initSysTray();

    //搜索栏
    initSearch();

    //侧边栏
    initSilidebar();

    //功能栏
    initFunctionList();

    //标题栏菜单
    initTitleMenu();

    //传输列表
    transList = new TransList(this);
    syncPanel = new syncList(this);


    //路径面板
    pathView = new PathView(this);
    pathView->setMaximumHeight(49);
    pathView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    ui->pathLayout->addWidget(pathView);

    //文件面板
//    panelStack = new QStackedLayout(this);
    pageWidget = new QWidget(this);

    //同步信息
    ui->msg_local->setText("本地同步文件0个");
    ui->msg_host->setText("云端同步文件0个");

    scrollFolder = new QScrollArea(this);
    scrollFolder->setFrameShape(QFrame::NoFrame);
    scrollFolder->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollFolder->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollFolder->setWidgetResizable(true);
    ui->panelLayout->addWidget(scrollFolder);
//    ui->panelLayout->addWidget(transList);
    ui->panelLayout->addWidget(&loadingUi);
    ui->panelLayout->addWidget(syncPanel);

    ui->mainLayout->addWidget(transList);
//    scrollFolder->verticalScrollBar()->setRange(0,100);
//    loadingUi.reloadStart();
    scrollFolder->hide();
    loadingUi.show();
    syncPanel->hide();
    ui->frame_sync->hide();
//    ui->frame_function->hide();
    diskPanel = new FilesPanel(this);
    diskPanel->httpClient->netInit(transList);

    //文件同步时钟
    syncTimer = new QTimer(this);
    syncTimer->start(600000);//600s
    connect(syncTimer, SIGNAL(timeout()), diskPanel->diskSync, SLOT(loginSync()));

    initPageWidgets();
    scrollFolder->setWidget(diskPanel);
    syncPanel->initTable(diskPanel->diskSync->getTable());

    connect(diskPanel->httpClient, SIGNAL(loginStateChanged(bool)), this, SLOT(loginRst(bool)));
    connect(diskPanel->httpClient, SIGNAL(newUserInfo(UserInfo)), this, SLOT(userinfoUpdate(UserInfo)));
    connect(diskPanel, SIGNAL(pathChanged(QList<fileInfo*>)), pathView, SLOT(pathChange(QList<fileInfo*>)));
    connect(diskPanel, SIGNAL(historyEnable(bool,bool)), this, SLOT(historyEnabled(bool,bool)));
    connect(diskPanel, SIGNAL(newTask(netTrans*)), transList, SLOT(newTask(netTrans*)));
    connect(diskPanel, SIGNAL(isLoading(bool)), this, SLOT(isLoading(bool)));
    connect(diskPanel, SIGNAL(scrollValueChanged(int)), this, SLOT(scrollValueUpdate(int)));
    connect(diskPanel, SIGNAL(hasSelected(bool)), this, SLOT(funcStateEnable(bool)));
//    connect(ui->syncStart, SIGNAL(clicked()), diskPanel->diskSync, SLOT(syncTaskUpload()));
//    connect(ui->syncStart, SIGNAL(clicked()), diskPanel->diskSync, SLOT(syncTaskDownload()));
    connect(syncPanel, SIGNAL(pathChanged(QList<QFileInfo*>)), pathView, SLOT(pathChange(QList<QFileInfo*>)));
    connect(syncPanel, SIGNAL(historyEnable(bool,bool)), this, SLOT(historyEnabled(bool,bool)));
    connect(syncPanel, SIGNAL(syncNumChanged(int,int)), this, SLOT(getSyncNum(int,int)));
//    connect(diskPanel->diskSync, SIGNAL(syncStateChanged(bool)), this, SLOT(syncEnable(bool)));
    connect(ui->showDelete, SIGNAL(toggled(bool)), diskPanel, SLOT(showDelete(bool)));
    connect(pathView, SIGNAL(cdRequest(double)), diskPanel, SLOT(cmdCd(double)));
    connect(pathView, SIGNAL(cdRequest(int)), syncPanel, SLOT(cmdCd(int)));
    connect(ui->searchFilter,  SIGNAL(currentIndexChanged(int)), this, SLOT(searchTypeChanged(int)));
    connect(ui->sliderbar, SIGNAL(clicked(QModelIndex)), this, SLOT(on_sliderbar_clicked(QModelIndex)));
}

void MainWidget::initSysTray()
{
    sysTray = new QSystemTrayIcon(this);
    sysTray->setIcon(QIcon(":/imgs/app.ico"));
    sysTray->setToolTip("联瑞企业网盘");
    act_openDownloadDir = new QAction("打开默认下载目录");
    act_openPanel = new QAction("打开主面板");
    act_openWebsite = new QAction("访问联瑞企业网盘网站");
    act_openDiskSet = new QAction("设置");
    act_logout = new QAction("注销");
    act_quit = new QAction("退出");

    sysTrayMenu = new QMenu(this);
    titleMenu = new QMenu(ui->title);
    sysTray->setContextMenu(sysTrayMenu);
    sysTrayMenu->addAction(act_openPanel);
    sysTrayMenu->addAction(act_openWebsite);
    sysTrayMenu->addSeparator();
    sysTrayMenu->addAction(act_openDownloadDir);
    sysTrayMenu->addSeparator();
    sysTrayMenu->addAction(act_openDiskSet);
    sysTrayMenu->addAction(act_logout);
    sysTrayMenu->addAction(act_quit);

    connect(act_openPanel, SIGNAL(triggered(bool)), this, SLOT(actOpenPanel(bool)));
    connect(act_openWebsite, SIGNAL(triggered(bool)), this, SLOT(actOpenWebsite(bool)));
    connect(act_openDownloadDir, SIGNAL(triggered(bool)), this, SLOT(actOpenDownloadDir(bool)));
    connect(act_openDiskSet, SIGNAL(triggered(bool)), this, SLOT(actOpenDiskSet(bool)));
    connect(act_logout, SIGNAL(triggered(bool)), this, SLOT(actLogout(bool)));
    connect(act_quit, SIGNAL(triggered(bool)), this, SLOT(actQuit(bool)));

    setSysMenu();

    sysTray->show();
}

void MainWidget::initPageWidgets()
{
    pageLayout = new QHBoxLayout();
    page_ahead = new QPushButton("上一页",pageWidget);
    page_next = new QPushButton("下一页",pageWidget);
    page_info = new QLabel("第0页/共0页",pageWidget);

    ui->panelLayout->addWidget(pageWidget);
    pageWidget->setLayout(pageLayout);

    page_ahead->setStyleSheet("QPushButton{border:0px;color:rgb(67,122,232)}QPushButton:hover{text-decoration: underline}");
    page_next->setStyleSheet("QPushButton{border:0px;color:rgb(67,122,232)}QPushButton:hover{text-decoration: underline}");
    page_info->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    pageLayout->addStretch(1);
    pageLayout->addWidget(page_ahead);
    pageLayout->addWidget(page_info);
    pageLayout->addWidget(page_next);
    pageLayout->addStretch(1);

//    ui->panelLayout->addLayout(pageLayout);

    connect(diskPanel->httpClient, SIGNAL(pageChanged(bool,bool,int,int)), this, SLOT(pageUpdate(bool,bool,int,int)));
    connect(page_ahead, SIGNAL(clicked(bool)), this, SLOT(aheadPage(bool)));
    connect(page_next, SIGNAL(clicked(bool)), this, SLOT(nextPage(bool)));


}

void MainWidget::on_back_clicked()
{
    if(syncPanel->isHidden())
        diskPanel->panelBack();
    else
        syncPanel->goBack();
}

void MainWidget::on_forward_clicked()
{
    if(syncPanel->isHidden())
        diskPanel->panelAhead();
    else
        syncPanel->goAhead();
}

void MainWidget::on_refresh_clicked()
{
//    isLoading(true);
    if(syncPanel->isHidden())
        diskPanel->panelRefresh();
    else
        syncPanel->syncRefresh();
}

void MainWidget::on_translist_toggled(bool checked)
{qDebug()<<"checked"<<checked;
//    hidePanel();
    if(checked)
    {
        ui->mainPanel->hide();
        transList->show();
    }
    else
    {
        transList->hide();
        ui->mainPanel->show();
//        on_sliderbar_clicked(ui->sliderbar->currentIndex());
    }
}

void MainWidget::searchTypeChanged(int i)
{
    char c[4] = {0,3,1,4};//"全部"<<"图片"<<"视频"<<"文档";
    //l<<"全部"<<"视频"<<"音频"<<"图片"<<"文档"<<"压缩文件"<<"其它";
    fType = c[i];
}

void MainWidget::on_searchBtn_clicked()
{
    QString str = ui->search->text();

    ui->search->setStyleSheet("#search{\
                              background-color: rgb(255, 255, 255);\
                              border: 1px solid rgb(187, 187, 187);\
                              border-left-width: 0px;\
                              border-right-width: 0px;\
                              color:gray;\
                          }");
    if((str == "搜索我的文件") || str.isEmpty())
    {
        ui->search->setText("搜索我的文件");
        if(fType == 0)
            return;
        diskPanel->panelSearch(fType);
        return;
    }
    diskPanel->panelSearch(fType,str);
}

void MainWidget::isLoading(bool checked)
{qDebug()<<"loadint"<<checked;
    if(checked)
    {
//        loadingUi.reloadStart();
        loadingUi.show();
        scrollFolder->hide();
        transList->hide();
    }
    else
    {
        loadingUi.reloadStop();
        scrollFolder->show();
        loadingUi.hide();
        transList->hide();
    }
}

void MainWidget::reload()
{qDebug("reloading");
    diskPanel->panelRefresh();
}

void MainWidget::netLogin()
{
//    loginUi.close();
//    this->show();
//    diskPanel->panelCd((fileInfo*)NULL);
    diskPanel->httpClient->netLogin(netConf->getUsrname(), netConf->getPasswd());
}

void MainWidget::netClose()
{qDebug()<<"close";
    loginUi.close();
    this->close();
}

void MainWidget::loginRst(bool isSucceed)
{
    if(isSucceed)
    {
        hidePanel();
        loginUi.close();
        this->show();
        pageWidget->show();
        ui->frame_function->show();
        ui->sliderbar->setCurrentRow(0);
        diskPanel->panelCd((fileInfo*)NULL);
        isLogin = true;
        setSysMenu();
    }
    else
    {
        loginUi.loginFailed();
    }
}

void MainWidget::scrollValueUpdate(int value)
{
    scrollFolder->verticalScrollBar()->setValue(value);
}

void MainWidget::on_viewCut_toggled(bool checked)
{
    if(checked)
        scrollFolder->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    else
        scrollFolder->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    diskPanel->setViewMode(checked);
}

void MainWidget::pageUpdate(bool _isFirst,bool _isLast,int _pageNum,int _totalPageNum)
{
    qDebug()<<_isFirst<<_isLast<<_pageNum<<_totalPageNum;
    diskPanel->pageNum = _pageNum;
    diskPanel->totalPageNum = _totalPageNum;
    page_ahead->setEnabled(!_isFirst);
    page_next->setEnabled(!_isLast);
    page_info->setText(QString("第%1页/共%2页").arg(_pageNum).arg(_totalPageNum));
}

void MainWidget::actOpenPanel(bool)
{
    this->setWindowState(Qt::WindowNoState);
    this->show();
}

void MainWidget::actOpenWebsite(bool)
{
    QDesktopServices::openUrl(QUrl(netConf->getServerAddress()));
}

void MainWidget::actOpenDownloadDir(bool)
{
    QDesktopServices::openUrl(QUrl("file:///"+netConf->getDownloadPath()));
}

void MainWidget::actOpenDiskSet(bool)
{
    diskConfig->show();
}

void MainWidget::actLogout(bool)
{
    isLogin = false;
    this->hide();
    loginUi.logout();
    loginUi.show();
    setSysMenu();
}

void MainWidget::actQuit(bool)
{
    this->close();
}

void MainWidget::on_sliderbar_clicked(QModelIndex index)
{
//    bool syncHidden = ui->frame_sync->isHidden();
    hidePanel();

    switch(index.row())
    {
    case 0://我的文件
        scrollFolder->show();
        pageWidget->show();
//        if(!syncHidden)
//        {
//            ui->frame_sync->show();
//            ui->syncStart->setEnabled(true);
//        }
        ui->frame_function->show();
        diskPanel->pathRefresh();
        ui->searchBtn->setEnabled(true);
        ui->searchFilter->setEnabled(true);
        ui->search->setEnabled(true);
        break;
    case 1://我的同步
        syncPanel->show();
//        if(!syncHidden)
//        {
//            ui->frame_sync->show();
//            ui->syncStart->setEnabled(true);
//        }
        ui->frame_sync->show();
        ui->searchBtn->setEnabled(false);
        ui->searchFilter->setEnabled(false);
        ui->search->setEnabled(false);
        break;
    default:break;
    }
}

void MainWidget::aheadPage(bool)
{
    if(diskPanel->pageNum < 2)
        return;
    diskPanel->panelCdPage(diskPanel->pageNum-1);
}

void MainWidget::nextPage(bool)
{
    if(diskPanel->pageNum > (diskPanel->totalPageNum-1))
        return;
    diskPanel->panelCdPage(diskPanel->pageNum+1);
}


void MainWidget::on_functionList_clicked(const QModelIndex&)
{

}

void MainWidget::menuOpenWebsite()
{
    QDesktopServices::openUrl(QUrl(netConf->getServerAddress()));
}

void MainWidget::menuOpenDownloadDir()
{
    QDesktopServices::openUrl(QUrl("file:///"+netConf->getDownloadPath()));
}

void MainWidget::menuOpenSyncDir()
{
    QDesktopServices::openUrl(QUrl("file:///"+netConf->getSyncPath()));
}

void MainWidget::menuOpenDiskSet()
{
    diskConfig->show();
}

void MainWidget::menuLogout()
{
    isLogin = false;
    this->hide();
    loginUi.logout();
    loginUi.show();
    setSysMenu();
}

void MainWidget::menuQuit()
{
    this->close();
}

void MainWidget::on_menu_activated(int index)
{
    switch(index)
    {
    case 0:
        menuOpenWebsite();break;
    case 1:
        menuOpenDownloadDir();break;
    case 2:
        menuOpenSyncDir();break;
    case 3:
        menuOpenDiskSet();break;
    case 4:
        menuLogout();break;
    case 5:
        menuQuit();break;

    default:break;
    }
}

void MainWidget::on_syncStart_clicked()
{
    switch(syncState)
    {
    case 0://无同步
        setSyncState(4);
        ui->syncMsg->setText("正在检查同步");
        diskPanel->diskSync->loginSync();
//        if(diskPanel->diskSync->syncLocalCheck())//检查本地同步
        break;
    case 1://同步中
        ui->syncMsg->setText("正在同步中");
        break;
    case 2://可同步
        setSyncState(1);
        diskPanel->diskSync->syncTaskDownload();
        diskPanel->diskSync->syncTaskUpload();
        break;
    case 3://自动同步
        ui->syncMsg->setText("自动同步无需手动操作");
    case 4://同步检测中
        ui->syncMsg->setText("正在检查同步");
        break;
    }
}
