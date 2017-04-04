#include "synclist.h"
#include "ui_synclist.h"
#include <QTableWidgetItem>
#include <qfileiconprovider.h>
#include <QDebug>
#include <QIcon>

#include "netconfig.h"

syncList::syncList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::syncList)
{
    ui->setupUi(this);
    ui->tableWidget->setItemDelegate(new NoFocusDelegate());
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("名称"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("修改日期"));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("类型"));
    ui->tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem("大小"));
    ui->tableWidget->setColumnWidth(0, 20);
    ui->tableWidget->setColumnWidth(1, 280);
    ui->tableWidget->setColumnWidth(2, 180);
    ui->tableWidget->setColumnWidth(3, 150);
    ui->tableWidget->setColumnWidth(4, 90);

    checkTable = NULL;
    currentIndex = 0;
    currentDir = netConf->getSyncPath();
    list_show = QDir(currentDir).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
//    showList();
    list_path<<new QFileInfo("我的同步");
}

syncList::~syncList()
{
    delete ui;
}

void syncList::initTable(syncTable *table)
{
    checkTable = table;
}

void syncList::goAhead()
{
    qDebug()<<"goAhead"<<currentIndex;
    if(currentIndex>=(list_path.count()-1))
        return;
    currentIndex++;
    currentDir = list_path.at(currentIndex)->absoluteFilePath();
    list_show = QDir(currentDir).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    if(currentIndex>=(list_path.count()-1))
    {
        emit historyEnable(true, false);
    }
    else
        emit historyEnable(true, true);

    emit pathChanged(list_path.mid(0, currentIndex+1));
    showList();
}

void syncList::goBack()
{qDebug()<<"goBack"<<currentIndex;
    if(currentIndex<=0)
        return;
    currentIndex--;
    if(currentIndex<=0)
    {
        emit historyEnable(false, true);
        currentDir = netConf->getSyncPath();
        list_show = QDir(currentDir).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    }
    else
    {
        emit historyEnable(true, true);
        currentDir = list_path.at(currentIndex)->absoluteFilePath();
        list_show = QDir(currentDir).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    }

    emit pathChanged(list_path.mid(0, currentIndex+1));
    showList();
}

void syncList::cmdCd(int id)
{
    qDebug()<<"cd"<<id;
    int index = id;
    if((index<0) || (index>list_path.count()))
        return;
    while(list_path.count()>(index+1))
    {
        QFileInfo* info = list_path.takeLast();
        delete info;
    }

    currentIndex = list_path.count()-1;
    if(index == 0)
    {
        currentDir = netConf->getSyncPath();
        list_show = QDir(currentDir).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
        emit historyEnable(false, false);

    }
    else
    {
        currentDir = list_path.at(index)->absoluteFilePath();
        list_show = QDir(currentDir).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
        emit historyEnable(true, false);
    }
    emit pathChanged(list_path);
    showList();
}

void syncList::showList()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(list_show.count());

    for(int i=0; i<list_show.count();)

    if(list_show.isEmpty())
    {
        return;
    }

    for(int i=0; i<list_show.count(); i++)
    {
        QFileIconProvider icoProvider;
        bool ischanged = false;
        checkTable->getIdByName(list_show.at(i).absoluteFilePath(), &ischanged);
        if(ischanged)
            ui->tableWidget->setItem(i,0,new QTableWidgetItem(QIcon(":/imgs/32x32/alert_warning.png"),""));
        else
            ui->tableWidget->setItem(i,0,new QTableWidgetItem(QIcon(":/imgs/32x32/green_ok_yes.png"),""));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(QIcon(icoProvider.icon(list_show.at(i))),list_show.at(i).fileName()));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(list_show.at(i).lastModified().toString("yyyy/MM/dd hh:mm")));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(getFolderType(list_show.at(i))));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(getFolderSize(list_show.at(i).size())));
    }
}

QString syncList::getFolderType(QFileInfo fInfo)
{
    QFileIconProvider icoProvider;
    QString folder_type = icoProvider.type(fInfo);

    return folder_type;
}

QString syncList::getFolderSize(quint64 fileSize)
{
    int i = 0;
    int num = 0;
    QStringList l;
    l<<"B"<<"KB"<<"MB"<<"GB";

    if(fileSize == 0)
        return "";

    while((num=fileSize/1000) && (i<3))
    {
        fileSize = num;
        i++;
    }
    return QString::number(fileSize)+l.at(i);
}

void syncList::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    QFileInfo* info = new QFileInfo(list_show.at(index.row()));
    if(!info->isDir())
        return;

    while(list_path.count()>(currentIndex+1))
    {
        QFileInfo* fInfo = list_path.takeLast();
        delete fInfo;
    }
    list_path<<info;


    list_show = QDir(info->absoluteFilePath()).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);

    currentDir = info->absoluteFilePath();
    emit historyEnable(true, false);
    currentIndex = list_path.count()-1;
    emit pathChanged(list_path);
    showList();
    qDebug()<<list_path.count();

}

void syncList::showEvent(QShowEvent* event)
{
    qDebug()<<"showevent"<<currentIndex;
    emit pathChanged(list_path);
    if(currentIndex <= 0)
    {
        if(currentIndex<(list_path.count()-1))
            emit historyEnable(false, true);
        else
            emit historyEnable(false, false);
    }
    else
    {
        if(currentIndex<(list_path.count()-1))
            emit historyEnable(true, true);
        else
            emit historyEnable(true, false);
    }
    showList();
    QWidget::showEvent(event);
}

/************
NoFocusDelegate
*************/

NoFocusDelegate::NoFocusDelegate(QStyledItemDelegate *parent):
    QStyledItemDelegate(parent)
{

}

void NoFocusDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption(option);
    if (itemOption.state & QStyle::State_HasFocus)
    {
        itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
    }
    QStyledItemDelegate::paint(painter, itemOption, index);
}

QSize NoFocusDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return QSize();
}
