#include "nettrans.h"
#include <QtDebug>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <qthread.h>
#include "netconfig.h"

netWork::netWork(QObject *parent) :
    QObject(parent)
{
    md5CheckOk = true;
    netState = 0;
    isupload = false;
    pFile = NULL;
    manager = NULL;
    netReply = NULL;
    taskInfo.taskState = NO_STATE;
    taskInfo.taskSpeed = 0;
    taskInfo.curSize = 0;

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
    taskInfo.curSize = 0;

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

void netWork::netDownload(fileInfo info, QString downLoadPath, QString token)
{

    QStringList paramList;
    taskInfo.curSize = 0;
    isupload = false;
    transToken = token;
    manager = new QNetworkAccessManager(this->parent());
//    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    fileMd5 = QByteArray::fromHex(info.MD5.toLocal8Bit());
    taskInfo.fileName = info.FILE_NAME;
    taskInfo.fileSize = info.SIZE;
    fileId = info.ID;

    if(downLoadPath.isEmpty())
        pFile = new QFile(netConf->getDownloadPath() + info.FILE_NAME, this->parent());
    else
        pFile = new QFile(downLoadPath + info.FILE_NAME, this->parent());
    if(!pFile->open(QFile::ReadWrite))
    {
        qDebug()<<"[File download]:"<<"file open error.";
        return;
    }

    paramList<<QString("fid=%1&").arg(info.ID)<<QString("token=%1&").arg(token)<<QString(APP_ID)+"&";
    sign = getSign(paramList);


    nUrl = QString(HTTP_ADDR) + "/api/file/checkDownload?"+QString("fid=%1&sign=%2&token=%3").arg(info.ID).arg(QString(sign.toHex())).arg(transToken);
    nUrl+="&"+QString(APP_ID);
    qDebug()<<"[checkDownload]"<<nUrl;
    netReply = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(netReply, SIGNAL(readyRead()), this, SLOT(getServerAddr()));
}

QByteArray netWork::getSign(QStringList param)
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

TaskInfo netWork::taskinfo()
{
    return taskInfo;
}

void netWork::taskStart()
{
    if(isupload)
    {
        md5Check();
        taskInfo.taskState = UPLOAD_STATE;
    }
    else
    {
        if(taskInfo.taskState == ERROR_STATE)
        {
//            taskInfo.curSize = 0;
//            netReply = manager->get(QNetworkRequest(QUrl(nUrl)));
        }
        else
        {
                taskInfo.taskState = DOWNLOAD_STATE;
                netReply = manager->get(QNetworkRequest(QUrl(nUrl)));
                qDebug()<<"[download]:"<<nUrl;
                connect(netReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));
                connect(netReply, SIGNAL(readyRead()), this, SLOT(fileRecv()));
                connect(netReply, SIGNAL(finished()), this, SLOT(fileRecvFinished()));
        }
    }
}

QString netWork::getTaskSpeed()
{
    int i = 0;
    int num = 0;
    quint64 speed = taskInfo.taskSpeed;
    QStringList l;
    QString fTime;
    l<<"B/s"<<"KB/s"<<"MB/s"<<"GB/s";

    while((num=speed/1000) && (i<3))
    {
        speed = num;
        i++;
    }
    if(taskInfo.taskSpeed)
    {
        taskInfo.finishTime = QTime(0,0,0).addSecs(taskInfo.fileSize / taskInfo.taskSpeed);
        fTime = taskInfo.finishTime.toString();
    }
    else
        fTime = "--:--:--";
    qDebug()<<"finish time"<<fTime;
    taskInfo.taskSpeed = 0;
    return fTime+"  "+QString::number(speed)+l.at(i);
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
    quint64 bytesToWrite;
    quint64 bytesWriten = 0;
    quint64 blockSize = 4*1024;
    QByteArray qba;
    QCryptographicHash ch(QCryptographicHash::Md5);

    if(!pFile->open(QFile::ReadOnly))
    {
        qDebug()<<"[File upload]:"<<"file open error.";
        return QByteArray();
    }
    bytesToWrite = f->size();qDebug()<<f->size();

    while(bytesToWrite > 0)
    {
        qba = f->read(qMin(bytesToWrite, blockSize));
        ch.addData(qba);
        bytesToWrite -= qba.size();
        bytesWriten += qba.size();
        qba.resize(0);
    }
    pFile->close();

//    qDebug()<<"MD5"<<ch.result().toHex();
    return ch.result();
}

