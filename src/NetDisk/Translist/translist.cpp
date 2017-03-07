#include "translist.h"
#include "ui_translist.h"

TransList::TransList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransList)
{
    ui->setupUi(this);
    transNum = 1;
    transModel = new TransModel(transNum,2);
    transModel->setItem(0,0,new QStandardItem("123"));
    transModel->setItem(0,0,new QStandardItem("20"));

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setFrameShape(QFrame::NoFrame);
    ui->transview->setModel(transModel);
    BarDelegate* bar = new BarDelegate(this);
    ui->transview->setItemDelegateForColumn(1,bar);
    transModel->setData(transModel->index(0,1),50);

}

TransList::~TransList()
{
    delete ui;
}
