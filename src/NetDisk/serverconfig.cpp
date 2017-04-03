#include "serverconfig.h"
#include "ui_serverconfig.h"
#include <qmessagebox.h>
#include "netconfig.h"

ServerConfig::ServerConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerConfig)
{
    ui->setupUi(this);
}

ServerConfig::~ServerConfig()
{
    delete ui;
}

void ServerConfig::on_btn_conf_clicked()
{
    QString str = ui->server_info->text();
    QString pattern("*.*.*.*:*");
    QRegExp rx(pattern);
    rx.setPatternSyntax(QRegExp::Wildcard);
    if(rx.exactMatch(str))
    {
        QMessageBox::about(this, "设置服务器地址", "\n设置服务器地址成功。\n");
        netConf->setServerAddress(str);
        netConf->saveAll();
        this->close();
    }
    else
    {
        QMessageBox::about(this, "设置服务器地址", "\n  设置服务器地址失败，\n请输入正确的地址格式。\n");
    }
}

void ServerConfig::on_btn_cancel_clicked()
{

}
