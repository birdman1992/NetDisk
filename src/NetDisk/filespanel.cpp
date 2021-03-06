#include "filespanel.h"
#include "ui_filespanel.h"
#include "QFolder/qfolder.h"
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QStandardItem>
#include <qscrollbar.h>
#include "Share/sharelink.h"

#define FOLDER_SIZE 100

FilesPanel::FilesPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilesPanel)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    pasteEnable = false;

    curPanel.clear();
    dirTree.clear();
    folderPath.clear();
    checkList.clear();
    pFolder = NULL;
    curDirId = 0;
    pCdFolder = NULL;
    showListView = true;
    resizeEventEnable = true;
    curDirId = -1;
    pageSize = 56;
    pageNum = 1;
    totalPageNum = 1;
    showDeleteFolder = 0;
    isResize = false;

    //http
    httpClient = new NetHttp(this);

    diskSync = new NetSync(this);
    diskSync->setNetClient(httpClient);

    //主菜单
    menu = new QMenu(this);
    act_new = new QAction("新建文件夹");
    act_refresh = new QAction("刷新");
    act_sort = new QAction("排序");
    act_upload = new QAction("上传");
    connect(act_new, SIGNAL(triggered(bool)), this, SLOT(fileNew()));
    connect(act_refresh, SIGNAL(triggered(bool)), this, SLOT(fileRefresh()));
    connect(act_sort, SIGNAL(triggered(bool)), this, SLOT(fileSort()));
    connect(act_upload, SIGNAL(triggered(bool)), this, SLOT(fileUpload()));

    setTableState(true);

    //FTP槽
//    connect(&ftpClient, SIGNAL(listInfo(QUrlInfo)), this, SLOT(ftpGetListInfo(QUrlInfo)));
//    connect(&ftpClient, SIGNAL(needRefresh()), this, SLOT(fileRefresh()));
//    connect(&ftpClient, SIGNAL(cmdCd()), this, SLOT(ftpCdFinishi()));

    //HTTP槽
    connect(httpClient, SIGNAL(listUpdate(QList<fileInfo*>)), this, SLOT(httpGetListInfo(QList<fileInfo*>)));
    connect(httpClient, SIGNAL(updateRequest()), this, SLOT(fileRefresh()));
//    connect(httpClient, SIGNAL(newTask(netTrans*)), this, SIGNAL(newTask(netTrans*)));
    connect(httpClient,SIGNAL(shareLink(QString,QString)), this, SLOT(getShareLink(QString,QString)));

    //排序子菜单
    menu_sort = new QMenu(this);
    act_filename = new QAction("文件名");
    act_filesize = new QAction("大小");
    act_filetime = new QAction("修改时间");

//    httpClient->netLogin("admin","888888");
//    httpClient->netList(curDirId,pageNum,pageSize);
//    ftpClient.ftpLogin("test", "123456");
}

FilesPanel::~FilesPanel()
{
    delete ui;
}

/***↓↓↓↓公有接口↓↓↓↓***/

//设置视图显示模式
//showList:true 显示列表视图  false 显示平铺视图
void FilesPanel::setViewMode(bool showList)
{
    qDebug()<<"list"<<showList;
    showListView = showList;
    panelShow(curPanel);
}

//显示文件夹面板
void FilesPanel::panelShow(QList<QFolder*> fPanel)
{
    if(showListView)
    {qDebug("show list");
        showList(fPanel);
        return;
    }
    else
    {qDebug("show folder");
//        showFolder(fPanel);
//        return;
        int i = 0;
        int j = 0;
        int offset_x = 15;
        int offset_y = 15;

        if(fPanel.isEmpty())
        {
            emit isLoading(false);
            return;
        }
        emit isLoading(false);
        ui->listView->hide();
        qDebug()<<"geometry"<<this->geometry().width();
        int ele_wid = fPanel.at(i)->geometry().width() + offset_x;
        int ele_hei = fPanel.at(i)->geometry().height() + offset_y;
        int count_x = (this->geometry().width() - offset_x)/(ele_wid);
        int panelSize_h = ((fPanel.count()/count_x) + (fPanel.count()%count_x != 0))*ele_hei + offset_y;
        this->setMinimumHeight(panelSize_h);

        for(i=0; i<fPanel.count();)
        {
            fPanel.at(i)->move(offset_x + (i % count_x) * ele_wid, offset_y + j * ele_hei);
            fPanel.at(i)->show();
            i++;
            if(!(i%count_x))
                j++;
        }

        return;
    }

}

