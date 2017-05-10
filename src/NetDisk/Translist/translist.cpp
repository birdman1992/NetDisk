#include "translist.h"
#include "ui_translist.h"
#include <QtDebug>
#include <qstring.h>
#include <QStringList>

static QStringList tips;
static QList<QBrush*> brush;

TransList::TransList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransList)
{
    ui->setupUi(this);

    listUiInit();
//    transNum = 1;
//    progress = 1;
//    colIndex_state = 2;
//    colIndex_speed = 3;
//    colIndex_progress = 4;
//    tips<<"等待"<<"下载中"<<"上传中"<<"下载完成"<<"下载失败";
//    brush<<new QBrush(QColor(0, 0, 0))<<new QBrush(QColor(0, 0, 0))<<new QBrush(QColor(0, 0, 0))<<new QBrush(QColor(0, 0, 255))<<new QBrush(QColor(255, 0, 0));
//    transModel = new TransModel(this);

//    transModel->setHorizontalHeaderItem(0, new QStandardItem("文件"));
//    transModel->setHorizontalHeaderItem(1, new QStandardItem("大小"));
//    transModel->setHorizontalHeaderItem(2, new QStandardItem("状态"));
//    transModel->setHorizontalHeaderItem(3, new QStandardItem("完成时间"));
//    transModel->setHorizontalHeaderItem(4, new QStandardItem("进度"));

//    ui->scrollArea->setWidgetResizable(true);
//    ui->scrollArea->setFrameShape(QFrame::NoFrame);
//    ui->transview->setModel(transModel);
//    ui->transview->setColumnWidth(0, 400);
//    ui->transview->setColumnWidth(1,60);
//    ui->transview->setColumnWidth(colIndex_state,60);
//    ui->transview->setColumnWidth(colIndex_speed,120);
//    ui->transview->setColumnWidth(colIndex_progress, 150);



//    BarDelegate* bar = new BarDelegate(this);
//    ui->transview->setItemDelegateForColumn(colIndex_progress,bar);

//    connect(&tProgress, SIGNAL(timeout()), this, SLOT(progressCheck()));
//    tProgress.start(1000);
}

int TransList::download(DownloadInfo*)
{
    return 0;
}

TransList::~TransList()
{
    delete ui;
}

QString TransList::sizeofbytes(quint64 fsize)
{
    int i = 0;
    int num = 0;
    QStringList l;
    l<<"B"<<"KB"<<"MB"<<"GB";

    while((num=fsize/1000) && (i<3))
    {
        fsize = num;
        i++;
    }
    return QString::number(fsize)+l.at(i);
}

void TransList::listUiInit()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->addItems(QStringList()<<"上传列表"<<"下载列表");
    ui->listWidget->item(0)->setSizeHint(QSize(440,30));
    ui->listWidget->item(1)->setSizeHint(QSize(440,30));
    ui->listWidget->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->item(1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    ui->listWidget->setCurrentRow(0);
}

void TransList::progressCheck()
{
    for(int i=0; i<taskList.count();)
    {
        if(taskList.at(i)->taskinfo().taskState == FINISHI_STATE)
        {
            netTrans* trans = taskList.takeAt(i);
            delete trans;
            transModel->removeRow(i);
//            ui->transview->update();
            continue;
        }
        else
        {
            transModel->item(i, colIndex_state)->setForeground(*(brush.at(taskList.at(i)->taskinfo().taskState)));
            transModel->setData(transModel->index(i,colIndex_state),tips.at(taskList.at(i)->taskinfo().taskState),Qt::DisplayRole);
        }
        transModel->setData(transModel->index(i,colIndex_speed),taskList.at(i)->getTaskSpeed(), Qt::DisplayRole);
        transModel->setData(transModel->index(i,colIndex_progress),taskList.at(i)->getTaskProgress());
        i++;
    }
}

void TransList::downloadFinish(DownloadInfo*)
{

}

void TransList::newTask(netTrans *trans)
{
    int rows = transModel->rowCount();
    taskList<<trans;
    if(taskList.count()<=3)
        trans->taskStart();
//    qDebug()<<sizeofbytes(trans->taskinfo().fileSize);
    transModel->setItem(rows,0,new QStandardItem(trans->taskinfo().fileName));
    transModel->setItem(rows,1,new QStandardItem(sizeofbytes(trans->taskinfo().fileSize)));
    transModel->setItem(rows,2,new QStandardItem());
    transModel->setItem(rows,3,new QStandardItem());
    transModel->setItem(rows,4,new QStandardItem());

    transModel->item(rows,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    transModel->item(rows,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    transModel->item(rows,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}
