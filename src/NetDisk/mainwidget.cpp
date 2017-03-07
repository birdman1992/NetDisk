#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPushButton>
#include <QLineEdit>
#include "filespanel.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    wMoveable = false;

    //侧边栏
    initSilidebar();

    //功能栏
    initFunctionList();

    //传输列表
    transList = new TransList(this);

    //路径面板
    pathView = new PathView(this);
    pathView->setMaximumHeight(49);
    pathView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    ui->pathLayout->addWidget(pathView);

    //文件面板
    scrollFolder = new QScrollArea(this);
    scrollFolder->setFrameShape(QFrame::NoFrame);
    scrollFolder->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollFolder->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollFolder->setWidgetResizable(true);
    ui->panelLayout->addWidget(scrollFolder);
    ui->panelLayout->addWidget(transList);
    transList->hide();

    diskPanel = new FilesPanel(this);
    scrollFolder->setWidget(diskPanel);

    //信号槽
    connect(diskPanel, SIGNAL(pathChanged(QList<fileInfo*>)), pathView, SLOT(pathChange(QList<fileInfo*>)));
    connect(diskPanel, SIGNAL(historyEnable(bool,bool)), this, SLOT(historyEnabled(bool,bool)));
    connect(ui->showDelete, SIGNAL(toggled(bool)), diskPanel, SLOT(showDelete(bool)));
    connect(pathView, SIGNAL(cdRequest(double)), diskPanel, SLOT(cmdCd(double)));
    diskPanel->panelCd((fileInfo*)NULL);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::initSilidebar()
{
    QSize isize = QSize(81,81);
    ui->sliderbar->setIconSize(QSize(36,36));
    QListWidgetItem* item = new QListWidgetItem(QIcon(":/imgs/slidebar/我的文件.png"),"我的文件");
    item->setTextAlignment(Qt::AlignHCenter);
    item->setSizeHint(isize);
    ui->sliderbar->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/slidebar/他人共享.png"),"他人共享");
    item->setTextAlignment(Qt::AlignHCenter);
    item->setSizeHint(isize);
    ui->sliderbar->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/slidebar/链接管理.png"),"链接管理");
    item->setTextAlignment(Qt::AlignHCenter);
    item->setSizeHint(isize);
    ui->sliderbar->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/slidebar/文件同步.png"),"文件同步");
    item->setTextAlignment(Qt::AlignJustify);
    item->setSizeHint(isize);
    ui->sliderbar->addItem(item);

    ui->sliderbar->setCurrentRow(0);
}

void MainWidget::initFunctionList()
{
    QListWidgetItem* item;
    QSize itemSize = QSize(77,47);
//    ui->functionList->setBatchSize();
    ui->functionList->setIconSize(QSize(47,47));

    item = new QListWidgetItem(QIcon(":/imgs/function/+-上传.png"),"上传");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-新建文件夹.png"),"新建文件夹");
    item->setSizeHint(QSize(110,47));
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-共享.png"),"共享");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-链接分享.png"),"链接分享");
    item->setSizeHint(QSize(90,47));
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-下载.png"),"下载");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-删除.png"),"删除");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/function/+-还原.png"),"还原");
    item->setSizeHint(itemSize);
    ui->functionList->addItem(item);

    ui->searchFilter->addItem(" 全部");
    ui->showDelete->setText("显示已删文件");
    ui->search->setTextMargins(5,0,0,0);

    connect(ui->functionList, SIGNAL(clicked(QModelIndex)), this, SLOT(functionBtnClicked(QModelIndex)));
//    connect(ui->functionList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(founctionListClicked(QListWidgetItem*)));
}

//鼠标事件
void MainWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        QRect rect = ui->title->frameRect();
        if(rect.contains(event->pos()))
        {
            wMoveable = true;
        }
        event->accept();
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && wMoveable)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent*)
{
    if(wMoveable)
    {
        wMoveable = false;
    }
}

void MainWidget::on_wMin_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWidget::on_wClose_clicked()
{
    this->close();
}

void MainWidget::on_wMax_toggled(bool checked)
{
    if(checked)
    {
        this->setWindowState(Qt::WindowMaximized);
    }
    else
    {
        this->setWindowState(Qt::WindowNoState);

    }
}

void MainWidget::founctionListClicked(QListWidgetItem*)
{

}

void MainWidget::functionBtnClicked(QModelIndex index)
{
    int funcNum = index.row();

    switch (funcNum)
    {
        case 0:
            diskPanel->fileUpload(); break;
        case 1:
            diskPanel->fileNew(); break;
        case 2:
            qDebug("share");break;
        case 3:
            qDebug("link");break;
        case 4:
            qDebug("download");break;
        case 5:
            qDebug("delete");break;
        case 6:
            qDebug("restore");break;

        default:
            break;
    }
}

void MainWidget::historyEnabled(bool backEnable, bool aheadEnable)
{
    ui->back->setEnabled(backEnable);
    ui->forward->setEnabled(aheadEnable);
}

void MainWidget::on_back_clicked()
{
    diskPanel->panelBack();
}

void MainWidget::on_forward_clicked()
{
    diskPanel->panelAhead();
}

void MainWidget::on_refresh_clicked()
{
    diskPanel->panelRefresh();
}

void MainWidget::on_translist_toggled(bool checked)
{qDebug()<<"checked"<<checked;
    if(checked)
    {
        scrollFolder->hide();
        transList->show();
    }
    else
    {
        scrollFolder->show();
        transList->hide();
    }
}