void FilesPanel::panelClear()
{
    QFolder* f;
    emit isLoading(true);

    while(!curPanel.isEmpty())
    {
        f = curPanel.takeFirst();
        f->deleteLater();
    }
}

void FilesPanel::panelRefresh()
{
//    qDebug("refresh");
    resizeEventEnable = false;
    httpClient->netList(curDirId, pageNum, pageSize, showDeleteFolder);
    resizeEventEnable = true;
}

void FilesPanel::panelCopy(QFolder *p)
{
    pClipboard = p;
}

void FilesPanel::panelPaste()
{
    qDebug("paste");
}

void FilesPanel::panelCd(fileInfo* dir)
{
//    int i = 0;
//    bool del = false;
    qDebug()<<"HTTP:cd";
    panelClear();
    if(dir == NULL)
    {
        while(!folderPath.isEmpty())
        {
            fileInfo* f = folderPath.takeFirst();
            delete f;
        }
        curDirId = -1;
        curIndex = 0;
        pageNum = 1;
        httpClient->netList(curDirId, pageNum, pageSize, showDeleteFolder);
        emit historyEnable(false, false);
        emit pathChanged(folderPath);
        return;
    }
    fileInfo* info = new fileInfo(*dir);

    while((folderPath.count()-1)>curIndex)
    {
        fileInfo* f = folderPath.takeLast();
        delete f;
    }

//    for(i=0; i<folderPath.count(); i++)
//    {
//        if(del||(i>curIndex))
//        if(i>curIndex)
//        {
//            fileInfo* f = folderPath.takeAt(i);
//            delete f;
//            continue;
//        }
//        if(folderPath.at(i)->ID == dir->ID)
//            del = true;
//    }
//    if(!del)
        folderPath<<info;
    curDirId = dir->ID;
    curIndex = folderPath.count()-1;
    pageNum = 1;
    httpClient->netList(curDirId, pageNum, pageSize, showDeleteFolder);
    emit historyEnable(true, false);
    emit pathChanged(folderPath);
}

void FilesPanel::panelCd(double dirId)
{
    int i = 0;
    bool del = false;
    fileInfo* f = NULL;
    qDebug()<<"HTTP:cd"<<dirId;
    panelClear();
    if(dirId == -1)
    {
        while(!folderPath.isEmpty())
        {
            f = folderPath.takeFirst();
            delete f;
        }
        curDirId = -1;
        curIndex = 0;
        httpClient->netList(curDirId, pageNum, pageSize, showDeleteFolder);
        emit pathChanged(folderPath);
        emit historyEnable(false, false);
        return;
    }

//    qDebug()<<"FTP:cd"<<dir->info()->ID;
    for(i=0; i<folderPath.count(); i++)
    {
        if(del)
        {
            f = folderPath.takeAt(i);
            delete f;
            continue;
        }
        if(folderPath.at(i)->ID == dirId)
            del = true;
    }
    curDirId = folderPath.last()->ID;
    curIndex = folderPath.count()-1;
    httpClient->netList(curDirId, pageNum, pageSize, showDeleteFolder);
    qDebug("pathChanged");
    emit pathChanged(folderPath);
    emit historyEnable(true, false);
}

void FilesPanel::panelCdPage(int page)
{
    pageNum = page;
    httpClient->netList(curDirId, pageNum, pageSize, showDeleteFolder);
}

