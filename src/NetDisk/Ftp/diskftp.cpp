#include "diskftp.h"
#include <QtDebug>

#define FTP_ADDR "192.168.0.154"
#define FTP_PORT 21

DiskFtp::DiskFtp(QObject *parent) : QObject(parent)
{
    cmdState = 0;
}


/***FTP公有接口***/
//FTP登录
void DiskFtp::ftpLogin(QString user, QString passwd)
{
    netFtp = new QFtp(this);
    ftpListDIR = "/";

    //连接FTP相关槽函数
    connect(netFtp, SIGNAL(commandFinished(int,bool)), this, SLOT(ftpCommandFinished(int,bool)));
    connect(netFtp, SIGNAL(listInfo(QUrlInfo)), this, SIGNAL(listInfo(QUrlInfo)));

    cmdState = 1;//进入连接状态
    netFtp->connectToHost(FTP_ADDR,FTP_PORT);
    netFtp->login(user,passwd);
    ftpUser = user;
    ftpPasswd = passwd;
}

//FTP列出指定目录
void DiskFtp::ftpList(QString dir)
{
    ftpListDIR = dir;
    if(cmdState == 0)//FTP未连接，连接FTP
    {
        ftpLogin(ftpUser, ftpPasswd);
    }
    else if(cmdState == 2)
    {
        netFtp->list(ftpListDIR);
    }
}

//FTP创建目录
void DiskFtp::ftpMkdir(QString dir)
{
    ftpListDIR = dir;
    if(cmdState == 0)//FTP未连接，连接FTP
    {
        ftpLogin(ftpUser, ftpPasswd);
    }
    else if(cmdState == 2)
    {
        netFtp->mkdir(dir);
    }
}

void DiskFtp::ftpRmdir(QString dir)
{
    ftpListDIR = dir;
    if(cmdState == 0)//FTP未连接，连接FTP
    {
        ftpLogin(ftpUser, ftpPasswd);
    }
    else if(cmdState == 2)
    {
        netFtp->rmdir(dir);
    }
}


/***FTP命令处理槽函数***/
void DiskFtp::ftpCommandFinished(int, bool error)
{
    if(!netFtp)
        return;
    if(netFtp->currentCommand() == QFtp::ConnectToHost)
    {
        if(error)
            qDebug()<<"QFtp::ConnectToHost error";
        else
        {

        }
    }
    else if(netFtp->currentCommand() == QFtp::Login)
    {
        if(error)
            qDebug()<<"QFtp::Login error";
        else
        {
            cmdState = 2;//ftp已连接
            netFtp->list(ftpListDIR);
        }
    }
    else if(netFtp->currentCommand() == QFtp::List)
    {
        if(!error)
        {
//            qDebug()<<"LIST";
        }
    }
    else if(netFtp->currentCommand() == QFtp::Mkdir)
    {
        if(error)
            qDebug()<<"QFtp::Mkdir error";
    }
}

