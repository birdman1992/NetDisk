#include "diskftp.h"
#include <QtDebug>
#include <qfile.h>

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
    {qDebug()<<"list:"<<ftpListDIR;
        netFtp->list(_ToSpecialEncoding(ftpListDIR));
    }
}

//FTP创建目录
void DiskFtp::ftpMkdir(QString dir)
{qDebug()<<"mk";
//    ftpListDIR = dir;
    if(cmdState == 0)//FTP未连接，连接FTP
    {qDebug()<<"2";
        ftpLogin(ftpUser, ftpPasswd);
    }
    else if(cmdState == 2)
    {
        qDebug()<<"mkdir"<<dir;
        netFtp->mkdir(_ToSpecialEncoding(dir));
    }
}

void DiskFtp::ftpRmdir(QString dir)
{
//    ftpListDIR = dir;
    if(cmdState == 0)//FTP未连接，连接FTP
    {
        ftpLogin(ftpUser, ftpPasswd);
    }
    else if(cmdState == 2)
    {
        netFtp->rmdir(_ToSpecialEncoding(dir));
    }
}

void DiskFtp::ftpRename(QString oldDir, QString newDir)
{
    if(cmdState == 0)//FTP未连接，连接FTP
    {
        ftpLogin(ftpUser, ftpPasswd);
    }
    else if(cmdState == 2)
    {
        netFtp->rename(_ToSpecialEncoding(oldDir), _ToSpecialEncoding(newDir));
    }
}

void DiskFtp::ftpUpload(QString localFolder, QString fileName)
{
    if(cmdState == 0)//FTP未连接，连接FTP
    {
        ftpLogin(ftpUser, ftpPasswd);
    }
    else if(cmdState == 2)
    {
        QFile* fLocal = new QFile(localFolder);
        if(!fLocal->open(QIODevice::ReadWrite))
        {
            qDebug("open error");
        }
        netFtp->put(fLocal, _ToSpecialEncoding(fileName));
    }
}

void DiskFtp::ftpCd(QString dir)
{
    if(cmdState == 0)//FTP未连接，连接FTP
    {
        ftpLogin(ftpUser, ftpPasswd);
    }
    else if(cmdState == 2)
    {
        netFtp->cd(_ToSpecialEncoding(dir));
    }
}

/***FTP编码转换函数***/
QString DiskFtp::_FromSpecialEncoding(const QString &InputStr)//未使用
{
#ifdef Q_OS_WIN
    return  QString::fromLocal8Bit(InputStr.toLatin1());
#else
    QTextCodec *codec = QTextCodec::codecForName("gbk");
    if (codec)
    {
        return codec->toUnicode(InputStr.toLatin1());
    }
    else
    {
        return QString("");
    }
#endif
}


QString DiskFtp::_ToSpecialEncoding(const QString &InputStr)//可使用
{
#ifdef Q_OS_WIN
    return QString::fromLatin1(InputStr.toUtf8());
#else
    QTextCodec *codec= QTextCodec::codecForName("gbk");
    if (codec)
    {
        return QString::fromLatin1(codec->fromUnicode(InputStr));
    }
    else
    {
        return QString("");
    }
#endif
}

/***FTP命令处理槽函数***/
void DiskFtp::ftpCommandFinished(int, bool error)
{
    if(!netFtp)
        return;

    if(error)
        emit needRefresh();

    if(netFtp->currentCommand() == QFtp::ConnectToHost)
    {
        if(error)
            qDebug()<<"QFtp::ConnectToHost error";
    }
    else if(netFtp->currentCommand() == QFtp::Login)
    {
        if(error)
            qDebug()<<"QFtp::Login error";
        else
        {
            cmdState = 2;//ftp已连接
            emit needRefresh();
        }
    }
    else if(netFtp->currentCommand() == QFtp::List)
    {
        if(error)
        {
            qDebug()<<"QFtp::List error";
        }
    }
    else if(netFtp->currentCommand() == QFtp::Mkdir)
    {
        if(error)
            qDebug()<<"QFtp::Mkdir error";
        emit needRefresh();
    }
    else if(netFtp->currentCommand() == QFtp::Rename)
    {
        if(error)
            qDebug()<<"QFtp::Rename error";
        emit needRefresh();
    }
    else if(netFtp->currentCommand() == QFtp::Put)
    {
        if(error)
            qDebug()<<"QFtp::Put error";
        emit needRefresh();
    }
    else if(netFtp->currentCommand() == QFtp::Cd)
    {
        if(error)
            qDebug()<<"QFtp::Cd error";
        else
            emit cmdCd();
    }
    else if(netFtp->currentCommand() == QFtp::Rmdir)
    {
        if(error)
            qDebug()<<"QFtp::Rmdir error";
        emit needRefresh();
    }
}