void FilesPanel::panelBack()
{
    if(curIndex < 0)
        return;
    if(curIndex == 0)
    {
        httpClient->netList(-1, pageNum, pageSize, showDeleteFolder);
        curDirId = -1;
        curIndex--;
    }
    else
    {
        curIndex--;
        httpClient->netList(folderPath.at(curIndex)->ID,pageNum,pageSize, showDeleteFolder);
        curDirId = folderPath.at(curIndex)->ID;
    }

    emit pathChanged(folderPath.mid(0,curIndex+1));

    if(curIndex < 0)
        emit historyEnable(false, true);
    else
        emit historyEnable(true, true);
}

void FilesPanel::panelAhead()
{
    if(curIndex >= (folderPath.count()-1))
        return;
    curIndex++;
    httpClient->netList(folderPath.at(curIndex)->ID,pageNum,pageSize, showDeleteFolder);
    curDirId = folderPath.at(curIndex)->ID;
    emit pathChanged(folderPath.mid(0,curIndex+1));
    if(curIndex >= (folderPath.count()-1))
        emit historyEnable(true, false);
    else
        emit historyEnable(true, true);
}

void FilesPanel::panelSearch(int searchType, QString name)
{
    if(searchType == 0)
    {
        httpClient->netList(curDirId, pageNum, pageSize, showDeleteFolder, name, QString());
    }
    else
        httpClient->netList(curDirId, pageNum, pageSize, showDeleteFolder, name, QString::number(searchType));
}

//创建分享链接
void FilesPanel::panelShare()
{
    httpClient->getUserOrgList();
}

void FilesPanel::pathRefresh()
{
    emit pathChanged(folderPath);
}

void FilesPanel::creatShareLink()
{
    httpClient->netCreatShareLinks(getSelectFid());
}

void FilesPanel::deleteSelectedFiles()
{
    httpClient->netDelete(getSelectFid());
}

void FilesPanel::restoreSelectedFiles()
{
    httpClient->netFilesRestore(getSelectFid());
}

void FilesPanel::downloadSelectedFiles()
{
    httpClient->netFilesDownload(getSelectDirs(), getSelectFiles());
}

QStringList FilesPanel::getSelectFid()
{
    int i = 0;
    QStringList l;

    for(i=0; i<checkList.count(); i++)
    {
        if(checkList.at(i)->box->isChecked())
        {
            l<<QString::number(checkList.at(i)->fInfo->ID);
            qDebug()<<checkList.at(i)->fInfo->ID;
        }
    }
    return l;
}

QList<fileInfo*> FilesPanel::getSelectFiles()
{
    int i = 0;
    QList<fileInfo*> l;

    for(i=0; i<checkList.count(); i++)
    {
        if(checkList.at(i)->box->isChecked() && (!checkList.at(i)->fInfo->EXT.isEmpty()))
        {
            l<<new fileInfo(*(checkList.at(i)->fInfo));
            qDebug()<<"[files]"<<checkList.at(i)->fInfo->ID;
        }
    }
    return l;
}

QList<fileInfo*> FilesPanel::getSelectDirs()
{
    int i = 0;
    QList<fileInfo*> l;

    for(i=0; i<checkList.count(); i++)
    {
        if(checkList.at(i)->box->isChecked() && (checkList.at(i)->fInfo->EXT.isEmpty()))
        {
            l<<new fileInfo(*(checkList.at(i)->fInfo));
            qDebug()<<"[dirs]"<<checkList.at(i)->fInfo->ID;
        }
    }

    return l;
}

bool FilesPanel::repeatCheck(QString *fName, QFolder* pFolder)
{
    int i;

    for(i=0; i<curPanel.count(); i++)
    {
        if((*fName == curPanel.at(i)->fileName()) && (curPanel.at(i) != pFolder))
        {qDebug("repeat");
            *fName += "（复件）";
            return true;
        }
    }

    return false;
}

double FilesPanel::getCurId()
{
    return curDirId;
}
/***↑↑↑↑公有接口↑↑↑↑***/

