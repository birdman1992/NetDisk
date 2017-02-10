#include "qfolder.h"
#include "ui_qfolder.h"
#include <QCursor>
#include <QtDebug>
#include <QMenu>
#include <QPainter>
#include <QToolTip>
#include <QTableWidget>
#include "filespanel.h"

QFolder::QFolder(QWidget *parent,short _type,QString fName) :
    QWidget(parent),
    ui(new Ui::QFolder)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    //初始化属性
    fType = _type;
    pasteEnable = false;
    newfile = false;
    folderName = fName;
    ui->name->setText(folderName);
    textwid = this->geometry().width()/8;
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
//    menu->setStyleSheet("QMenu{border:1px solid #A0A0A0;background-color:#F0F0F0;}\
//                        QMenu::item{color:#000000;padding-left:20px;}\
//                        QMenu::item:selected{background-color: #2dabf9;}\
//                        QMenu::separator{height:2px;margin-left:10px;margin-right:5px;}");

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
    qDebug()<<"Folder name cut:"<<strIn;
    return strIn;
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
        p->ftpClient.ftpMkdir(p->getCurPath() + folderName);
//        ui->name->setText(folderNameCut(ui->name->text(),12));
        newfile = false;
    }
    else if(folderName != ui->name->text())
    {
        p->ftpClient.ftpRename(p->getCurPath() + folderName, p->getCurPath() + ui->name->text());
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
    par->panelCd(par->getCurPath() + folderName);
}

void QFolder::folderCopy()
{
    qDebug("copy");
    FilesPanel* par = (FilesPanel*)parent();
    QString selfPath = par->getCurPath();
    emit copy(selfPath);
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
    QString delFile = p->getCurPath() + folderName+"/";
    qDebug()<<"delete:"<<delFile;
    p->ftpClient.ftpRmdir(delFile);
}

void QFolder::folderDownload()
{
    qDebug("download");
}


