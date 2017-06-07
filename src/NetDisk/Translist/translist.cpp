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

    connect(&tProgress, SIGNAL(timeout()), this, SLOT(progressCheck()));
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
    totalTaskNum = 0;
    curSyncNum = 0;
    totalSyncNum = 0;
    curTaskNum = 0;
    downloadNum = 0;
    colIndex_state = 2;
    colIndex_speed = 3;
    colIndex_progress = 4;

    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->addItems(QStringList()<<"上传列表"<<"下载列表");
    ui->listWidget->item(0)->setSizeHint(QSize(440,30));
    ui->listWidget->item(1)->setSizeHint(QSize(440,30));
    ui->listWidget->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->item(1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    ui->listWidget->setCurrentRow(0);

    ui->downloadTable->setColumnCount(5);
    ui->downloadTable->setColumnWidth(0, 400);//文件名
    ui->downloadTable->setColumnWidth(1, 60);//大小
    ui->downloadTable->setColumnWidth(2, 60);//状态
    ui->downloadTable->setColumnWidth(3, 120);//完成时间
    ui->downloadTable->setColumnWidth(4, 150);//进度

    ui->uploadTable->setColumnCount(5);
    ui->uploadTable->setColumnWidth(0, 400);//文件名
    ui->uploadTable->setColumnWidth(1, 60);//大小
    ui->uploadTable->setColumnWidth(2, 60);//状态
    ui->uploadTable->setColumnWidth(3, 120);//完成时间
    ui->uploadTable->setColumnWidth(4, 150);//进度
}

void TransList::checkDownload()
{
    downloadNum = getDownloadNum();

    for(int i=0; i<taskDownload.count(); i++)
    {
        if((taskDownload.at(i)->trans->taskinfo().taskState == NO_STATE))
        {
            if(downloadNum < netConf->getMaxTaskNum())
            {qDebug()<<"[taskstart]"<<downloadNum<<netConf->getMaxTaskNum();
                taskDownload.at(i)->trans->taskStart();
                downloadNum++;
            }
            else
                break;
        }
    }
}

int TransList::getDownloadNum()
{
    int ret = 0;
    for(int i=0; (i<taskDownload.count()); i++)
    {
//        if(taskDownload.at(i)->trans == NULL)
//        {
//            qDebug()<<"[getDownloadNum]"<<i<<"isNULL";
//            ui->downloadTable->removeRow(i);
//            delete taskDownload.takeAt(i);
//            i--;
//            continue;
//        }
//        qDebug()<<"[getDownloadNum]"<<i<<taskDownload.at(i)->trans->taskinfo().taskState;
        if(taskDownload.at(i)->trans->taskinfo().taskState == DOWNLOAD_STATE)
        {
            ret++;
            ui->downloadTable->item(i, colIndex_state)->setText("下载中");
            ui->downloadTable->item(i,colIndex_speed)->setText(taskDownload.at(i)->trans->getTaskSpeed());
        }
        else if(taskDownload.at(i)->trans->taskinfo().taskState == FINISHI_STATE)
        {
            ui->downloadTable->removeRow(i);
            if(taskDownload.at(i)->isSync)
                curSyncNum--;
            delete taskDownload.takeAt(i);
            i--;
        }
        else if(taskDownload.at(i)->trans->taskinfo().taskState == ERROR_STATE)
        {
            if(ui->downloadTable->item(i, colIndex_state)->text() == "下载失败")
            {
                ui->downloadTable->removeRow(i);
                if(taskDownload.at(i)->isSync)
                    curSyncNum--;
                delete taskDownload.takeAt(i);
                i--;
            }
            else
            {
                ui->downloadTable->item(i, colIndex_state)->setText("下载失败");
                ui->downloadTable->item(i, colIndex_state)->setTextColor(QColor(222,0,0));
            }
        }
    }

    return ret;
}

void TransList::checkUpload()
{
    int uploadNum = getUploadNum();

    for(int i=0; i<taskUpload.count(); i++)
    {
        if((taskUpload.at(i)->trans->taskinfo().taskState == NO_STATE))
        {
            if(uploadNum <= netConf->getMaxTaskNum())
                taskUpload.at(i)->trans->taskStart();
            else
                break;
        }
    }
}

int TransList::getUploadNum()
{
    int ret = 0;
    for(int i=0; (i<taskUpload.count()); i++)
    {
//        if(taskUpload.at(i)->trans == NULL)
//        {
//            qDebug()<<"[getUploadNum]"<<i<<"isNULL";
//            ui->uploadTable->removeRow(i);
//            delete taskUpload.takeAt(i);
//            i--;
//            continue;
//        }
//        qDebug()<<"[getUploadNum]"<<i<<taskUpload.at(i)->trans->taskinfo().taskState;
        if(taskUpload.at(i)->trans->taskinfo().taskState == UPLOAD_STATE)
        {
            ret++;
            ui->uploadTable->item(i, colIndex_state)->setText("上传中");
            ui->uploadTable->item(i,colIndex_speed)->setText(taskUpload.at(i)->trans->getTaskSpeed());
        }
        else if(taskUpload.at(i)->trans->taskinfo().taskState == FINISHI_STATE)
        {
            ui->uploadTable->removeRow(i);
            if(taskUpload.at(i)->isSync)
                curSyncNum--;
            delete taskUpload.takeAt(i);
            i--;
        }
        else if(taskUpload.at(i)->trans->taskinfo().taskState == ERROR_STATE)
        {
            if(ui->uploadTable->item(i, colIndex_state)->text() == "上传失败")
            {
                ui->uploadTable->removeRow(i);
                if(taskUpload.at(i)->isSync)
                    curSyncNum--;
                delete taskUpload.takeAt(i);
                i--;
            }
            else
            {
                ui->uploadTable->item(i, colIndex_state)->setText("上传失败");
                ui->uploadTable->item(i, colIndex_state)->setTextColor(QColor(222,0,0));
            }
        }
    }

    return ret;
}

