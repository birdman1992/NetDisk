#include "qfolder.h"
#include "ui_qfolder.h"
#include <QCursor>
#include <QtDebug>
#include <QMenu>
#include <QPainter>

QFolder::QFolder(QWidget *parent,short _type,QString fName) :
    QWidget(parent),
    ui(new Ui::QFolder)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    //初始化属性
    fType = _type;
    pasteEnable = false;
    folderName = fName;
    ui->name->setText(folderName);

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

    connect(this, SIGNAL(nameFocus()), this, SLOT(nameFocused()));
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

/**********************
SLOT
**********************/

void QFolder::nameFocused()
{qDebug("rename");
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
            ui->name->selectAll();
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
        {
            ui->name->selectAll();
            selectEnable = false;
        }
    }
    return QWidget::eventFilter(watched,e);
}

void QFolder::editFinish()
{
    this->setFocus();
}

void QFolder::rename()
{
    ui->name->setFocus();
}

//右键菜单槽
void QFolder::folderRename()
{
    ui->name->setFocus();
}

void QFolder::folderOpen()
{
    qDebug("open");
}

void QFolder::folderCopy()
{
    qDebug("copy");
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
    qDebug("delete");
}

void QFolder::folderDownload()
{
    qDebug("download");
}


