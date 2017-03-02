#include "filespanel.h"
#include "ui_filespanel.h"
#include "QFolder/qfolder.h"
#include <QDebug>
#include <QPainter>
#include <QFileDialog>

#define FOLDER_SIZE 100

FilesPanel::FilesPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilesPanel)
{
    ui->setupUi(this);
    pasteEnable = false;

    curPanel.clear();
    dirTree.clear();
    folderPath.clear();
    pFolder = NULL;
    curDirId = 0;
    pCdFolder = NULL;
    showListView = false;
    curDirId = -1;
    pageSize = 28;
    pageNum = 1;


    //http
    httpClient = new NetHttp(this);

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

    //FTP槽
//    connect(&ftpClient, SIGNAL(listInfo(QUrlInfo)), this, SLOT(ftpGetListInfo(QUrlInfo)));
//    connect(&ftpClient, SIGNAL(needRefresh()), this, SLOT(fileRefresh()));
//    connect(&ftpClient, SIGNAL(cmdCd()), this, SLOT(ftpCdFinishi()));

    //HTTP槽
    connect(httpClient, SIGNAL(listUpdate(QList<fileInfo*>)), this, SLOT(httpGetListInfo(QList<fileInfo*>)));
    connect(httpClient, SIGNAL(updateRequest()), this, SLOT(fileRefresh()));

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
    showListView = showList;
    panelRefresh();
}

//显示文件夹面板
void FilesPanel::panelShow(QList<QFolder*> fPanel)
{
    if(showListView)
    {

    }
    else
    {
        int i = 0;
        int j = 0;
        int offset_x = 15;
        int offset_y = 15;

        if(fPanel.isEmpty())
            return;

        int ele_wid = fPanel.at(i)->geometry().width() + offset_x;
        int ele_hei = fPanel.at(i)->geometry().height() + offset_y;
        int count_x = (this->geometry().width() - offset_x)/(ele_wid);

        qDebug("w:%d h:%d",this->geometry().width(),this->geometry().height());

        for(i=0; i<fPanel.count();)
        {
            fPanel.at(i)->move(offset_x + (i % count_x) * ele_wid, offset_y + j * ele_hei);
            fPanel.at(i)->show();
            i++;
            if(!(i%count_x))
                j++;
        }
//        httpClient->netList(curDirId, pageNum, pageSize);
        return;
    }

}

void FilesPanel::panelClear()
{
    QFolder* f;
    while(!curPanel.isEmpty())
    {
        f = curPanel.takeFirst();
        f->deleteLater();
    }
}

void FilesPanel::panelRefresh()
{
//    qDebug("refresh");
//    connect(&ftpClient, SIGNAL(cmdList()), this, SLOT(ftpListShow()));
//    panelClear();
    httpClient->netList(curDirId, pageNum, pageSize);
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
    int i = 0;
    bool del = false;
    qDebug()<<"FTP:cd";
    if(dir == NULL)
    {
        while(!folderPath.isEmpty())
        {
            fileInfo* f = folderPath.takeFirst();
            delete f;
        }
        curDirId = -1;
        curIndex = 0;
        httpClient->netList(curDirId,pageNum,pageSize);
        emit historyEnable(false, false);
        emit pathChanged(folderPath);
        return;
    }
    fileInfo* info = new fileInfo(*dir);

//    qDebug()<<"FTP:cd"<<dir->info()->ID;
    for(i=0; i<folderPath.count(); i++)
    {
        if(del)
        {
            fileInfo* f = folderPath.takeAt(i);
            delete f;
            continue;
        }
        if(folderPath.at(i)->ID == dir->ID)
            del = true;
    }
    if(!del)
        folderPath<<info;
    curDirId = dir->ID;
    curIndex = folderPath.count()-1;
    httpClient->netList(curDirId,pageNum,pageSize);
    emit historyEnable(true, false);
    emit pathChanged(folderPath);
}

void FilesPanel::panelCd(double dirId)
{
    int i = 0;
    bool del = false;
    fileInfo* f = NULL;
    qDebug()<<"FTP:cd"<<dirId;
    if(dirId == -1)
    {
        curDirId = -1;
        while(!folderPath.isEmpty())
        {
            f = folderPath.takeFirst();
            delete f;
        }
        curIndex = 0;
        httpClient->netList(curDirId,pageNum,pageSize);
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
    httpClient->netList(curDirId,pageNum,pageSize);
    qDebug("pathChanged");
    emit pathChanged(folderPath);
    emit historyEnable(true, false);
}

void FilesPanel::panelBack()
{
    if(curIndex < 0)
        return;
    if(curIndex == 0)
    {
        httpClient->netList(-1,pageNum,pageSize);
        curDirId = -1;
        curIndex--;
    }
    else
    {
        curIndex--;
        httpClient->netList(folderPath.at(curIndex)->ID,pageNum,pageSize);
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
    httpClient->netList(folderPath.at(curIndex)->ID,pageNum,pageSize);
    curDirId = folderPath.at(curIndex)->ID;
    emit pathChanged(folderPath.mid(0,curIndex+1));
    if(curIndex >= (folderPath.count()-1))
        emit historyEnable(true, false);
    else
        emit historyEnable(true, true);
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



/***私有槽***/
//菜单槽
void FilesPanel::fileNew()
{
    pFolder = new QFolder(this);
    curPanel<<pFolder;
    panelShow(curPanel);
    pFolder->newfolder();
//    folderPath<<pFolder;
//    ftpClient.ftpMkdir(getCurPath()+);
}

void FilesPanel::fileRefresh()
{
    panelRefresh();
}

void FilesPanel::fileSort()
{
    qDebug("sort");
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

void FilesPanel::fileDownload(QString fileName, double fileId)
{
    httpClient->netDownload(fileName, fileId);
}

void FilesPanel::httpGetListInfo(QList<fileInfo*> lInfo)
{
//    disconnect(httpClient, SIGNAL(listUpdate(QList<fileInfo*>)), this, SLOT(httpGetListInfo(QList<fileInfo*>)));
    panelClear();
    for(int i=0; i<lInfo.count(); i++)
    {
        fileInfo* info = lInfo.at(i);
        pFolder = new QFolder(this, info);
        curPanel<<pFolder;
    }
    panelShow(curPanel);
    qDebug()<<curPanel.count();
//    pFolder = new QFolder(this,folderTypeJudge(lInfo,0),str);
//    pFolder->setFolderTime(info.lastModified());
//    if(info.isDir())
//        curPanel.insert(0, pFolder);
//    else
//        curPanel<<pFolder;
}

void FilesPanel::ftpListShow()
{                                     
//    disconnect(&ftpClient, SIGNAL(cmdList()), this, SLOT(ftpListShow()));
//    emit pathChanged(folderPath, curPanel);
//    panelShow(curPanel);
}

void FilesPanel::ftpCdFinishi()
{
//    if(pCdFolder == NULL)
//        return;

//    while(!folderPath.isEmpty())
//    {
//        QString* str = folderPath.takeFirst();
//        delete str;
//    }

//    QStringList l = pCdFolder->split('/',QString::SkipEmptyParts);
//    for(int i=0; i< l.count(); i++)
//    {
//        QString* str = new QString(l.at(i));
//        qDebug()<<*str;
//        folderPath<<str;
//    }

//    pCdFolder = NULL;
//    panelRefresh();
}




