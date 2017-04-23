#include "netsync.h"
#include <QDebug>
#include <QString>
#include <QDirIterator>
#include <QDateTime>
#include <QProcess>

NetSync::NetSync(QObject *parent):
  QObject(parent)
{
    syncDate = QDateTime();
    taskNum = 0;
    uptask = 0;
    isSyncing = false;
    syncTimer = NULL;
    connect(this, SIGNAL(syncStateChanged(bool)), this, SLOT(syncfinish(bool)));

    syncDateRead();
    initWatcher();
    syncLocalRead();
    syncLocalGet();
}

void NetSync::setNetClient(NetHttp *cli)
{
    netClient = cli;
    syncT.setHttpClient(cli);
    connect(netClient, SIGNAL(syncUpdate(QList<syncInfo*>,QDateTime)), this, SLOT(syncInfoRecv(QList<syncInfo*>,QDateTime)));
    connect(netClient, SIGNAL(needSync()), this, SLOT(loginSync()));
    connect(netClient, SIGNAL(syncHostPoint(QDateTime)), this, SLOT(syncHostPointSave(QDateTime)));

    connect(&syncT, SIGNAL(hostSyncFinished()), this, SLOT(syncHostFinished()));
    connect(&syncT, SIGNAL(localListChanged()), this, SLOT(syncLocalUpdate()),Qt::DirectConnection);
    connect(&syncT, SIGNAL(syncDownload()), this, SLOT(syncDownloadStart()));
    connect(&syncT, SIGNAL(syncUpload()), this, SLOT(syncUploadStart()));
    connect(&syncT, SIGNAL(needUpdateDate()), this, SLOT(dateFileUpdate()));
}

syncTable *NetSync::getTable()
{
    return &syncT;
}

void NetSync::syncAll()
{
    netClient->netSync(SYNC_ID);
}

void NetSync::loginSync()
{
    if(isSyncing)
        return;
    isSyncing = true;
    netClient->netSync(SYNC_ID, syncT.syncTime);
}

void NetSync::syncHostFinished()
{
    qDebug("asdfg\n\n\n\n\n\n\n\n");
    netClient->netSync(SYNC_ID, syncDate);
}

void NetSync::initWatcher()
{
    netWatcher.addPath(netConf->getSyncPath());
    connect(&netWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(syncDirChanged(QString)));
}

