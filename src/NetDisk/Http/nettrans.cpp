#include "nettrans.h"
#include <QtDebug>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <qthread.h>
#include <QHttpMultiPart>
#include "netconfig.h"

netWork::netWork(QObject *parent) :
    QObject(parent)
{
    md5CheckOk = true;
    netState = 0;
    isupload = false;
    pFile = NULL;
    manager = netConf->manager;
    managerUpload = netConf->manager;
    netReply = NULL;
    taskInfo.taskState = NO_STATE;
    taskInfo.taskSpeed = 0;
    taskInfo.curSize = 0;
    taskInfo.waitForDelete = false;

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

int netWork::netUpload(QString fileName, double pId, QString token, double fId)
{
    qDebug()<<fileName<<pId<<fId;
    netState = 0;
    taskInfo.curSize = 0;
    transToken = token;

//    if(manager == NULL)
//    {
//        manager = netConf->manager;
//        manager = new QNetworkAccessManager(this->parent());
//        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
//    }

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
    taskInfo.parentId = pId;
    taskInfo.filePath = fileName;
    taskInfo.taskId = fId;
    pFile->close();

    emit transReady();
    return 0;
}

void netWork::netDownload(fileInfo info, QString downLoadPath, QString token)
{
    QStringList paramList;
    taskInfo.curSize = 0;
    isupload = false;
    transToken = token;
    manager = netConf->manager;//new QNetworkAccessManager(this->parent());
//    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    fileMd5 = QByteArray::fromHex(info.MD5.toLocal8Bit());
    taskInfo.fileName = info.FILE_NAME;
    taskInfo.fileSize = info.SIZE;
    taskInfo.taskId = info.ID;

    fileId = info.ID;

    if(downLoadPath.isEmpty())
    {
        pFile = new QFile(netConf->getDownloadPath() + info.FILE_NAME, this->parent());
        taskInfo.filePath = netConf->getDownloadPath() + info.FILE_NAME;
    }
    else
    {
        pFile = new QFile(downLoadPath + "/" + info.FILE_NAME, this->parent());
        taskInfo.filePath = downLoadPath + "/" + info.FILE_NAME;
    }
//    if(!pFile->open(QFile::ReadWrite))
//    {
//        qDebug()<<"[File download]:"<<"file open error.";
//        return;
//    }
//    pFile->close();

    paramList<<QString("fid=%1&").arg(info.ID)<<QString("token=%1&").arg(token)<<QString(APP_ID)+"&";
    sign = getSign(paramList);


    nUrl = netConf->getServerAddress() + "/api/file/checkDownload?"+QString("fid=%1&sign=%2&token=%3").arg(info.ID).arg(QString(sign.toHex())).arg(transToken);
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

QByteArray netWork::getPost(QStringList param)
{
    QByteArray postData = QByteArray();
    qSort(param.begin(), param.end());
    QString str = QString();
    for(int i=0; i<param.count(); i++)
    {
        str += param.at(i);
    }
    str += QString("token=%1&").arg(transToken);
    postData += str.toUtf8();
    str += QString(APP_KEY);
//    qDebug()<<"[sign params]"<<str;
    QByteArray sign = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5);
//    qDebug()<<sign.toHex();
    postData += QString("sign=%1").arg(QString(sign.toHex())).toLocal8Bit();
    qDebug()<<"[post params]"<<postData;
    return postData;
}

TaskInfo netWork::taskinfo()
{
    return taskInfo;
}

void netWork::taskStart()
{qDebug("taskStart");
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
                if(!pFile->open(QFile::ReadWrite))
                {
                    qDebug()<<"[File download]:"<<"file open error.";
                    return;
                }
                taskInfo.taskState = DOWNLOAD_STATE;
                netReply = manager->get(QNetworkRequest(QUrl(nUrl)));
                qDebug()<<"[download]:"<<nUrl;
                connect(netReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));
                connect(netReply, SIGNAL(readyRead()), this, SLOT(fileRecv()));
                connect(netReply, SIGNAL(finished()), this, SLOT(fileRecvFinished()));
                connect(netReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(getDownloadProgress(qint64,qint64)));
        }
    }
}

