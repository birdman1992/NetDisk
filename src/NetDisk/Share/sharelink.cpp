#include "sharelink.h"
#include "ui_sharelink.h"
#include <QDebug>
#include <QClipboard>
#include <QTimer>

ShareLink::ShareLink(QString link, QString passwd, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareLink)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::ToolTip | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    share_link = link;
    share_passwd = passwd;
    win_tips = NULL;
    waitForDelete = false;
    ui->link->setText(share_link);
    ui->passwd->setText(share_passwd);
}

ShareLink::~ShareLink()
{
    if(win_tips != NULL)
    {
        win_tips->close();
        delete win_tips;
    }
    delete ui;
}

void ShareLink::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ShareLink::showEvent(QShowEvent*)
{
    waitForDelete = false;
    QTimer::singleShot(200,this, SLOT(showTipTimeout()));
}

void ShareLink::showTips(QString tips)
{
    if(win_tips != NULL)
    {
        win_tips->close();
        delete win_tips;
    }
    win_tips = new QLabel(tips,NULL,Qt::ToolTip);
    win_tips->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    win_tips->setStyleSheet("color:rgb(255,255,255);background-color: rgb(117, 171, 237);font: 12px \"微软雅黑\";");
    win_tips->resize(196,34);
    win_tips->move(this->geometry().x()+this->geometry().width()/2-98, this->geometry().y()-100);
    win_tips->show();
    QTimer::singleShot(1000, this, SLOT(tipTimeOut()));
}

void ShareLink::on_close_clicked()
{
    this->close();
    this->deleteLater();
}

void ShareLink::on_copy_clicked()
{
    QString boardStr = QString("链接：%1 密码：%2").arg(share_link, share_passwd);
    QClipboard *board = QApplication::clipboard();
    board->setText(boardStr);
    showTips("分享链接及密码已复制到剪贴板");
    this->close();
    waitForDelete = true;
}

void ShareLink::tipTimeOut()
{
    win_tips->show();
    if(win_tips != NULL)
    {
        win_tips->close();
        delete win_tips;
        win_tips = NULL;
    }
    if(waitForDelete)
        this->deleteLater();
}

void ShareLink::showTipTimeout()
{
    showTips("创建分享链接成功");
}
