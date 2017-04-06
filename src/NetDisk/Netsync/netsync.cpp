#include "netsync.h"
#include <QtDebug>
#include <QString>
#include <QDirIterator>
#include <QDateTime>

NetSync::NetSync(QObject *parent):
  QObject(parent)
{
    syncDate = QDateTime();
    taskNum = 0;
    uptask = 0;

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
    connect(netClient, SIGNAL(loginStateChanged(bool)), this, SLOT(loginSync(bool)));
    connect(netClient, SIGNAL(syncHostPoint(QDateTime)), this, SLOT(syncHostPointSave(QDateTime)));

    connect(&syncT, SIGNAL(hostSyncFinished()), this, SLOT(syncHostFinished()));
    connect(&syncT, SIGNAL(localListChanged()), this, SLOT(syncLocalUpdate()));
    connect(&syncT, SIGNAL(syncDownload()), this, SLOT(syncTaskDownload()));
    connect(&syncT, SIGNAL(syncUpload()), this, SLOT(syncTaskUpload()));
}

syncTable *NetSync::getTable()
{
    return &syncT;
}

void NetSync::syncAll()
{
    netClient->netSync(SYNC_ID);
}

void NetSync::loginSync(bool isLogin)
{
    if(isLogin)
        netClient->netSync(SYNC_ID, syncT.syncTime);
}

void NetSync::syncHostFinished()
{
    qDebug("\n\n\n\n\n\n\n\n");
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

    while(!syncT.list_loacl_real.isEmpty())
    {
        QFileInfo *info = syncT.list_loacl_real.takeFirst();
        delete info;
    }

    while (iter.hasNext())
    {
        iter.next();
        QFileInfo* info = new QFileInfo(iter.fileInfo());
        if(info->fileName() == ".sync")
            continue;
        syncT.list_loacl_real<<info;
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
    else
        f->open(QFile::ReadOnly);

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
    QJsonDocument doc;
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
    return;
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
    syncLocalGet();
    syncT.creatSyncUploadList();
    qDebug("Local dir changed!");
}

void NetSync::syncInfoRecv(QList<syncInfo *>sInfo, QDateTime sTime)
{
    qDebug("syncInfoRecv");
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
    syncDateWrite(sTime);
    syncT.syncTime = sTime;
    syncT.syncHostToLocal();
    syncT.creatSyncUploadList();
}

void NetSync::syncLocalUpdate()
{qDebug("syncLocalUpdate");
    syncLocalWrite(syncT.list_local);
}

void NetSync::syncTaskDownload()
{
    netTrans* trans;
    syncInfo* info;
//    int i = 0;
    taskNum = 0;
    emit syncStateChanged(true);
//    syncT.isSyncing = true;
    qDebug()<<"download:"<<syncT.list_sync_download.count();
    while((!syncT.list_sync_download.isEmpty()) && taskDownload.count()<=3)
    {
        trans = new netTrans;
        info = syncT.list_sync_download.takeFirst();
        qDebug()<<"[sync down]"<<info->PARENT_ID<<syncT.getPathById(info->PARENT_ID)<<info->FILE_NAME;
        trans->netDownload(info, syncT.getPathById(info->PARENT_ID),netClient->netToken());
        trans->taskStart();
        connect(trans, SIGNAL(taskFinished(TaskInfo)), this, SLOT(taskDownloadFinished(TaskInfo)));
        taskDownload<<trans;
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

void NetSync::syncTaskUpload()
{
    netTrans* trans = new netTrans;
    syncInfo* info;
    int i = 0;
    uptask = 0;

    qDebug()<<"upload:"<<syncT.list_sync_upload.count();
    for(i=0; i<syncT.list_sync_upload.count(); i++)
    {
        trans = new netTrans;
        info = syncT.list_sync_upload.at(i);
        qDebug()<<"[sync up]"<<info->PARENT_ID<<syncT.getPathById(info->PARENT_ID)<<info->FILE_NAME;
        trans->netUpload(info->FILE_NAME, info->PARENT_ID, netClient->netToken());
        connect(trans, SIGNAL(taskUpFinished(TaskInfo)), this, SLOT(taskUploadFinished(TaskInfo)));
        taskUpload<<trans;
        trans->taskStart();
    }
    while((uptask < 3) && (uptask<taskUpload.count()))
    {
        taskUpload.at(uptask++)->taskStart();
    }
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
            sInfo->syncPath = info.filePath;qDebug()<<"[date 2]"<<QFileInfo(sInfo->syncPath).absoluteFilePath()<<QFileInfo(sInfo->syncPath).lastModified();
            sInfo->lastDate = QFileInfo(sInfo->syncPath).lastModified();
            syncT.list_local<<sInfo;
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
    syncT.reportSyncNum();
    if(syncT.list_sync_download.count() == 0)
        syncStateChanged(false);
    downloadNum(syncT.list_sync_download.count());
    syncT.setLocalList();
//    syncLocalWrite(syncT.list_local);
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
//            disconnect(trans, SIGNAL(taskFinished(TaskInfo)), this, SLOT(taskUploadFinished(TaskInfo)));
            QByteArray md5Input = (info.fileName+QString::number(info.fileSize)).toUtf8();
            ch.addData(md5Input);
            sInfo->fileId = info.taskId;
            sInfo->fileMd5 = QString(ch.result().toHex());
            sInfo->fileName = info.fileName;
            sInfo->fileSize = info.fileSize;
            sInfo->isDir = 0;
            sInfo->parentId = info.taskId;
            sInfo->syncPath = info.filePath;
            sInfo->lastDate = QFileInfo(sInfo->syncPath).lastModified();
            syncT.list_local<<sInfo;
            delete trans;
            break;
        }
    }
    i=0;
    while(i<taskUpload.count())
    {
        if(taskUpload.at(i)->taskIsStart())
        {
            i++;
            continue;
        }
        taskUpload.at(i)->taskStart();
        break;
    }
    if(taskUpload.count() == 0)
        syncT.creatSyncUploadList();

    syncT.reportSyncNum();
    syncT.setLocalList();
    syncLocalWrite(syncT.list_local);
    qDebug()<<"[taskUpload]"<<taskUpload.count()<<syncT.list_local.count();
}