QString netWork::getTaskSpeed()
{
    int i = 0;
    int num = 0;
    taskMutex.lock();
    quint64 speed = taskInfo.taskSpeed;
    if(speed)
        timeoutCount = 0;
    else if(timeoutCount++>10)
    {
        taskInfo.taskState = ERROR_STATE;
        taskInfo.waitForDelete = true;
    }

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
        taskInfo.finishTime = QTime(0,0,0).addSecs((taskInfo.fileSize-taskInfo.curSize) / taskInfo.taskSpeed);
        fTime = taskInfo.finishTime.toString();
    }
    else
        fTime = "--:--:--";
//    qDebug()<<"finish time"<<fTime;
    taskInfo.taskSpeed = 0;
    taskMutex.unlock();
    return fTime+"  "+QString::number(speed)+l.at(i);
}

int netWork::getTaskProgress()
{
    taskMutex.lock();
    int ret = taskInfo.curSize*100/taskInfo.fileSize;
    taskMutex.unlock();
    return ret;
}

void netWork::setTaskInfo(double parentId)
{
    taskMutex.lock();
    taskInfo.parentId = parentId;
    taskMutex.unlock();
}

void netWork::netPost(QNetworkRequest postRequest, QByteArray postData)
{
    qDebug()<<"up post1";
    netReply = managerUpload->post(postRequest, postData);
    connect(netReply, SIGNAL(finished()), this, SLOT(uploadRelpy()));
}

void netWork::netPost(QNetworkRequest postRequest, QHttpMultiPart *postData)
{
    qDebug()<<"up post";
    netReply = managerUpload->post(postRequest, postData);
    connect(netReply, SIGNAL(finished()), this, SLOT(uploadRelpy()));
}

void netWork::waitForDelete()
{
    taskInfo.waitForDelete = true;
}

void netWork::md5Check()
{
//    quint64 bytesToWrite = fInfo.size();
//    quint64 bytesWriten = 0;
//    quint64 blockSize = 4*1024;
    QByteArray qba;
    QStringList params;
    QCryptographicHash ch(QCryptographicHash::Md5);

//    pFile->open(QFile::ReadWrite);
    QByteArray md5Input = (fInfo.fileName()+QString::number(fInfo.size())).toUtf8();

    ch.addData(md5Input);
//    while(bytesToWrite > 0)
//    {
//        qba = pFile->read(qMin(bytesToWrite, blockSize));
//        ch.addData(qba);
//        bytesToWrite -= qba.size();
//        bytesWriten += qba.size();
//        qba.resize(0);
//    }
//    pFile->seek(0);

    fileMd5 = ch.result();
    params<<QString("md5=%1&").arg(QString(fileMd5.toHex()))<<QString("pid=%1&").arg(filepId)<<QString(APP_ID)+"&";
    qba = getPost(params);
    qDebug()<<"info"<<md5Input<<"MD5"<<ch.result().toHex();
    QString nUrl = netConf->getServerAddress() + "/api/file/getFileByMd5";
    request.setUrl(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    netReply = manager->post(request, qba);
    connect(netReply, SIGNAL(finished()), this, SLOT(uploadRelpy()));
    qDebug()<<"post"<<nUrl<<qba;
}


QByteArray netWork::getMd5(QFile *f)
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
    QByteArray md5buf = (taskinfo().fileName+QString::number(f->size())).toUtf8();
    ch.addData(md5buf);
    qDebug()<<md5buf;
    qDebug()<<"MD5"<<ch.result().toHex();
    qDebug()<<"fileMd5"<<fileMd5.toHex();
    return ch.result();
}

