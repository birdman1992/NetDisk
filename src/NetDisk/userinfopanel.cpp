#include "userinfopanel.h"
#include "ui_userinfopanel.h"

UserInfoPanel::UserInfoPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInfoPanel)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Popup|Qt::Tool);
}

UserInfoPanel::~UserInfoPanel()
{
    delete ui;
}

void UserInfoPanel::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void UserInfoPanel::userinfoUpdate(UserInfo info)
{
    ui->user->setText(info.USER_NAME);
    ui->compInfo->setText(info.COMNAME);
    ui->spaceInfo->setText(sizeToByte(info.CURRENT_SIZE)+"/"+sizeToByte(info.MAX_SIZE));
    ui->progressBar->setValue(info.CURRENT_SIZE/info.MAX_SIZE*100);
}

QString UserInfoPanel::sizeToByte(quint64 fileSize)
{
    int i = 0;
    int num = 0;
    QStringList l;
    l<<"B"<<"KB"<<"MB"<<"GB"<<"TB";

    if(fileSize == 0)
        return "";

    while((num=fileSize/1000) && (i<(l.count()-1)))
    {
        fileSize = num;
        i++;
    }
    return QString::number(fileSize)+l.at(i);
}
