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
    curDir = NULL;
    pCdFolder = NULL;

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
    connect(&ftpClient, SIGNAL(listInfo(QUrlInfo)), this, SLOT(ftpGetListInfo(QUrlInfo)));
    connect(&ftpClient, SIGNAL(needRefresh()), this, SLOT(fileRefresh()));
    connect(&ftpClient, SIGNAL(cmdCd()), this, SLOT(ftpCdFinishi()));

    //排序子菜单
    menu_sort = new QMenu(this);
    act_filename = new QAction("文件名");
    act_filesize = new QAction("大小");
    act_filetime = new QAction("修改时间");

    ftpClient.ftpLogin("test", "123456");
}

FilesPanel::~FilesPanel()
{
    delete ui;
}

/***↓↓↓↓公有接口↓↓↓↓***/
void FilesPanel::addFolder(QFolder* parFolder)
{
    curDir->setParFolder(parFolder);
}

//显示文件夹面板
void FilesPanel::panelShow(QList<QFolder*> fPanel)
{
    int i = 0;
    int j = 0;
    int offset_x = 20;
    int offset_y = 20;

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
    qDebug("refresh");
    connect(&ftpClient, SIGNAL(cmdList()), this, SLOT(ftpListShow()));
    panelClear();
    ftpClient.ftpList(getCurPath());
}

void FilesPanel::panelCopy(QFolder *p)
{
    pClipboard = p;
}

void FilesPanel::panelPaste()
{
    qDebug("paste");
}

void FilesPanel::panelCd(QString dir)
{
    qDebug()<<"FTP:cd"<<dir;

//    bool deletTail = false;

//    for(int i=0;i <folderPath.count(); i++)
//    {
//        if(dir == *(folderPath.at(i)))
//        {
//            deletTail = true;
//        }
//        if(deletTail)
//        {
//            QString* str = folderPath.takeAt(i);
//            delete str;
//            continue;
//        }
//    }

    pCdFolder = new QString(dir);
    ftpClient.ftpCd(dir);
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

QString FilesPanel::getCurPath()
{
    QString str;
    int i = 0;

    while(i<folderPath.count())
    {
        str += "/";
        str += folderPath.at(i);
        i++;
    }
    str += "/";
    qDebug()<<"current path:"<<str;
    return str;
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
void FilesPanel::cmdCd(QString dir)
{
    panelCd(dir);
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
    qDebug("upload");
    QString upFile = QFileDialog::getOpenFileName(this, tr("上传文件"), "./");
    QFileInfo info = QFileInfo(upFile);
    QString fName = info.fileName();

    if(upFile.length() == 0)
        return;

    ftpClient.ftpUpload(upFile, getCurPath() + fName);
//    panelRefresh();
//    qDebug()<<upFile<<(getCurPath() + fName);
}

void FilesPanel::ftpGetListInfo(QUrlInfo info)
{
    QString str = QString::fromUtf8(info.name().toLatin1());
    qDebug()<<"list info"<<folderTypeJudge(str,info.isDir());
    pFolder = new QFolder(this,folderTypeJudge(str,info.isDir()),str);
    pFolder->setFolderTime(info.lastModified());
    if(info.isDir())
        curPanel.insert(0, pFolder);
    else
        curPanel<<pFolder;
}

void FilesPanel::ftpListShow()
{
    disconnect(&ftpClient, SIGNAL(cmdList()), this, SLOT(ftpListShow()));
    emit pathChanged(folderPath, curPanel);
    panelShow(curPanel);
}

void FilesPanel::ftpCdFinishi()
{
    if(pCdFolder == NULL)
        return;

    while(!folderPath.isEmpty())
    {
        QString* str = folderPath.takeFirst();
        delete str;
    }

    QStringList l = pCdFolder->split('/',QString::SkipEmptyParts);
    for(int i=0; i< l.count(); i++)
    {
        QString* str = new QString(l.at(i));
        qDebug()<<*str;
        folderPath<<str;
    }

    pCdFolder = NULL;
    panelRefresh();
}