void NetSync::syncLocalGet()
{
    QDirIterator iter(netConf->getSyncPath(),QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot,
                      QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    while(!syncT.list_local_real.isEmpty())
    {
        QFileInfo *info = syncT.list_local_real.takeFirst();
        delete info;
    }

    while (iter.hasNext())
    {
        iter.next();
        QFileInfo* info = new QFileInfo(iter.fileInfo());
        if(info->fileName() == ".sync")
            continue;
        if(info->fileName() == ".date")
            continue;
        syncT.list_local_real<<info;
        if(info->isDir())
            syncT.list_local_real_dir<<info;
//        qDebug()<<info->absoluteFilePath()<<info->isReadable()<<info->isWritable()<<info->size();
//        qDebug()<<info->fileName()<<info->absolutePath()<<info->absoluteFilePath();
    }
}

void NetSync::syncLocalRead()
{
    QFile* f = new QFile(netConf->getSyncPath()+QString(FILE_SYNC));
    if(!f->exists())
    {
        syncT.setLocalList();
        return;
    }
    else if(!f->open(QFile::ReadOnly))
    {
        qDebug()<<netConf->getSyncPath()+QString(FILE_SYNC)<<"open failed";
    }


    while(!syncT.list_local.isEmpty())
    {
        syncLocalInfo* info = syncT.list_local.takeFirst();
        delete info;
    }

    QByteArray qba;
//    qba.resize(1024);
    while(1)
    {
        qba = f->readLine();
        if(qba.isEmpty())
            break;
//        qDebug()<<"[local read]"<<qba;
        syncLocalInfo* info = new syncLocalInfo;
        QStringList lInfo = QString::fromLocal8Bit(qba.left(qba.size()-1)).split("\t");
        info->parentId = QString(lInfo.at(0)).toDouble();
        info->fileId = QString(lInfo.at(1)).toDouble();
        info->syncPath = lInfo.at(2);
        info->fileName = info->syncPath.section('/',-1);
        info->fileMd5 = lInfo.at(3);
        info->fileSize = QString(lInfo.at(4)).toLongLong();
        info->isDir = QString(lInfo.at(5)).toInt();
        info->lastDate = QDateTime::fromString(lInfo.at(6), "yyyy-MM-dd hh:mm:ss");
        syncT.list_local<<info;
    }
    syncT.setLocalList();
//    syncT.getIdByName("C:/Users/Administrator/Desktop/同步测试/C");
}

void NetSync::syncLocalWrite(QList<syncLocalInfo *> l)
{
//    QJsonDocument doc;
    syncT.isSyncing = true;
    QFile* f = new QFile(netConf->getSyncPath()+QString(FILE_SYNC));
    f->open(QFile::WriteOnly);

    for(int i=0; i<l.count(); i++)
    {
        syncLocalInfo* info = l.at(i);
        QString str = QString::number(info->parentId) +"\t"+ QString::number(info->fileId) +"\t"+ info->syncPath+"\t"+ info->fileMd5 +"\t"+ QString::number(info->fileSize) +"\t"+
                QString::number(info->isDir) +"\t" + info->lastDate.toString("yyyy-MM-dd hh:mm:ss")+"\n";
        f->write(str.toLocal8Bit());
    }

    f->close();

    QFileInfo finfo(*f);

    if(!finfo.isHidden())
    {
        hideFile(f);
    }
    syncT.isSyncing = false;
    delete f;
}

void NetSync::syncDateRead()
{
    QFile* f = new QFile(netConf->getSyncPath()+QString(FILE_SYNC_DATE));
    if(!f->exists())
    {
        syncT.syncTime = QDateTime();
        return;
    }
    else
        f->open(QFile::ReadOnly);

    syncT.syncTime = QDateTime::fromString(QString(f->readAll()),"yyyy-MM-dd hh:mm:ss");
    qDebug()<<"[SYNC time read]"<<syncT.syncTime.toString("yyyy-MM-dd hh:mm:ss");
}

void NetSync::syncDateWrite(QDateTime date)
{
    QFile* f = new QFile(netConf->getSyncPath()+QString(FILE_SYNC_DATE));
    f->open(QFile::WriteOnly);

    if(date.isNull())
    {
        f->close();
        return;
    }

    QString str = date.toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<<"[SYNC time write]"<<str;
    f->write(str.toLocal8Bit());
    f->close();

    if(!QFileInfo(*f).isHidden())
        hideFile(f);
    return;
}

void NetSync::syncUploadStart()
{
    netTrans* trans = new netTrans;
    syncInfo* info;
    int i = 0;
    uptask = 0;
    emit syncStateChanged(true);
    qDebug()<<"upload:"<<syncT.list_sync_upload.count();

    while((!syncT.list_sync_upload.isEmpty()) && taskUpload.count()<=netConf->getMaxTaskNum())
    {
        trans = new netTrans;
        info = syncT.list_sync_upload.takeFirst();
        qDebug()<<"[sync up]"<<info->PARENT_ID<<syncT.getPathById(info->PARENT_ID)<<info->FILE_NAME;
        trans->netUpload(info->FILE_NAME, info->PARENT_ID, netClient->netToken(), info->ID);
        connect(trans, SIGNAL(taskUpFinished(TaskInfo)), this, SLOT(taskUploadFinished(TaskInfo)));
        taskUpload<<trans;
        trans->taskStart();
    }

//    for(i=0; i<syncT.list_sync_upload.count(); i++)
//    {
//        trans = new netTrans;
//        info = syncT.list_sync_upload.at(i);
//        qDebug()<<"[sync up]"<<info->PARENT_ID<<syncT.getPathById(info->PARENT_ID)<<info->FILE_NAME;
//        trans->netUpload(info->FILE_NAME, info->PARENT_ID, netClient->netToken());
//        connect(trans, SIGNAL(taskUpFinished(TaskInfo)), this, SLOT(taskUploadFinished(TaskInfo)));
//        taskUpload<<trans;
//        trans->taskStart();
    //    }
}

void NetSync::syncDownloadStart()
{
    netTrans* trans;
    syncInfo* info;
    QString downloadPath;
//    int i = 0;
    taskNum = 0;
    emit syncStateChanged(true);
//    syncT.isSyncing = true;
    qDebug()<<"download:"<<syncT.list_sync_download.count();
    while((!syncT.list_sync_download.isEmpty()) && taskDownload.count()<=netConf->getMaxTaskNum())
    {
        trans = new netTrans;qDebug("1");
        info = syncT.list_sync_download.takeFirst();qDebug("2");
        downloadPath = syncT.getPathById(info->PARENT_ID);qDebug("3");
        qDebug()<<"[sync down]"<<info->PARENT_ID<<downloadPath<<info->FILE_NAME;qDebug("4");
        trans->netDownload(info, downloadPath,netClient->netToken());
        trans->taskStart();
        connect(trans, SIGNAL(taskFinished(TaskInfo)), this, SLOT(taskDownloadFinished(TaskInfo)));
        taskDownload<<trans;
        syncT.list_download_task<<downloadPath+"/"+info->FILE_NAME;
    }
//    for(i=0; i<syncT.list_sync_download.count(); i++)
//    {
//        trans = new netTrans;
//        info = syncT.list_sync_download.at(i);
//        qDebug()<<"[sync down]"<<info->PARENT_ID<<syncT.getPathById(info->PARENT_ID)<<info->FILE_NAME;
//        trans->netDownload(info, syncT.getPathById(info->PARENT_ID),netClient->netToken());
//        connect(trans, SIGNAL(taskFinished(TaskInfo)), this, SLOT(taskDownloadFinished(TaskInfo)));
//        taskDownload<<trans;
//    }
//    while((taskNum < 3) && (taskNum<taskDownload.count()))
//    {
//        taskDownload.at(taskNum++)->taskStart();
//    }
}

void NetSync::hideFile(QFile *file)
{
    QString str = file->fileName().replace('/','\\').toLatin1();
    QProcess process;
    process.start("attrib", QStringList()<<"+h"<<str);
    qDebug()<<str<<process.waitForFinished(1000);
}

QString NetSync::getLocalPath(QString path)
{
    int offset = netConf->getSyncPath().length();
    if(path.length() < offset)
        return QString();

    return path.right(path.length() - offset);
}

void NetSync::syncDirChanged(QString dir)
{
    if(syncT.isSyncing)
        return;
    qDebug()<<"Local dir changed!"<<dir;
    syncLocalGet();

    if(syncTimer == NULL)
    {
        syncTimer = new QTimer;
        syncTimer->start(10000);
        connect(syncTimer, SIGNAL(timeout()), this, SLOT(syncTimeOut()));//20s
    }
    else
    {
        syncTimer->setInterval(10000);
    }
}

void NetSync::syncTimeOut()
{
    disconnect(syncTimer, SIGNAL(timeout()), this, SLOT(syncTimeOut()));
    delete syncTimer;
    syncTimer = NULL;
    syncLocalGet();
    qDebug()<<"AAAA";
    syncT.getDownloadTaskNum();
    syncT.getUploadTaskNum();
//    syncT.syncMkDir();
//    syncT.creatSyncDownloadList();
//    syncT.creatSyncUploadList();
}

void NetSync::syncInfoRecv(QList<syncInfo *>sInfo, QDateTime sTime)
{
    qDebug("syncInfoRecv");
    syncT.syncInfoNeedUpdate = false;
    syncDate = sTime;
    syncT.syncInfoInsert(sInfo);
}

void NetSync::syncHostPointSave(QDateTime sTime)
{
    if(!syncT.list_task.isEmpty())
    {
        syncT.syncNextDir();
        return;
    }

    isSyncing = false;
    syncT.syncTime = sTime;

    if(!(syncT.getDownloadTaskNum() || syncT.getUploadTaskNum()))
    {
        syncDateWrite(sTime);
    }

    if(netConf->autoSyncDir())
    {
        syncT.syncHostToLocal();
        syncT.syncLocalToHost();
    }

    if((syncT.list_sync_download.count() == 0) && (syncT.list_sync_upload.count() == 0))
    {
        syncDateWrite(sTime);
    }
}

void NetSync::syncLocalUpdate()
{qDebug("syncLocalUpdate");
    syncLocalWrite(syncT.list_local);
}

void NetSync::syncTaskDownload()
{
    syncT.syncDir();
}

void NetSync::syncTaskUpload()
{
    syncT.syncMkDir();
}

void NetSync::taskDownloadFinished(TaskInfo info)
{
    int i=0;
    netTrans* trans;
    QCryptographicHash ch(QCryptographicHash::Md5);

    for(i=0; i<taskDownload.count(); i++)
    {
        if(taskDownload.at(i)->taskinfo().taskId == info.taskId)
        {
            trans = taskDownload.takeAt(i);
            syncT.list_download_task.removeAt(i);
            syncLocalInfo* sInfo = new syncLocalInfo;
            disconnect(trans, SIGNAL(taskFinished(TaskInfo)), this, SLOT(taskDownloadFinished(TaskInfo)));
            QByteArray md5Input = (info.fileName+QString::number(info.fileSize)).toUtf8();
            ch.addData(md5Input);
            sInfo->fileId = info.taskId;
            sInfo->fileMd5 = QString(ch.result().toHex());
            sInfo->fileName = info.fileName;
            sInfo->fileSize = info.fileSize;
            sInfo->isDir = 0;
            sInfo->parentId = info.parentId;
            sInfo->syncPath = info.filePath;
            sInfo->lastDate = QFileInfo(sInfo->syncPath).lastModified();
//            syncT.list_local<<sInfo;
            syncT.addSyncLocalInfo(sInfo);
            syncT.updateParentDate(sInfo->parentId);
            delete trans;
            syncLocalWrite(syncT.list_local);

            syncTaskDownload();

            break;
        }
    }
    i=0;

//    syncT.isSyncing = false;
//    syncT.creatSyncUploadList();
    syncT.setLocalList();
    syncT.reportSyncNum();
    if((syncT.list_sync_download.count() == 0) && (syncT.list_sync_upload.count() == 0))
    {
        syncLocalGet();
        emit syncStateChanged(false);
    }
    qDebug()<<"[taskDownload]"<<taskDownload.count()<<syncT.list_local.count();
}

void NetSync::taskUploadFinished(TaskInfo info)
{
    int i=0;
    netTrans* trans;
    QCryptographicHash ch(QCryptographicHash::Md5);

    for(i=0; i<taskUpload.count(); i++)
    {
        if(taskUpload.at(i)->taskinfo().taskId == info.taskId)
        {
            trans = taskUpload.takeAt(i);
            syncLocalInfo* sInfo = new syncLocalInfo;
            disconnect(trans, SIGNAL(taskFinished(TaskInfo)), this, SLOT(taskUploadFinished(TaskInfo)));
            QByteArray md5Input = (info.fileName+QString::number(info.fileSize)).toUtf8();
            ch.addData(md5Input);
            sInfo->fileId = info.taskId;
            sInfo->fileMd5 = QString(ch.result().toHex());
            sInfo->fileName = info.fileName;
            sInfo->fileSize = info.fileSize;
            sInfo->isDir = 0;
            sInfo->parentId = info.parentId;
            sInfo->syncPath = info.filePath;
            sInfo->lastDate = QFileInfo(sInfo->syncPath).lastModified();
//            syncT.list_local<<sInfo;
            syncT.addSyncLocalInfo(sInfo);
//            qDebug()<<sInfo->fileName;
            delete trans;
            syncLocalWrite(syncT.list_local);
            syncUploadStart();
            break;
        }
    }

    if(taskUpload.count() == 0)
    {
        qDebug()<<"CCCC";
        syncT.creatSyncUploadList();
    }

    syncT.setLocalList();
    syncT.reportSyncNum();
    qDebug()<<"[taskUpload]"<<taskUpload.count()<<syncT.list_sync_upload.count();
    if((syncT.list_sync_download.count() == 0) && (syncT.list_sync_upload.count() == 0))
        emit syncStateChanged(false);
}

void NetSync::syncfinish(bool isSyncing)
{
    if(!isSyncing)
        syncDateWrite(syncT.syncTime);
}

void NetSync::dateFileUpdate()
{
    syncDateWrite(syncT.syncTime);
}

