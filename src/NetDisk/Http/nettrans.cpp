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

    crlf = "\r\n";
    qsrand(QDateTime::currentDateTime().toTime_t());
    b=QVariant(qrand()).toString()+QVariant(qrand()).toString()+QVariant(qrand()).toString();
    boundary = "----"+b;
    contentType="multipart/form-data; boundary="+boundary;
    endBoundary=crlf+"--"+boundary+"--"+crlf;
    boundary="--"+boundary+crlf;

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
    FileInfo = QFileInfo(fileName);
    filepId = pId;
    isupload = true;
    md5Check();
    pFile->seek(0);

    return 0;
}

void netWork::netDownload(QString fileName, double fileId)
{
    QString nUrl;
    isupload = false;
    manager = new QNetworkAccessManager(this->parent());
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    pFile = new QFile(fileName, this->parent());
    if(!pFile->open(QFile::WriteOnly))
    {
        qDebug()<<"[File upload]:"<<"file open error.";
        return;
    }

    nUrl = QString(HTTP_ADDR) + "/api/file/download?"+QString("fid=%1").arg(fileId);
    qDebug()<<"[download]:"<<nUrl<<fileName;
    manager->get(QNetworkRequest(QUrl(nUrl)));
}

netWork::~netWork()
{
}

void netWork::md5Check()
{
    quint64 bytesToWrite = FileInfo.size();
    quint64 bytesWriten = 0;
    quint64 blockSize = 4*1024;
    QByteArray qba;
    QCryptographicHash ch(QCryptographicHash::Md5);

    while(bytesToWrite > 0)
    {
        qba = pFile->read(qMin(bytesToWrite, blockSize));
        ch.addData(qba);
        bytesToWrite -= qba.size();
        bytesWriten += qba.size();
        qba.resize(0);
    }

    fileMd5 = ch.result();
    qba = "md5=" + fileMd5.toHex();
    qDebug()<<"MD5"<<ch.result().toHex();
    QString nUrl = QString(HTTP_ADDR) + "/api/file/getFileByMd5";
    request.setUrl(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    manager->post(request, qba);
    qDebug()<<"post"<<nUrl<<qba;
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
                chunks = (FileInfo.size()) / CHUNK_SIZE + ((FileInfo.size() % CHUNK_SIZE) != 0);
                chunk = 0;
                bytesToLoad = FileInfo.size();
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

void netTrans::netDownload(QString fileName, double fileId)
{
    work->netDownload(fileName, fileId);
    return;
}

netTrans::~netTrans()
{
    Thread->quit();
    Thread->wait();
}
