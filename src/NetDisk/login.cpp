#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->username->setText(netConf->getUsrname());
    ui->isauto->setChecked(netConf->autoLoginDisk());
    ui->isrem->setChecked(netConf->remPassword());
    if(netConf->remPassword())
        ui->passwd->setText(netConf->getPasswd());
    if(netConf->autoLoginDisk())
        emit netLogin();
}

login::~login()
{
    delete ui;
}

void login::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void login::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        QRect rect = QRect(0, 0, 360, 45);
        if(rect.contains(event->pos()))
        {
            wMoveable = true;
        }
        event->accept();
    }
}

void login::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && wMoveable)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void login::mouseReleaseEvent(QMouseEvent*)
{
    if(wMoveable)
    {
        wMoveable = false;
    }
}

void login::on_btn_min_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void login::on_btn_set_clicked()
{

}

void login::on_btn_close_clicked()
{
    this->close();
    emit close();
}

void login::on_btn_login_clicked()
{
    netConf->setUsrname(ui->username->text());
    netConf->setPasswd(ui->passwd->text());
    netConf->saveAll();
    emit netLogin();
}

void login::on_isrem_toggled(bool checked)
{
    netConf->setremPassword(checked);
    netConf->saveAll();
}

void login::on_isauto_toggled(bool checked)
{
    netConf->setautoLogin(checked);
    netConf->saveAll();
}