int netWork::fileUpload(bool)
{
    quint64 ret = 0;
    QString nUrl = netConf->getServerAddress() + "/api/file/upload";
    request.setUrl(nUrl);
    request.setRawHeader("Content-Type", contentType.toLocal8Bit());
    if(chunk >= chunks)
    {
        if(pFile->isOpen())
            pFile->close();
        qDebug()<<"[netWork]:Upload finishi.";
//        taskInfo.taskState = FINISHI_STATE;
//        emit taskUpFinish(taskInfo);
        return 0;
    }
    qDebug("%d/%d",chunk,chunks);

    chunksize = qMin(bytesToLoad, CHUNK_SIZE);

    QByteArray bond;
    QByteArray send;

    send.append(boundary.toLocal8Bit());
    bond = (crlf + boundary).toLocal8Bit();
    send.append(QString("Content-Disposition: form-data; name=\"chunksize\""+crlf));
    send.append(crlf);
    send.append(QString::number(chunksize));

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"token\""+crlf));
    send.append(crlf);
    send.append(transToken);

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"chunk\""+crlf));
    send.append(crlf);
    send.append(QString::number(chunk));

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"chunks\""+crlf));
    send.append(crlf);
    send.append(QString::number(chunks));

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"size\""+crlf));
    send.append(crlf);
    send.append(QString::number(fileSize));

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"pid\""+crlf));
    send.append(crlf);
    send.append(QString::number(filepId));

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
    QByteArray buffer = pFile->read(chunksize);
    send.append(buffer);
    ret = send.size()-ret;
    send.append(crlf);
    send.append(crlf);

    send.append(endBoundary);
    qDebug()<<"[payload]:"<<ret;
    qDebug()<<endBoundary;
    chunk++;
    bytesToLoad -= chunksize;
    emit needPost(request, send);
//    qDebug()<<"up post1";
//    netReply = managerUpload->post(request, send);qDebug()<<"up post2";
//    connect(netReply, SIGNAL(finished()), this, SLOT(uploadRelpy()));
    return 0;
}

int netWork::netFileUpload()
{
//    quint64 ret = 0;
    QString nUrl = netConf->getServerAddress() + "/api/file/upload";
    QHttpMultiPart* multiSend = new QHttpMultiPart;
    QNetworkRequest multiRequest(nUrl);
    multiSend->setContentType(QHttpMultiPart::FormDataType);
    multiSend->setBoundary(QByteArray("----WebKitFormBoundaryBPanjV2kGW1tw4ah"));

    if(chunk >= chunks)
    {
        if(pFile->isOpen())
            pFile->close();
        qDebug()<<"[netWork]:Upload finishi.";
        taskMutex.lock();
        taskInfo.taskState = FINISHI_STATE;
        taskMutex.unlock();
//        emit taskUpFinish(taskInfo);
        return 0;
    }
    qDebug("%d/%d",chunk,chunks);
    emit DownloadProgress(100*chunk/chunks);
    chunksize = qMin(bytesToLoad, CHUNK_SIZE);
//    chunksize = CHUNK_SIZE;

    QHttpPart chunksize_part;
    chunksize_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"chunksize\""));
    chunksize_part.setBody(QString::number(CHUNK_SIZE).toLocal8Bit());
    multiSend->append(chunksize_part);

    QHttpPart fileMd5_part;
    fileMd5_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"md5\""));
    fileMd5_part.setBody(fileMd5.toHex());
    multiSend->append(fileMd5_part);

    QHttpPart filePid_part;
    filePid_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"pid\""));
    filePid_part.setBody(QString::number(filepId).toLocal8Bit());
    multiSend->append(filePid_part);

    taskMutex.lock();
    if(taskInfo.taskId)
    {
        QHttpPart fileId_part;
        fileId_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"fid\""));
        fileId_part.setBody(QString::number(taskInfo.taskId).toLocal8Bit());
        multiSend->append(fileId_part);
    }
    taskMutex.unlock();

//    QHttpPart path_part;
//    path_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"path\""));
//    multiSend->append(path_part);

//    QHttpPart id_part;
//    id_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"id\""));
//    id_part.setBody(QByteArray("WU_FILE_0"));
//    multiSend->append(id_part);

