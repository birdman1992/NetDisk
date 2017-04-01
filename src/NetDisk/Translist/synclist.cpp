#include "synclist.h"
#include "ui_synclist.h"

syncList::syncList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::syncList)
{
    ui->setupUi(this);
}

syncList::~syncList()
{
    delete ui;
}
