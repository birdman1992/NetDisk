#include "loading.h"
#include "ui_loading.h"

loading::loading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loading)
{
    ui->setupUi(this);
    str = "努力加载中，请稍候..";
    connect(&tReload, SIGNAL(timeout()), this, SLOT(reloadTimeout()));
}

loading::~loading()
{
    delete ui;
}

void loading::reloadStart()
{
    count = 0;
    tReload.start(1000);
}

void loading::reloadStop()
{
    tReload.stop();
}

void loading::reloadTimeout()
{
    count++;
    if(!count%8)
        emit reload();
    ui->label_2->setText(str+QString().fill('.', count%5));

    if(count >= 40)
    {
        reloadStop();
        ui->label_2->setText("网络似乎出了点问题...");
    }
}

