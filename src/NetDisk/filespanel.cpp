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
    pCdFolder = new QString(dir);
    ftpClient.ftpCd(*pCdFolder);
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
{qDebug("list info");
    QString str = QString::fromUtf8(info.name().toLatin1());
    pFolder = new QFolder(this,2,str);
     info.lastModified();
    curPanel<<pFolder;
    panelShow(curPanel);
}

void FilesPanel::ftpCdFinishi()
{
    if(pCdFolder == NULL)
        return;

    folderPath<<pCdFolder;
    pCdFolder = NULL;
    panelRefresh();
}