void TransList::progressCheck()
{

    curTaskNum = taskDownload.count() + taskUpload.count();
    checkDownload();
    checkUpload();

//    if(totalTaskNum)
//        emit taskProgress(100-100*curTaskNum/totalTaskNum);
    if(totalSyncNum)
        emit syncProgress(100-100*curSyncNum/totalSyncNum);

    qDebug()<<"progressCheck"<<curSyncNum<<"/"<<totalSyncNum;
    if(curTaskNum == 0 && totalTaskNum==0)
    {qDebug()<<"stop";
        emit taskClear();
        tProgress.stop();
    }
    if(curSyncNum == 0 && totalSyncNum!=0)
    {
        emit syncClear();
    }

    if(curSyncNum == 0)
        totalSyncNum = 0;

    if(curTaskNum == 0)
        totalTaskNum = 0;
//    for(int i=0; i<taskList.count();)
//    {
//        if(taskList.at(i)->taskinfo().taskState == FINISHI_STATE)
//        {
//            netTrans* trans = taskList.takeAt(i);
//            delete trans;
//            transModel->removeRow(i);
//            continue;
//        }
//        else
//        {
//            transModel->item(i, colIndex_state)->setForeground(*(brush.at(taskList.at(i)->taskinfo().taskState)));
//            transModel->setData(transModel->index(i,colIndex_state),tips.at(taskList.at(i)->taskinfo().taskState),Qt::DisplayRole);
//        }
//        transModel->setData(transModel->index(i,colIndex_speed),taskList.at(i)->getTaskSpeed(), Qt::DisplayRole);
//        transModel->setData(transModel->index(i,colIndex_progress),taskList.at(i)->getTaskProgress());
//        i++;
//    }
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

//文件名 大小 状态 完成时间 进度
void TransList::newDownloadTask(netTrans *trans, bool isSync)
{
    TaskRow* _row = new TaskRow();
    totalTaskNum++;
    if(isSync)
    {
        totalSyncNum++;
        curSyncNum++;
    }
    _row->isSync = isSync;
    _row->trans = trans;
    _row->progress->setStyleSheet("QProgressBar {margin-top:10px;margin-bottom:10px;margin-right:36px; border: 0px solid grey;background-color: rgb(225, 230, 240);}\
                                  QProgressBar::chunk {background-color: rgb(194, 200, 204);}");
    int rows = taskDownload.count();
    connect(_row->trans, SIGNAL(downloadProgress(int)), _row->progress, SLOT(setValue(int)));
//    connect(_row->trans, SIGNAL(downloadProgress(int)), this, SLOT(setValue(int)));
    taskDownload<<_row;
    ui->downloadTable->setRowCount(taskDownload.count());

    ui->downloadTable->setItem(rows, 0, new QTableWidgetItem(trans->taskinfo().fileName));
    ui->downloadTable->setItem(rows, 1, new QTableWidgetItem(sizeofbytes(trans->taskinfo().fileSize)));
    ui->downloadTable->setItem(rows, 2, new QTableWidgetItem("等待中"));
    ui->downloadTable->setItem(rows, 3, new QTableWidgetItem("---"));
    ui->downloadTable->setCellWidget(rows, 4, _row->proCell);

    if(!tProgress.isActive())
        tProgress.start(1000);
}

void TransList::newUploadTask(netTrans *trans, bool isSync)
{
    TaskRow* _row = new TaskRow();
    totalTaskNum++;
    if(isSync)
    {
        totalSyncNum++;
        curSyncNum++;
    }
    _row->isSync = isSync;
    _row->trans = trans;
    _row->progress->setStyleSheet("QProgressBar {margin-top:10px;margin-bottom:10px;margin-right:36px; border: 0px solid grey;background-color: rgb(225, 230, 240);}\
                                  QProgressBar::chunk {background-color: rgb(194, 200, 204);}");
    int rows = taskUpload.count();
    connect(_row->trans, SIGNAL(downloadProgress(int)), _row->progress, SLOT(setValue(int)));
    connect(_row->trans, SIGNAL(downloadProgress(int)), this, SLOT(setValue(int)));
    taskUpload<<_row;
    ui->uploadTable->setRowCount(taskUpload.count());

    ui->uploadTable->setItem(rows, 0, new QTableWidgetItem(trans->taskinfo().fileName));
    ui->uploadTable->setItem(rows, 1, new QTableWidgetItem(sizeofbytes(trans->taskinfo().fileSize)));
    ui->uploadTable->setItem(rows, 2, new QTableWidgetItem("等待中"));
    ui->uploadTable->setItem(rows, 3, new QTableWidgetItem("---"));
    ui->uploadTable->setCellWidget(rows, 4, _row->proCell);

    if(!tProgress.isActive())
        tProgress.start(1000);
}

void TransList::setValue(int val)
{
    qDebug()<<"setValue"<<val;
}


TaskRow::TaskRow()
{
    proCell = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(proCell);
    progress = new QProgressBar(proCell);
    hLayout->addWidget(progress);
    proCell->setLayout(hLayout);
    progress->setMaximumHeight(25);
    progress->setTextVisible(true);
    progress->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    progress->setRange(0,100);
}

TaskRow::~TaskRow()
{
    if(proCell!=NULL)
        delete proCell;
    if(trans!=NULL)
    {
//        qDebug()<<"[TaskRow delete]"<<trans->taskinfo().taskId<<trans->taskinfo().taskState;
        delete trans;
        trans = NULL;
    }
}
