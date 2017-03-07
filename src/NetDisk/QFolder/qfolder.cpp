#include "qfolder.h"
#include "ui_qfolder.h"
#include <QCursor>
#include <QtDebug>
#include <QMenu>
#include <QPainter>
#include <QToolTip>
#include <QTableWidget>
#include "filespanel.h"

QFolder::QFolder(QWidget *parent,fileInfo* info) :
    QWidget(parent),
    ui(new Ui::QFolder)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    //初始化属性
//    fType = _type;
    if(info)
        folderInfo = fileInfo(*info);
    else
    {
        folderInfo.FILE_NAME = "新建文件夹";
    }

    fType = folderTypeJudge(folderInfo.EXT);
    pasteEnable = false;
    newfile = false;
    folderName = folderInfo.FILE_NAME;
    ui->name->setText(folderName);
    textwid = this->geometry().width()/8;
    setImg();
    ui->name->setText(folderNameCut(ui->name->text(),12));
//    this->setToolTip("sda");

    //初始化右键菜单项
    menu = new QMenu(this);
    act_copy = new QAction(tr("复制"),this);
    act_cut = new QAction(tr("剪切"),this);
    act_delete = new QAction(tr("删除"),this);
    act_download = new QAction(tr("下载"),this);
    act_open = new QAction(tr("打开"),this);
    act_paste = new QAction(tr("粘贴"),this);
    act_rename = new QAction(tr("重命名"),this);

    act_copy->setShortcut(QKeySequence::Copy);
    act_paste->setShortcut(QKeySequence::Paste);

    ui->name->setFocusPolicy(Qt::NoFocus);
    ui->name->setCursor(QCursor(Qt::ArrowCursor));
    ui->name->installEventFilter(this);

    connect(ui->name, SIGNAL(editingFinished()), this, SLOT(editFinish()));
    connect(act_rename, SIGNAL(triggered()), this, SLOT(folderRename()));
    connect(act_open, SIGNAL(triggered()), this, SLOT(folderOpen()));
    connect(act_copy, SIGNAL(triggered()), this, SLOT(folderCopy()));
    connect(act_cut, SIGNAL(triggered()), this, SLOT(folderCut()));
    connect(act_delete, SIGNAL(triggered()), this, SLOT(folderDelete()));
    connect(act_paste, SIGNAL(triggered()), this, SLOT(folderPaste()));
    connect(act_download, SIGNAL(triggered()), this, SLOT(folderDownload()));
}

QFolder::~QFolder()
{
    delete ui;
}

/*****************
EVENT
*****************/
void QFolder::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void QFolder::focusInEvent(QFocusEvent* e)
{
    if(e == NULL)
        return;

    this->setStyleSheet("QWidget#QFolder:focus{\
                        border:2px solid rgb(171,214,234);\
                        background-color: rgb(230,243,252);\
                        }");
    ui->name->setFocusPolicy(Qt::ClickFocus);
    this->addAction(act_copy);
    this->addAction(act_paste);
}

void QFolder::focusOutEvent(QFocusEvent* e)
{
    if(e == NULL)
        return;

    this->setStyleSheet("QWidget#QFolder:hover{\
                        border:2px solid rgb(217,242,253);\
                        }");
    ui->name->setFocusPolicy(Qt::NoFocus);
    this->removeAction(act_copy);
    this->removeAction(act_paste);
}

void QFolder::contextMenuEvent(QContextMenuEvent*)
{
    QCursor cur = this->cursor();
    menu->clear();
    QList<QAction*> acts;
    if(pasteEnable)
        acts<<act_open<<act_download<<act_copy<<act_cut<<act_paste<<act_delete<<act_rename;
    else
        acts<<act_open<<act_download<<act_copy<<act_cut<<act_delete<<act_rename;

//    act_download->setIcon(QIcon(":/imgs/item.ico"));
    menu->addActions(acts);
    menu->insertSeparator(act_download);


    menu->exec(cur.pos());
}

void QFolder::mouseDoubleClickEvent(QMouseEvent*)
{
    folderOpen();
}