//事件
void FilesPanel::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FilesPanel::resizeEvent(QResizeEvent*)
{qDebug()<<"[resizeEvent]";
    if(resizeEventEnable)
    {
        isResize = true;
        panelShow(curPanel);
        isResize = false;
    }
}

void FilesPanel::pathClear()
{
//    while()
}

short FilesPanel::folderTypeJudge(QString fName, bool isDir)
{
    if(isDir)
    {
        if(fName == "共享文件夹")
            return QFolder::DIR_SHARE;
        else if(fName == "私密文件")
            return QFolder::DIR_LOCK;
        else if(fName == "我的收藏")
            return QFolder::DIR_ENSHRINE;
        else
            return QFolder::DIR_COMMON;
    }
    else
    {
        if(fName.endsWith(".apk", Qt::CaseInsensitive))
            return QFolder::FILE_APK;
        else if(fName.endsWith(".doc", Qt::CaseInsensitive) || fName.endsWith(".docx"))
            return QFolder::FILE_DOC;
        else if(fName.endsWith(".mp3", Qt::CaseInsensitive))
            return QFolder::FILE_MP3;
        else if(fName.endsWith(".mp4", Qt::CaseInsensitive))
            return QFolder::FILE_MP4;
        else if(fName.endsWith(".pdf", Qt::CaseInsensitive))
            return QFolder::FILE_PDF;
        else if(fName.endsWith(".ppt", Qt::CaseInsensitive))
            return QFolder::FILE_PPT;
        else if(fName.endsWith(".rar", Qt::CaseInsensitive))
            return QFolder::FILE_RAR;
        else if(fName.endsWith(".txt", Qt::CaseInsensitive))
            return QFolder::FILE_TXT;
        else if(fName.endsWith(".xls", Qt::CaseInsensitive) || fName.endsWith("xlsx", Qt::CaseInsensitive))
            return QFolder::FILE_XLS;
        else if(fName.endsWith(".zip", Qt::CaseInsensitive))
            return QFolder::FILE_ZIP;
        else
            return QFolder::FILE_DEFAULT;
    }
}

void FilesPanel::setTableState(bool listState)
{
    if(listState)
    {
        ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->listView->setColumnCount(5);
        ui->listView->horizontalHeader()->setVisible(true);
        ui->listView->horizontalHeader()->setHighlightSections(false);

        ui->listView->setHorizontalHeaderItem(0,new QTableWidgetItem(""));
        ui->listView->setHorizontalHeaderItem(1,new QTableWidgetItem("文件名"));
        ui->listView->setHorizontalHeaderItem(2,new QTableWidgetItem("大小"));
        ui->listView->setHorizontalHeaderItem(3,new QTableWidgetItem("共享者"));
        ui->listView->setHorizontalHeaderItem(4,new QTableWidgetItem("修改时间"));
        ui->listView->horizontalHeaderItem(1)->setTextAlignment(Qt::ElideLeft);
        ui->listView->horizontalHeaderItem(2)->setTextAlignment(Qt::ElideLeft);
        ui->listView->horizontalHeaderItem(3)->setTextAlignment(Qt::ElideLeft);
        ui->listView->horizontalHeaderItem(4)->setTextAlignment(Qt::ElideLeft);
        ui->listView->hide();

        //列表视图槽
        connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(listViewClicked(QModelIndex)));
        connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(listViewCd(QModelIndex)));
    }
    else
    {
        ui->listView->horizontalHeader()->setVisible(false);
        ui->listView->setSelectionBehavior(QAbstractItemView::SelectItems);
        disconnect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(listViewCd(QModelIndex)));
    }
}

