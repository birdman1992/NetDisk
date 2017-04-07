#include "netdonwloader.h"
#include <QtDebug>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <qthread.h>
#include "Http/nethttp.h"
#include "netconfig.h"

downloadWork::downloadWork(QNetworkAccessManager* downloadManager, QObject *parent) :
    QObject(parent)
{
    manager = downloadManager;
}

downloadWork::~downloadWork()
{

}

void downloadWork::initTask(DownloadInfo *info)
{
    downloadInfo = info;

    if(info->downLoadPath.isEmpty())
        pFile = new QFile(netConf->getDownloadPath());
    else
        pFile = new QFile(info->downLoadPath);


}

void downloadWork::taskStart()
{
    QStringList paramList;
    QString nUrl;
    paramList<<QString("fid=%1&").arg(downloadInfo->taskId)<<QString("token=%1&").arg(downloadInfo->token)<<QString(APP_ID)+"&";
    sign = getSign(paramList);
    downloadInfo->taskState = DOWN_NEW;

    nUrl = netConf->getServerAddress() + "/api/file/checkDownload?"+QString("fid=%1&sign=%2&token=%3").arg(downloadInfo->taskId).arg(QString(sign.toHex())).arg(downloadInfo->token);
    nUrl+="&"+QString(APP_ID);
    qDebug()<<"[checkDownload]"<<nUrl;
    netReply = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(netReply, SIGNAL(readyRead()), this, SLOT(getDownloadAddr()));
}

QByteArray downloadWork::getSign(QStringList param)
{
    qSort(param.begin(), param.end());
    QString str = QString();
    for(int i=0; i<param.count(); i++)
    {
        str += param.at(i);
    }

    str += QString(APP_KEY);
    qDebug()<<str;

    QByteArray qba = QCryptographicHash::hash(str.toLocal8Bit(), QCryptographicHash::Md5);
    qDebug()<<qba.toHex();
    return qba;
}

QByteArray downloadWork::getPost(QStringList param)
{
    QByteArray postData = QByteArray();
    qSort(param.begin(), param.end());
    QString str = QString();
    for(int i=0; i<param.count(); i++)
    {
        str += param.at(i);
    }
    str += QString("token=%1&").arg(downloadInfo->token);
    postData += str.toUtf8();
    str += QString(APP_KEY);
//    qDebug()<<"[sign params]"<<str;
    QByteArray sign = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5);
//    qDebug()<<sign.toHex();
    postData += QString("sign=%1").arg(QString(sign.toHex())).toLocal8Bit();
    qDebug()<<"[post params]"<<postData;
    return postData;
}

void downloadWork::getDownloadAddr()
{
    QString nUrl;
    QJsonDocument parseDoc;
    QJsonParseError jError;

    QByteArray qba = netReply->readAll();qDebug()<<"[getServerAddr]"<<qba;
    parseDoc = QJsonDocument::fromJson(qba, &jError);

    nUrl = netConf->getServerAddress() + "/api/file/download?"+QString("fid=%1&sign=%2&token=%3").arg(downloadInfo->taskId).arg(QString(sign.toHex())).arg(downloadInfo->token);
    nUrl+="&"+QString(APP_ID);
    disconnect(netReply, SIGNAL(readyRead()), this, SLOT(getDownloadAddr()));
    netReply->deleteLater();

    if(jError.error == QJsonParseError::NoError)
    {
        if(parseDoc.isObject())
        {
            QJsonObject obj = parseDoc.object();
            if(obj.contains("server"))
            {
                QJsonValue value = obj.take("server");
                obj = value.toObject();
                if(obj.contains("SERVER_URL"))
                {
                    value = obj.take("SERVER_URL");
                    nUrl = value.toString() + "/api/file/download?"+QString("fid=%1&sign=%2").arg(downloadInfo->taskId).arg(QString(sign.toHex()));
                    nUrl+="&"+QString(APP_ID);
                }
            }
            if(obj.contains("msg"))
            {
                //解析返回的状态码
                QJsonValue value = obj.take("msg");
                QString msg = value.toString();
                qDebug()<<"msg"<<msg;
            }
        }
    }

    if(!pFile->open(QFile::ReadWrite))
    {
        qDebug()<<"[down task]:"<<downloadInfo->taskId<<"failed,file open error";
        downloadInfo->taskState = DOWN_ERROR;
        return;
    }
    downloadInfo->taskState = DOWN_LOADING;
    netReply = manager->get(QNetworkRequest(QUrl(nUrl)));
    qDebug()<<"[download]:"<<nUrl;
    connect(netReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));
    connect(netReply, SIGNAL(readyRead()), this, SLOT(fileRecv()));
    connect(netReply, SIGNAL(finished()), this, SLOT(fileRecvFinished()));
}