bool QFolder::event(QEvent *event)
{
    if(event->type() == QEvent::ToolTip)
    {
        QToolTip::showText(this->cursor().pos(),QString("名称:%1\n修改日期:%2")
                 .arg(folderName).arg(folderTime.toString("yyyy-MM-dd \nhh:mm:ss dddd")));
    }
    return QWidget::event(event);
}

QString QFolder::folderNameCut(QString strIn, int cutLength)
{
    if(strIn.toLocal8Bit().length() > cutLength)
        while(strIn.toLocal8Bit().length() > cutLength)
            strIn = strIn.left(strIn.length()-1);
    else
        return strIn;

    strIn += "...";
//    qDebug()<<"Folder name cut:"<<strIn;
    return strIn;
}

void QFolder::setImg()
{
    QString qss;
    switch(fType)
    {
        case DIR_COMMON:
            qss = QString("#img{border-image:url(\":/imgs/60x60/普通文件夹.png\")}");break;
        case DIR_ENSHRINE:
            qss = QString("#img{border-image:url(\":/imgs/60x60/收藏文件夹.png\")}");break;
        case DIR_LOCK:
            qss = QString("#img{border-image:url(\":/imgs/60x60/锁定文件夹.png\")}");break;
        case DIR_SHARE:
            qss = QString("#img{border-image:url(\":/imgs/60x60/共享文件夹.png\")}");break;

        case FILE_APK:
            qss = QString("#img{border-image:url(\":/imgs/60x60/apk.png\")}");break;
        case FILE_DOC:
            qss = QString("#img{border-image:url(\":/imgs/60x60/doc.png\")}");break;
        case FILE_MP3:
            qss = QString("#img{border-image:url(\":/imgs/60x60/mp3.png\")}");break;
        case FILE_MP4:
            qss = QString("#img{border-image:url(\":/imgs/60x60/mp4.png\")}");break;
        case FILE_PDF:
            qss = QString("#img{border-image:url(\":/imgs/60x60/pdf.png\")}");break;
        case FILE_PPT:
            qss = QString("#img{border-image:url(\":/imgs/60x60/ppt.png\")}");break;
        case FILE_RAR:
            qss = QString("#img{border-image:url(\":/imgs/60x60/rar.png\")}");break;
        case FILE_TXT:
            qss = QString("#img{border-image:url(\":/imgs/60x60/txt.png\")}");break;
        case FILE_XLS:
            qss = QString("#img{border-image:url(\":/imgs/60x60/xls.png\")}");break;
        case FILE_ZIP:
            qss = QString("#img{border-image:url(\":/imgs/60x60/zip.png\")}");break;
        case FILE_DEFAULT:
            qss = QString("#img{border-image:url(\":/imgs/60x60/缺省文件格式.png\")}");break;
        default:break;
    }
    ui->img->setStyleSheet(qss);
}

short QFolder::folderTypeJudge(QString fName)
{
    if(fName.isEmpty())
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
        if(fName.endsWith("apk", Qt::CaseInsensitive))
            return QFolder::FILE_APK;
        else if(fName.endsWith("doc", Qt::CaseInsensitive) || fName.endsWith("docx"))
            return QFolder::FILE_DOC;
        else if(fName.endsWith("mp3", Qt::CaseInsensitive))
            return QFolder::FILE_MP3;
        else if(fName.endsWith("mp4", Qt::CaseInsensitive))
            return QFolder::FILE_MP4;
        else if(fName.endsWith("pdf", Qt::CaseInsensitive))
            return QFolder::FILE_PDF;
        else if(fName.endsWith("ppt", Qt::CaseInsensitive))
            return QFolder::FILE_PPT;
        else if(fName.endsWith("rar", Qt::CaseInsensitive))
            return QFolder::FILE_RAR;
        else if(fName.endsWith("txt", Qt::CaseInsensitive))
            return QFolder::FILE_TXT;
        else if(fName.endsWith("xls", Qt::CaseInsensitive) || fName.endsWith("xlsx", Qt::CaseInsensitive))
            return QFolder::FILE_XLS;
        else if(fName.endsWith("zip", Qt::CaseInsensitive))
            return QFolder::FILE_ZIP;
        else
            return QFolder::FILE_DEFAULT;
    }
}