//    QHttpPart filename_part;
//    filename_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
//    filename_part.setBody(QFileInfo(*pFile).fileName().toUtf8());
//    multiSend->append(filename_part);

//    QHttpPart type_part;
//    type_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"type\""));
//    type_part.setBody(QByteArray("text/plain"));
//    multiSend->append(type_part);

//    QHttpPart date_part;
//    date_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"lastModifiedDate\""));
//    date_part.setBody(QByteArray("Tue Apr 11 2017 14:01:34 GMT+0800 (中国标准时间)"));
//    multiSend->append(date_part);

    QHttpPart token_part;
    token_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"token\""));
    token_part.setBody(transToken.toLocal8Bit());
    multiSend->append(token_part);

    QHttpPart chunk_part;
    chunk_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"chunk\""));
    chunk_part.setBody(QString::number(chunk).toLocal8Bit());
    multiSend->append(chunk_part);

    QHttpPart chunks_part;
    chunks_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"chunks\""));
    chunks_part.setBody(QString::number(chunks).toLocal8Bit());
    multiSend->append(chunks_part);

    QHttpPart fileSize_part;
    fileSize_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"size\""));
    fileSize_part.setBody(QString::number(fileSize).toLocal8Bit());
    multiSend->append(fileSize_part);

    QHttpPart file_part;
    file_part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    file_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"file\";filename=\"%1\"").arg(QFileInfo(*pFile).fileName())));
    QByteArray buffer = pFile->read(chunksize);
    file_part.setBody(buffer);
    multiSend->append(file_part);

    chunk++;
    bytesToLoad -= chunksize;
    emit needPost(multiRequest, multiSend);

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
        qDebug()<<"[reply]"<<qba;
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
                taskMutex.lock();
                taskInfo.taskState = FINISHI_STATE;
                QJsonObject obj = result.toObject();
                taskInfo.taskId = obj.take("ID").toDouble();
                emit taskUpFinish(taskInfo);
                qDebug()<<"file exit"<<taskInfo.taskId;
                taskMutex.unlock();
            }
            else if(code == "-200")//文件MD5不存在
            {
//                if(managerUpload == NULL)
//                {
//                    managerUpload = new QNetworkAccessManager(this);
//                    connect(managerUpload, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
//                }
                    if(!pFile->open(QFile::ReadWrite))
                    {
                        qDebug()<<"[File download]:"<<"file open error.";
                        return;
                    }
                qDebug("file not exit upload");
                chunks = (fInfo.size()) / CHUNK_SIZE + ((fInfo.size() % CHUNK_SIZE) != 0);
                chunk = 0;
                bytesToLoad = fInfo.size();
                fileSize = bytesToLoad;
                qDebug()<<"fileSize"<<fileSize;
                netState = 1;//进入上传文件状态
                //fileUpload(false);
                netFileUpload();
            }
        }
        else if(netState == 1)//接收文件上传返回结果
        {
            if(code == "200")//单片上传成功
            {
                taskMutex.lock();
                taskInfo.taskSpeed += chunksize;
                taskInfo.curSize += chunksize;
                taskInfo.taskSpeed += chunksize;
                taskMutex.unlock();
//                fileUpload(false);
                netFileUpload();
            }
            if(!result.isNull())
            {
                QJsonObject obj = result.toObject();
                taskMutex.lock();
                taskInfo.taskId = obj.take("ID").toDouble();
                qDebug()<<"upload task over"<<taskInfo.taskId;
                taskMutex.unlock();
                emit taskUpFinish(taskInfo);
            }
        }
    }
    else
    {
        qDebug()<<"[down]:"<<qba.size();
        pFile->write(qba);
    }
    reply->deleteLater();
    return;
}

