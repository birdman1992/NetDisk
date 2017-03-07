#include "nettrans.h"
#include <QtDebug>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

netWork::netWork(QObject *parent) :
    QObject(parent)
{
    md5CheckOk = true;
    netState = 0;
    isupload = false;
    pFile = NULL;
    manager = NULL;
    netReply = NULL;
    taskInfo.taskState = LOCK_STATE;
    taskInfo.taskSpeed = 0;

    crlf = "\r\n";
    qsrand(QDateTime::currentDateTime().toTime_t());
    b=QVariant(qrand()).toString()+QVariant(qrand()).toString()+QVariant(qrand()).toString();
    boundary = "----"+b;
    contentType="multipart/form-data; boundary="+boundary;
    endBoundary=crlf+"--"+boundary+"--"+crlf;
    boundary="--"+boundary+crlf;

}

netWork::~netWork()
{
}

int netWork::netUpload(QString fileName, double pId)
{
    qDebug()<<fileName<<pId;
    netState = 0;

    if(manager == NULL)
    {
        manager = new QNetworkAccessManager(this->parent());
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    }

    //打开文件
    pFile = new QFile(fileName, this->parent());
    if(!pFile->open(QFile::ReadWrite))
    {
        qDebug()<<"[File upload]:"<<"file open error.";
        return -1;
    }
    fInfo = QFileInfo(fileName);
    filepId = pId;
    isupload = true;
    taskInfo.taskState = NO_STATE;
    taskInfo.fileName = fInfo.fileName();
    taskInfo.fileSize = pFile->size();


    return 0;
}

void netWork::netDownload(fileInfo info)
{
    isupload = false;
    manager = new QNetworkAccessManager(this->parent());
//    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    fileMd5 = QByteArray::fromHex(info.MD5.toLocal8Bit());
    taskInfo.fileName = info.FILE_NAME;
    taskInfo.fileSize = info.SIZE;

    pFile = new QFile(info.FILE_NAME, this->parent());
    if(!pFile->open(QFile::WriteOnly))
    {
        qDebug()<<"[File upload]:"<<"file open error.";
        return;
    }

    nUrl = QString(HTTP_ADDR) + "/api/file/download?"+QString("fid=%1").arg(info.ID);
    qDebug()<<"[download]:"<<nUrl<<info.FILE_NAME<<fileMd5.toHex();
}

TaskInfo netWork::taskinfo()
{
    return taskInfo;
}

void netWork::taskStart()
{
    if(isupload)
    {
        md5Check();
        pFile->seek(0);
    }
    else
    {
        netReply = manager->get(QNetworkRequest(QUrl(nUrl)));
        connect(netReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));
        connect(netReply, SIGNAL(readyRead()), this, SLOT(fileRecv()));
        connect(netReply, SIGNAL(finished()), this, SLOT(fileRecvFinished()));
    }
}

void netWork::md5Check()
{
    quint64 bytesToWrite = fInfo.size();
    quint64 bytesWriten = 0;
    quint64 blockSize = 4*1024;
    QByteArray qba;
    QCryptographicHash ch(QCryptographicHash::Md5);

    pFile->seek(0);
    while(bytesToWrite > 0)
    {
        qba = pFile->read(qMin(bytesToWrite, blockSize));
        ch.addData(qba);
        bytesToWrite -= qba.size();
        bytesWriten += qba.size();
        qba.resize(0);
    }
    pFile->seek(0);

    fileMd5 = ch.result();
    qba = "md5=" + fileMd5.toHex();
    qDebug()<<"MD5"<<ch.result().toHex();
    QString nUrl = QString(HTTP_ADDR) + "/api/file/getFileByMd5";
    request.setUrl(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    manager->post(request, qba);
    qDebug()<<"post"<<nUrl<<qba;
}

QByteArray netWork::getMd5(QFile *f)
{
    quint64 bytesToWrite = fInfo.size();
    quint64 bytesWriten = 0;
    quint64 blockSize = 4*1024;
    QByteArray qba;
    QCryptographicHash ch(QCryptographicHash::Md5);

    f->seek(0);
    while(bytesToWrite > 0)
    {
        qba = f->read(qMin(bytesToWrite, blockSize));
        ch.addData(qba);
        bytesToWrite -= qba.size();
        bytesWriten += qba.size();
        qba.resize(0);
    }

    fileMd5 = ch.result();
    f->seek(0);
    qba = "md5=" + fileMd5.toHex();
    qDebug()<<"MD5"<<ch.result().toHex();
    return fileMd5;
}

int netWork::fileUpload(bool reload)
{
    quint64 ret = 0;
    QString nUrl = QString(HTTP_ADDR) + "/api/file/upload";
    request.setUrl(nUrl);
    request.setRawHeader("Content-Type", contentType.toLocal8Bit());
    if(chunk >= chunks)
    {
        qDebug()<<"[netWork]:Upload finishi.";
        return 0;
    }

    chunksize = qMin(bytesToLoad, CHUNK_SIZE);

    QByteArray bond;
    QByteArray send;

    send.append(boundary.toLocal8Bit());
    bond = (crlf + boundary).toLocal8Bit();
    send.append(QString("Content-Disposition: form-data; name=\"chunksize\""+crlf));
    send.append(crlf);
    send.append(QString::number(chunksize).toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"chunk\""+crlf));
    send.append(crlf);
    send.append(QString::number(chunk).toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"chunks\""+crlf));
    send.append(crlf);
    send.append(QString::number(chunks).toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"size\""+crlf));
    send.append(crlf);
    send.append(QString::number(fileSize).toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"pid\""+crlf));
    send.append(crlf);
    send.append(QString::number(filepId).toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"md5\""+crlf));
    send.append(crlf);
    send.append(QString(fileMd5.toHex()));

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"file\";filename=\"%1\"").arg(pFile->fileName()));
    send.append(crlf);
    send.append(QString("Content-Type: application/octet-stream"));
    send.append(crlf);
    qDebug()<<"[fileUpload]:\n"<<QString(send);
    ret = send.size();
    send.append(pFile->read(chunksize));
    ret = send.size()-ret;
    send.append(crlf);
    send.append(crlf);
    send.append(endBoundary);
    qDebug()<<"[payload]:"<<ret;
    qDebug()<<endBoundary;
    chunk++;
    bytesToLoad -= chunksize;
    manager->post(request, send);
    return 0;
}