void downloadWork::fileRecv()
{
    QByteArray qba = netReply->readAll();qDebug()<<"download:"<<qba.size();

    QJsonDocument parseDoc;
    QJsonParseError jError;
    parseDoc = QJsonDocument::fromJson(qba, &jError);

    if(jError.error == QJsonParseError::NoError)
    {
        if(parseDoc.isObject())
        {
            QJsonObject obj = parseDoc.object();
            if(obj.contains("msg"))
            {
                //解析返回的状态码
                QJsonValue value = obj.take("msg");
                QString msg = value.toString();
                qDebug()<<"msg"<<msg;
            }
        }
    }

    downloadInfo->curSize+=qba.size();
    downloadInfo->taskSpeed += qba.size();
    pFile->write(qba);
}

void downloadWork::replyError(QNetworkReply::NetworkError errorCode)
{
    qDebug()<<errorCode;
    downloadInfo->taskState = DOWN_ERROR;
    if(pFile->isOpen())
    {
        pFile->close();
    }
    qDebug()<<"remove"<<pFile->remove();
}

void downloadWork::fileRecvFinished()
{
    if(pFile->isOpen())
    {
        pFile->close();
    }
    qDebug()<<"[downloadWork]:"<<pFile->fileName()<<"download finished.";
    if(getMd5(pFile) == downloadInfo->fileMd5)
    {
        downloadInfo->taskState = DOWN_FINISH;qDebug()<<"check pass";
    }
    else
        downloadInfo->taskState = DOWN_ERROR;
    emit taskFinished(downloadInfo);
}

QByteArray downloadWork::getMd5(QFile *f)
{
//    quint64 bytesToWrite;
//    quint64 bytesWriten = 0;
//    quint64 blockSize = 4*1024;
//    QByteArray qba;
    QCryptographicHash ch(QCryptographicHash::Md5);

//    if(!pFile->open(QFile::ReadOnly))
//    {
//        qDebug()<<"[File upload]:"<<"file open error.";
//        return QByteArray();
//    }
//    bytesToWrite = f->size();qDebug()<<f->size();

//    while(bytesToWrite > 0)
//    {
//        qba = f->read(qMin(bytesToWrite, blockSize));
//        ch.addData(qba);
//        bytesToWrite -= qba.size();
//        bytesWriten += qba.size();
//        qba.resize(0);
//    }
//    pFile->close();
    QByteArray md5buf = (downloadInfo->fileName+QString::number(f->size())).toUtf8();
    ch.addData(md5buf);
    qDebug()<<md5buf;
    qDebug()<<"MD5"<<ch.result().toHex();
    qDebug()<<"fileMd5"<<downloadInfo->fileMd5;
    return ch.result();
}
/****************************************************************************************************/
NetDonwloader::NetDonwloader(QObject *parent):
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    Thread = new QThread(this);
    work = new downloadWork(manager, this);
    work->moveToThread(Thread);
    Thread->start();
    isLeisure = true;
}

bool NetDonwloader::downloaderIsLeisure()
{
    return isLeisure;
}

bool NetDonwloader::addTask(DownloadInfo *info)
{
    if(!isLeisure)
        return false;
    downloadInfo = info;
    work->initTask(downloadInfo);
    work->taskStart();
    return true;
}

NetDonwloader::~NetDonwloader()
{
    Thread->quit();
    Thread->wait();
    delete work;
}