int netWork::fileUpload(bool)
{
    quint64 ret = 0;
    QString nUrl = QString(HTTP_ADDR) + "/api/file/upload";
    request.setUrl(nUrl);
    request.setRawHeader("Content-Type", contentType.toLocal8Bit());
    if(chunk >= chunks)
    {
        qDebug()<<"[netWork]:Upload finishi.";
        taskInfo.taskState = FINISHI_STATE;
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
                taskInfo.taskState = FINISHI_STATE;
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
                taskInfo.taskSpeed += chunksize;
                taskInfo.curSize += chunksize;
                taskInfo.taskSpeed += chunksize;
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
    taskInfo.taskState = ERROR_STATE;
    if(pFile->isOpen())
    {
        pFile->close();
    }
    qDebug()<<"remove"<<pFile->remove();
}

void netWork::getServerAddr()
{
    QJsonDocument parseDoc;
    QJsonParseError jError;

    QByteArray qba = netReply->readAll();qDebug()<<"[getServerAddr]"<<qba;
    parseDoc = QJsonDocument::fromJson(qba, &jError);

    nUrl = QString(HTTP_ADDR) + "/api/file/download?"+QString("fid=%1&sign=%2&token=%3").arg(fileId).arg(QString(sign.toHex())).arg(transToken);
    nUrl+="&"+QString(APP_ID);
    disconnect(netReply, SIGNAL(readyRead()), this, SLOT(getServerAddr()));

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
                    nUrl = value.toString() + "/api/file/download?"+QString("fid=%1&sign=%2").arg(fileId).arg(QString(sign.toHex()));
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
    emit transReady();
}

void netWork::fileRecv()
{
    QByteArray qba = netReply->readAll();qDebug()<<"download:"<<qba.size();qDebug()<<qba;


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


    taskInfo.curSize+=qba.size();
    taskInfo.taskSpeed += qba.size();
    pFile->write(qba);
}

void netWork::fileRecvFinished()
{
    if(pFile->isOpen())
    {
        pFile->close();
    }
    qDebug()<<"[netTrans]:"<<pFile->fileName()<<"download finished.";
    if(getMd5(pFile) == fileMd5)
    {
        taskInfo.taskState = FINISHI_STATE;qDebug()<<"check pass";
    }
    else
        taskInfo.taskState = ERROR_STATE;
}

/****************************************************************************************************/
netTrans::netTrans(QObject *parent):
    QObject(parent)
{
    Thread = new QThread(this);
    pmanager = new QNetworkAccessManager(this);
    work = new netWork();
    readyTrans = false;
    work->moveToThread(Thread);
    Thread->start();
    connect(work, SIGNAL(transReady()), this, SLOT(transReady()));
}

int netTrans::netUpload(QString fileName, double pId)
{
    work->netUpload(fileName, pId);
    return 0;
}

void netTrans::netDownload(fileInfo info, QString downLoadPath, QString token)
{
    work->netDownload(info, downLoadPath, token);
    return;
}

void netTrans::taskStart()
{
    if(readyTrans)
        work->taskStart();
    else
        readyTrans = true;
}

QString netTrans::getTaskSpeed()
{
    return work->getTaskSpeed();
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

void netTrans::transReady()
{
    if(readyTrans)
        work->taskStart();
    else
        readyTrans = true;
}