void netWork::uploadRelpy()
{qDebug()<<"uploadRelpy";
    QByteArray qba = netReply->readAll();
    disconnect(netReply, SIGNAL(finished()), this, SLOT(uploadRelpy()));
    netReply->deleteLater();

    if(isupload)
    {
        QJsonDocument parseDoc;
        QJsonParseError jError;
        QJsonValue jval;
        QJsonValue result;
        QString code;
        QString msg;
        qDebug()<<"[upload reply]"<<qba;
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
                taskMutex.lock();
                taskInfo.taskState = FINISHI_STATE;
                QJsonObject obj = result.toObject();
                taskInfo.taskId = obj.take("ID").toDouble();
                taskMutex.unlock();
                emit taskUpFinish(taskInfo);
                qDebug()<<"file exit"<<taskInfo.taskId;
            }
            else //if(code == "-200")//文件MD5不存在
            {
//                if(managerUpload == NULL)
//                {
//                    managerUpload = new QNetworkAccessManager(this);
//                    connect(managerUpload, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
//                }
                if(!pFile->open(QFile::ReadWrite))
                {
                    qDebug()<<"[File download]:"<<"file open error.";
                    return;
                }
                qDebug("file not exit upload");
                chunks = (fInfo.size()) / CHUNK_SIZE + ((fInfo.size() % CHUNK_SIZE) != 0);
                chunk = 0;
                bytesToLoad = fInfo.size();
                fileSize = bytesToLoad;
                qDebug()<<"fileSize"<<fileSize;
                netState = 1;//进入上传文件状态
//                fileUpload(false);
                netFileUpload();
            }
        }
        else if(netState == 1)//接收文件上传返回结果
        {
            if(code == "200")//单片上传成功
            {
                taskMutex.lock();
                taskInfo.taskSpeed += chunksize;
                taskInfo.curSize += chunksize;
                taskInfo.taskSpeed += chunksize;
                taskMutex.unlock();
//                fileUpload(false);
                netFileUpload();
            }
            if(!result.isNull())
            {
                QJsonObject obj = result.toObject();
                if(!obj.contains("ID"))
                    return;
                if(obj.contains("FILE_NAME"))
                    taskInfo.fileName = obj.take("FILE_NAME").toString();
                qDebug()<<pFile->fileName()<<taskinfo().fileName;
                QFileInfo upfileInfo(*pFile);
                if(upfileInfo.fileName() != taskInfo.fileName)
                {
                    pFile->rename(upfileInfo.absolutePath()+"/"+taskInfo.fileName);
                }
                qDebug()<<pFile->fileName()<<taskinfo().fileName;
                taskMutex.lock();
                taskInfo.filePath = QFileInfo(*pFile).absoluteFilePath();
                taskInfo.taskId = obj.take("ID").toDouble();
                taskInfo.taskState = FINISHI_STATE;
                taskMutex.unlock();
                qDebug()<<"[UPLOAD OVER]"<<taskInfo.taskId;
                emit taskUpFinish(taskInfo);
            }
        }
    }
    else
    {
        qDebug()<<"[down]:"<<qba.size();
        pFile->write(qba);
    }
    return;
}

void netWork::replyError(QNetworkReply::NetworkError errorCode)
{
    qDebug()<<"[replyError]"<<errorCode;
    taskMutex.lock();
    taskInfo.taskState = ERROR_STATE;
    taskInfo.waitForDelete = true;
    taskMutex.unlock();
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

    nUrl = netConf->getServerAddress() + "/api/file/download?"+QString("fid=%1&sign=%2&token=%3").arg(fileId).arg(QString(sign.toHex())).arg(transToken);
    nUrl+="&"+QString(APP_ID);
    disconnect(netReply, SIGNAL(readyRead()), this, SLOT(getServerAddr()));
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
            if(obj.contains("code"))
            {
                QJsonValue value = obj.take("code");
                if(value.toString() == "-200")
                {
                    taskMutex.lock();
                    taskInfo.taskState = ERROR_STATE;
                    taskInfo.waitForDelete = true;
                    taskMutex.unlock();
                    return;
                }
            }
        }
    }
    emit transReady();
}

void netWork::fileRecv()
{
    QByteArray qba = netReply->readAll();//qDebug()<<"download:"<<qba.size();

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
    taskMutex.lock();
    taskInfo.curSize+=qba.size();
    taskInfo.taskSpeed += qba.size();
    taskMutex.unlock();
    pFile->write(qba);
    //qDebug()<<"write";
}

