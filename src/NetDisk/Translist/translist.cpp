#include "translist.h"
#include "ui_translist.h"
#include <QtDebug>
#include <qstring.h>

TransList::TransList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransList)
{
    ui->setupUi(this);
    transNum = 1;
    progress = 1;
    transModel = new TransModel(transNum,2);

    transModel->setHorizontalHeaderItem(0, new QStandardItem("文件"));
    transModel->setHorizontalHeaderItem(1, new QStandardItem("大小"));
    transModel->setHorizontalHeaderItem(2, new QStandardItem("完成时间"));
    transModel->setHorizontalHeaderItem(3, new QStandardItem("进度"));

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setFrameShape(QFrame::NoFrame);
    ui->transview->setModel(transModel);
    ui->transview->setColumnWidth(0, 400);
    ui->transview->setColumnWidth(1,100);
    ui->transview->setColumnWidth(2,100);
    ui->transview->setColumnWidth(3, 150);

    BarDelegate* bar = new BarDelegate(this);
    ui->transview->setItemDelegateForColumn(3,bar);

    transModel->setData(transModel->index(0,3),progress);
    connect(&tProgress, SIGNAL(timeout()), this, SLOT(progressCheck()));
    tProgress.start(1000);
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

    while(num = (fsize/1000))
    {
        fsize /= 1000;
        i++;
    }
    return QString::number(fsize)+l.at(i);
}

void TransList::progressCheck()
{qDebug()<<"progress:"<<progress;
    progress++;
    transModel->setData(transModel->index(0,3),progress);
}

void TransList::newTask(netTrans *trans)
{
    int cols = transModel->columnCount();
    qDebug()<<sizeofbytes(trans->taskinfo().fileSize);
//    transModel->setItem(cols,0,new QStandardItem(trans->taskinfo().fileName));
//    transModel->setItem(cols,1,new QStandardItem(QString:: trans->taskinfo().fileSize));
//    transModel->setItem(cols,2,new QStandardItem(,));
//    transModel->setItem(cols,3,new QStandardItem());
}