void FilesPanel::showList(QList<QFolder *> fPanel)
{
    int i = 0;
    ui->listView->setRowCount(fPanel.count());
    //列表视图
    int wid = this->geometry().width();
    if(wid<1000)
    {
        ui->listView->setColumnWidth(0,25);
        ui->listView->setColumnWidth(1,436);
        ui->listView->setColumnWidth(2,100);
        ui->listView->setColumnWidth(3,100);
        ui->listView->setColumnWidth(4,217);
    }
    else
    {
        ui->listView->setColumnWidth(0,25);
        ui->listView->setColumnWidth(1,436*2);
        ui->listView->setColumnWidth(2,100*2);
        ui->listView->setColumnWidth(3,100*2);
        ui->listView->setColumnWidth(4,217*2);
    }

//    qDebug()<<wid;
//    qDebug()<<ui->listView->columnWidth(0)<<ui->listView->columnWidth(1)<<ui->listView->columnWidth(2)<<ui->listView->columnWidth(3)<<ui->listView->columnWidth(4);

    if(!isResize)
        checkListClear();
    else
        return;

    if(fPanel.isEmpty())
    {
        emit isLoading(false);
        ui->listView->show();
        return;
    }

    emit isLoading(false);

    for(i=0; i<fPanel.count(); i++)
    {
        fPanel.at(i)->hide();
        ListRowWidgets* _row = new ListRowWidgets(fPanel.at(i)->info(),i);
        connect(_row, SIGNAL(selectState(bool,int)), this, SLOT(rowSelected(bool,int)));
        checkList<<_row;
        ui->listView->setCellWidget(i,0,_row->slctBox);
        ui->listView->setItem(i,1,_row->file);
        ui->listView->setItem(i,2,_row->fileSize);
        ui->listView->setItem(i,3,_row->shareName);
        ui->listView->setItem(i,4,_row->Modifytime);
    }
    this->setMinimumHeight(410);
    ui->listView->show();
}

void FilesPanel::showFolder(QList<QFolder *> fPanel)
{
    int i=0;
    int j=0;
    int k=0;
    int offset_x = 15;
    int offset_y = 15;

    if(!isResize)
        checkListClear();
    else
        return;

    if(fPanel.isEmpty())
    {
        emit isLoading(false);
        return;
    }
    emit isLoading(false);
    int ele_wid = fPanel.at(i)->geometry().width() + offset_x;
    int ele_hei = fPanel.at(i)->geometry().height() + offset_y;
    int count_x = (this->geometry().width() - offset_x)/(ele_wid);
    int count_y = (fPanel.count()/count_x) + (fPanel.count()%count_x != 0);
//    int panelSize_h = ((fPanel.count()/count_x) + (fPanel.count()%count_x != 0))*ele_hei + offset_y;
//    this->setMinimumHeight(panelSize_h);

    qDebug()<<"folder count"<<fPanel.count()<<count_x<<count_y;
    setTableState(false);
    ui->listView->clearContents();
    ui->listView->setColumnCount(count_x);
    ui->listView->setRowCount(count_y);


    for(i=0; i<count_x; i++)
        ui->listView->setColumnWidth(i, ele_wid);

    for(i=0; i<count_y; i++)
        ui->listView->setRowHeight(i, ele_hei);
    ui->listView->show();

    k=0;
    i=0;

    for(i=0; i<(count_y-1); i++)
    {
        for(j=0; j<count_x; j++)
        {
            ui->listView->setCellWidget(i, j, fPanel.at(k));
            k++;
        }
    }

    //最后一行
    for(j=0; j<(fPanel.count()%count_x); j++)
    {
        if(k>=fPanel.count())
            break;
        qDebug()<<"[show]"<<i<<j<<k<<fPanel.count()%count_x;
        ui->listView->setCellWidget(i, j, fPanel.at(k));
        k++;
    }

}

void FilesPanel::checkListClear()
{
    ui->listView->clearContents();
    while(!checkList.isEmpty())
    {
        ListRowWidgets* c = checkList.takeFirst();
        delete c;
    }
}

//菜单
void FilesPanel::contextMenuEvent(QContextMenuEvent*)
{//qDebug("menu");
    QCursor cur = this->cursor();
    QMenu* smenu = new QMenu(this);
    QList<QAction*> acts;
    if(!pasteEnable)
        acts<<act_new<<act_upload<<act_refresh<<act_sort;
    else
        acts<<act_new<<act_upload<<act_paste<<act_refresh<<act_sort;

    smenu->addActions(acts);

    smenu->exec(cur.pos());
}

