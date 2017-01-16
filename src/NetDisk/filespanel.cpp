#include "filespanel.h"
#include "ui_filespanel.h"
#include "QFolder/qfolder.h"
#include <QPainter>

#define FOLDER_SIZE 100

FilesPanel::FilesPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilesPanel)
{
    ui->setupUi(this);
    pasteEnable = false;
    //主菜单
    menu = new QMenu(this);
    act_new = new QAction("新建文件夹");
    act_refresh = new QAction("刷新");
    act_sort = new QAction("排序");
    act_upload = new QAction("上传");
    connect(act_new, SIGNAL(triggered(bool)), this, SLOT(fileNew()));

    //排序子菜单
    menu_sort = new QMenu(this);
    act_filename = new QAction("文件名");
    act_filesize = new QAction("大小");
    act_filetime = new QAction("修改时间");
}

FilesPanel::~FilesPanel()
{
    delete ui;
}

void FilesPanel::panelShow(QList<QFolder*> fPanel)
{
    int i = 0;
    int j = 0;
    int offset_x = 20;
    int offset_y = 20;
    int ele_wid = fPanel.at(i)->geometry().width();
    int ele_hei = fPanel.at(i)->geometry().height();
    int count_x = (this->geometry().x() - offset_x)/ele_wid;

    qDebug("w:%d h:%d",this->geometry().x(),this->geometry().y());


    for(i=0; i<fPanel.count(); i)
    {

        fPanel.at(i)->move(offset_x + i % count_x,);
        fPanel.at(i)->show();
        i++;
    }
}

void FilesPanel::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//菜单
void FilesPanel::contextMenuEvent(QContextMenuEvent*)
{qDebug("menu");
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
    pFolder->rename();
}