/**********************
SLOT
**********************/

void QFolder::nameFocused()
{
    ui->name->selectAll();
}

bool QFolder::eventFilter(QObject *watched,QEvent *e)
{
    if(watched == ui->name)
    {
        if(e->type() == QEvent::FocusIn)
        {
            this->setStyleSheet("QWidget#QFolder{\
                                border:2px solid rgb(171,214,234);\
                                background-color: rgb(230,243,252);\
                                }");
            ui->name->setStyleSheet("QLineEdit#name:focus {\
                                    border: 2px solid rgb(200, 200, 200);\
                                    background:rgb(255, 255, 255);\
                                    }");
            ui->name->setCursor(QCursor(Qt::IBeamCursor));
//            ui->name->selectAll();
            selectEnable = true;
        }
        else if(e->type() == QEvent::FocusOut)
        {
            this->setStyleSheet("QWidget#QFolder:hover{\
                        border:2px solid rgb(217,242,253);\
                        }");
            ui->name->setStyleSheet("QLineEdit#name{\
                                    border: 0px solid;\
                                    background: rgba(255,255,255,0);\
                                    }");
            ui->name->setCursor(QCursor(Qt::ArrowCursor));
            selectEnable = true;
        }
        else if((e->type() == QEvent::MouseButtonRelease) && selectEnable)
        {qDebug("select");
            ui->name->setText(folderName);
            ui->name->selectAll();
            selectEnable = false;
        }
    }
    return QWidget::eventFilter(watched,e);
}

void QFolder::editFinish()
{
    FilesPanel* p = (FilesPanel*)parent();

    if(newfile)
    {
        folderName = ui->name->text();qDebug()<<"newfile"<<folderName;
        while(p->repeatCheck(&folderName,this));
        p->httpClient->netMkdir(p->getCurId(),folderName);
//        p->ftpClient.ftpMkdir(p->getCurPath() + folderName);
//        ui->name->setText(folderNameCut(ui->name->text(),12));
        newfile = false;
    }
    else if(folderName != ui->name->text())
    {
//        p->ftpClient.ftpRename(p->getCurPath() + folderName, p->getCurPath() + ui->name->text());
        qDebug()<<"rename:"<<folderName;
        folderName = ui->name->text();
        ui->name->setText(folderNameCut(ui->name->text(),12));
    }
    else
        ui->name->setText(folderNameCut(ui->name->text(),12));
    this->setFocus();
}

void QFolder::newfolder()
{
    newfile = true;
    ui->name->setFocus();
    ui->name->selectAll();
}

void QFolder::setParFolder(QFolder *par)
{
    pardir = par;
}

void QFolder::setFolderTime(QDateTime fTime)
{
    folderTime = fTime;
}

short QFolder::folderType()
{
    return fType;
}

fileInfo *QFolder::info()
{
    return &folderInfo;
}

QString QFolder::fileName()
{
    return folderName;
}

//右键菜单槽
void QFolder::folderRename()
{
    ui->name->setFocus();qDebug()<<"sdadw"<<folderName;
    ui->name->setText(folderName);
    ui->name->selectAll();
}

void QFolder::folderOpen()
{
    qDebug("open");
    FilesPanel* par = (FilesPanel*)parent();
    par->panelCd(&folderInfo);
}

void QFolder::folderCopy()
{
    qDebug("copy");
//    FilesPanel* par = (FilesPanel*)parent();
//    QString selfPath = par->getCurId();
//    emit copy(selfPath);
    pasteEnable = true;
}

void QFolder::folderCut()
{
    qDebug("cut");
}

void QFolder::folderPaste()
{
    qDebug("paste");
}

void QFolder::folderDelete()
{
    FilesPanel* p = (FilesPanel*)parent();
    p->httpClient->netDelete(this->info()->ID);
}

void QFolder::folderDownload()
{
    FilesPanel* p = (FilesPanel*)parent();
    p->fileDownload(folderInfo);
}


