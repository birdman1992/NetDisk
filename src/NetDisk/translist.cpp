#include "translist.h"
#include "ui_translist.h"

TransList::TransList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransList)
{
    ui->setupUi(this);
}

TransList::~TransList()
{
    delete ui;
}