/***公有槽***/
void FilesPanel::cmdCd(double id)
{
    panelCd(id);
}

void FilesPanel::showDelete(bool show)
{
    qDebug()<<"show delete"<<show;
    showDeleteFolder = show;
    panelRefresh();
}

//菜单槽
void FilesPanel::fileNew()
{
//    QScrollBar* bar = ui->listView->verticalScrollBar();
//    bar->setValue(bar->maximum());
//    if(showListView)
//    {
//        QScrollBar* bar = ui->listView->verticalScrollBar();
//        bar->setValue(bar->maximum());
//    }
//    else
//    {
        pFolder = new QFolder(this);
        curPanel<<pFolder;
        panelShow(curPanel);
        pFolder->newfolder();
    //    folderPath<<pFolder;
    //    ftpClient.ftpMkdir(getCurPath()+);
//    }
}

void FilesPanel::fileRefresh()
{
    httpClient->netSync(SYNC_ID, diskSync->getTable()->syncTime);
    panelRefresh();
}

void FilesPanel::fileSort()
{
    qDebug("sort");
}

void FilesPanel::rowSelected(bool state, int)
{
//    if(ui->listView->item(index,1)->isSelected() && state)
//        return;

//    if(ui->listView->selectionMode() != QAbstractItemView::MultiSelection)
//        ui->listView->setSelectionMode(QAbstractItemView::MultiSelection);
    emit hasSelected(state);
//    ui->listView->setItemSelected(ui->listView->item(index,0), state);
}

void FilesPanel::showEvent(QShowEvent *event)
{
//    emit pathChanged(folderPath);
    QWidget::showEvent(event);
}

void FilesPanel::fileUpload()
{
    QString upFile = QFileDialog::getOpenFileName(this, tr("上传文件"), "./");
    QFileInfo info = QFileInfo(upFile);
    QString fName = info.filePath();

    if(upFile.length() == 0)
        return;

    httpClient->netUpload(fName, curDirId);
}

/***私有槽***/
void FilesPanel::fileDownload(fileInfo info)
{
    httpClient->netDownload(info);
}

void FilesPanel::httpGetListInfo(QList<fileInfo*> lInfo)
{
    qDebug()<<"httpGetListInfo";
    panelClear();
    for(int i=0; i<lInfo.count(); i++)
    {
        fileInfo* info = lInfo.at(i);
        pFolder = new QFolder(this, info);
        curPanel<<pFolder;
    }

    panelShow(curPanel);
    scrollValueChanged(0);
    if(showListView)
        ui->listView->verticalScrollBar()->setValue(0);
    qDebug()<<curPanel.count();
}

void FilesPanel::review()
{
    resizeEventEnable = true;
}

void FilesPanel::listViewCd(QModelIndex index)
{
    panelCd(checkList.at(index.row())->fInfo);
}

void FilesPanel::listViewClicked(QModelIndex index)
{
    qDebug()<<"[listViewClicked]"<<index.row();
//    int i = 0;
    if(ui->listView->selectionMode() != QAbstractItemView::SingleSelection)
    {
        ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    }

    for(int i=0; i<ui->listView->rowCount(); i++)
    {
//            ui->listView->setItemSelected(ui->listView->item(i,1), false);
        checkList.at(i)->clearSelectState();
    }
    ui->listView->item(index.row(), 1)->setSelected(true);
    QWidget* wdg = (QWidget*)ui->listView->cellWidget(index.row(),0);
    QCheckBox* box = (QCheckBox*)wdg->children().at(1);
    box->setChecked(true);
}

void FilesPanel::getShareLink(QString link, QString passwd)
{
    ShareLink* winShareLink = new ShareLink(link,passwd);
    winShareLink->show();
}


