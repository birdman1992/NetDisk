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