void netWork::replyFinished(QNetworkReply *reply)
{
    QByteArray qba = reply->readAll();
    if(isupload)
    {
        QJsonDocument parseDoc;
        QJsonParseError jError;
        QJsonValue jval;
        QJsonValue result;
        QString code;
        QString msg;
        qDebug()<<qba;
        parseDoc = QJsonDocument::fromJson(qba, &jError);

        if(jError.error == QJsonParseError::NoError)
        {
            if(parseDoc.isObject())
            {
                QJsonObject obj = parseDoc.object();

                if(obj.contains("code"))
                {
                    //解析返回的状态码
                    jval = obj.take("code");
                    code = jval.toString();
                    qDebug()<<"code"<<code;
                }
                if(obj.contains("msg"))
                {
                    //解析返回的状态码
                    jval = obj.take("msg");
                    msg = jval.toString();
                    qDebug()<<"msg"<<msg;
                }
                if(obj.contains("result"))
                {
                    //解析返回的状态码
                    result = obj.take("result");
                    qDebug()<<"result is null"<<result.isNull();
                }
            }
        }

        if(netState == 0)//接收MD5校验返回结果
        {qDebug()<<"state:"<<netState;
            if(code == "200")//文件MD5存在
            {
                qDebug("file exit");
            }
            else if(code == "-200")//文件MD5不存在
            {
                qDebug("file not exit upload");
                chunks = (fInfo.size()) / CHUNK_SIZE + ((fInfo.size() % CHUNK_SIZE) != 0);
                chunk = 0;
                bytesToLoad = fInfo.size();
                fileSize = bytesToLoad;
                qDebug()<<"fileSize"<<fileSize;
                netState = 1;//进入上传文件状态
                fileUpload(false);
            }
        }
        else if(netState == 1)//接收文件上传返回结果
        {
            if(code == "200")//单片上传成功
            {
                fileUpload(false);
            }
        }
    }
    else
    {
        qDebug()<<"[down]:"<<qba.size()<<reply->isFinished();
        pFile->write(qba);
    }
    reply->deleteLater();
}

void netWork::replyError(QNetworkReply::NetworkError errorCode)
{
    qDebug()<<errorCode;
    if(pFile->isOpen())
    {
        pFile->close();
    }
}

void netWork::fileRecv()
{
    QByteArray qba = netReply->readAll();qDebug()<<"download:"<<qba.size();
    pFile->write(qba);
}

void netWork::fileRecvFinished()
{
    qDebug()<<pFile->fileName()<<"download finished.";
    if(pFile->isOpen())
    {
        pFile->close();
    }
}

/****************************************************************************************************/
netTrans::netTrans(QObject *parent):
    QObject(parent)
{
    Thread = new QThread(this);
    pmanager = new QNetworkAccessManager(this);
    work = new netWork();
    work->moveToThread(Thread);
    Thread->start();
}

int netTrans::netUpload(QString fileName, double pId)
{
    work->netUpload(fileName, pId);
    return 0;
}

void netTrans::netDownload(fileInfo info)
{
    work->netDownload(info);
    return;
}

TaskInfo netTrans::taskinfo()
{
    return work->taskinfo();
}

netTrans::~netTrans()
{
    Thread->quit();
    Thread->wait();
}
