#include "qfolder.h"
#include "ui_qfolder.h"
#include <QCursor>
#include <QtDebug>
#include <QMenu>
#include <QPainter>
#include <unistd.h>

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
    act_copy = new QAction(tr("复制"),this);
    act_cut = new QAction(tr("剪切"),this);
    act_delete = new QAction(tr("删除"),this);
    act_download = new QAction(tr("下载"),this);
    act_open = new QAction(tr("打开"),this);
    act_paste = new QAction(tr("粘贴"),this);
    act_rename = new QAction(tr("重命名"),this);

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

    ui->name->setFocusPolicy(Qt::ClickFocus);
}

void QFolder::focusOutEvent(QFocusEvent* e)
{
    if(e == NULL)
        return;
    ui->name->setFocusPolicy(Qt::NoFocus);
}

void QFolder::contextMenuEvent(QContextMenuEvent*)
{
    QCursor cur = this->cursor();
    QMenu* menu = new QMenu(this);
    QList<QAction*> acts;
    if(pasteEnable)
        acts<<act_open<<act_download<<act_copy<<act_cut<<act_paste<<act_delete<<act_rename;
    else
        acts<<act_open<<act_download<<act_copy<<act_cut<<act_delete<<act_rename;

    menu->addActions(acts);

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
    usleep(1000000);
    ui->name->selectAll();
}

bool QFolder::eventFilter(QObject *watched,QEvent *e)
{
    if(watched == ui->name)
    {
        if(e->type() == QEvent::FocusIn)
        {qDebug("select");
            ui->name->setCursor(QCursor(Qt::IBeamCursor));
//            emit nameFocus();
            ui->name->selectAll();
        }
        else if(e->type() == QEvent::FocusOut)
        {
            ui->name->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
    return QWidget::eventFilter(watched,e);
}

void QFolder::editFinish()
{
    this->setFocus();
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


