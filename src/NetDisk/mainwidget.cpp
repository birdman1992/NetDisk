#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPushButton>
#include <QLineEdit>
#include <qscrollarea.h>
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

    //路径面板
    pathView = new PathView(this);
    pathView->setMaximumHeight(49);
    pathView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    ui->pathLayout->addWidget(pathView);

    //文件面板
    QScrollArea* scrollFolder = new QScrollArea(this);
    scrollFolder->setFrameShape(QFrame::NoFrame);
    scrollFolder->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollFolder->setWidgetResizable(true);
    ui->panelLayout->addWidget(scrollFolder);

    diskPanel = new FilesPanel(this);
    scrollFolder->setWidget(diskPanel);


    //信号槽
    connect(diskPanel, SIGNAL(pathChanged(QList<QString*>,QList<QFolder*>)), pathView, SLOT(pathChange(QList<QString*>,QList<QFolder*>)));
    connect(pathView, SIGNAL(cdRequest(QString)), diskPanel, SLOT(cmdCd(QString)));
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::initSilidebar()
{
    ui->sliderbar->setIconSize(QSize(50,45));
    QListWidgetItem* item = new QListWidgetItem(QIcon(":/imgs/slidebar/我的文件.ico"),"我的文件");
    item->setSizeHint(QSize(81,74));
    ui->sliderbar->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/slidebar/他人共享.ico"),"他人共享");
    item->setSizeHint(QSize(81,74));
    ui->sliderbar->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/slidebar/链接管理.ico"),"链接管理");
    item->setSizeHint(QSize(81,74));
    ui->sliderbar->addItem(item);
    item = new QListWidgetItem(QIcon(":/imgs/slidebar/文件同步.ico"),"文件同步");
    item->setSizeHint(QSize(81,74));
    ui->sliderbar->addItem(item);


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