void netWork::fileRecvFinished()
{
    if(pFile->isOpen())
    {
        pFile->close();
    }
    netReply->deleteLater();
    qDebug()<<"[netTrans]:"<<pFile->fileName()<<"download finished.";
    taskMutex.lock();
    if(getMd5(pFile) == fileMd5)
    {
        taskInfo.taskState = FINISHI_STATE;qDebug()<<"check pass";
    }
    else
        taskInfo.taskState = ERROR_STATE;
    qDebug("1");
    taskMutex.unlock();    qDebug("2");
    emit taskFinish(taskInfo);    qDebug("3");
    delete pFile;    qDebug("4");
}

void netWork::getDownloadProgress(qint64 curBytes, qint64 totalBytes)
{
//    qDebug()<<"[download]"<<curBytes<<"/"<<totalBytes;
    emit DownloadProgress(100*curBytes/totalBytes);
}

/****************************************************************************************************/
netTrans::netTrans(QObject *parent):
    QObject(parent)
{
    Thread = new QThread(this);
    work = new netWork();
    readyTrans = false;
    isStart = false;
    work->moveToThread(Thread);
    Thread->start();
    connect(work, SIGNAL(transReady()), this, SLOT(transReady()));
    connect(work, SIGNAL(taskFinish(TaskInfo)), this, SIGNAL(taskFinished(TaskInfo)));
    connect(work, SIGNAL(DownloadProgress(int)), this, SIGNAL(downloadProgress(int)));
    connect(work, SIGNAL(taskUpFinish(TaskInfo)), this, SIGNAL(taskUpFinished(TaskInfo)));
    connect(work, SIGNAL(needPost(QNetworkRequest,QByteArray)), this, SLOT(netPost(QNetworkRequest,QByteArray)));
    connect(work, SIGNAL(needPost(QNetworkRequest,QHttpMultiPart*)), this, SLOT(netPost(QNetworkRequest,QHttpMultiPart*)));
}

int netTrans::netUpload(QString fileName, double pId, QString token, double fId)
{
    if(fId == 0)
        work->netUpload(fileName, pId, token);
    else
        work->netUpload(fileName, pId, token, fId);
    return 0;
}

void netTrans::netDownload(fileInfo info, QString downLoadPath, QString token)
{
    work->netDownload(info, downLoadPath, token);
    return;
}

void netTrans::netDownload(syncInfo info, QString downLoadPath, QString token)
{
    fileInfo fInfo;
    fInfo.FILE_NAME = info.FILE_NAME;
    fInfo.ID = info.ID;
    fInfo.SIZE = info.SIZE;
    fInfo.MD5 = info.MD5;
    work->netDownload(fInfo, downLoadPath,token);
    work->setTaskInfo(info.PARENT_ID);
    return;
}

void netTrans::taskStart()
{
    if(isStart)
        return;
    isStart = true;
    if(readyTrans)
        work->taskStart();
    else
        readyTrans = true;
}

bool netTrans::taskIsStart()
{
    return isStart;
}

QString netTrans::getTaskSpeed()
{
    return work->getTaskSpeed();
}

int netTrans::getTaskProgress()
{
    return work->getTaskProgress();
}

TaskInfo netTrans::taskinfo()
{
    return work->taskinfo();
}

void netTrans::waitForDelete()
{
    work->waitForDelete();
}

netTrans::~netTrans()
{
    Thread->quit();
    Thread->wait();

    delete work;
}

void netTrans::transReady()
{
    if(readyTrans)
        work->taskStart();
    else
        readyTrans = true;
}

void netTrans::netPost(QNetworkRequest postRequest, QByteArray postData)
{
    work->netPost(postRequest, postData);
}

void netTrans::netPost(QNetworkRequest postRequest, QHttpMultiPart *postData)
{
    work->netPost(postRequest, postData);
}

