#include "qfolder.h"
#include "ui_qfolder.h"
#include <QCursor>
#include <QtDebug>
#include <QMenu>

QFolder::QFolder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFolder)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

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

    connect(act_rename, SIGNAL(triggered()), this, SLOT(folderRename()));
    connect(this, SIGNAL(nameFocus()), this, SLOT(nameFocused()));

}

QFolder::~QFolder()
{
    delete ui;
}

void QFolder::focusInEvent(QFocusEvent* e)
{
    if(e == NULL)
        return;

    ui->name->setFocusPolicy(Qt::StrongFocus);
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
    menu->addAction(act_rename);
    menu->exec(cur.pos());
}

void QFolder::folderRename()
{
    ui->name->setFocus();
}

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
            ui->name->setCursor(QCursor(Qt::IBeamCursor));
            emit nameFocus();
        }
        else if(e->type() == QEvent::FocusOut)
        {
            ui->name->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
    return QWidget::eventFilter(watched,e);
}





