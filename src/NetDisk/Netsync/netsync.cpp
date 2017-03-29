#include "netsync.h"
#include <QtDebug>
#include <QString>
#include <QDirIterator>
#include <QDateTime>

NetSync::NetSync(QObject *parent):
  QObject(parent)
{
    syncDate = QDateTime();

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
    connect(&syncT, SIGNAL(localListChanged()), this, SLOT(syncLocalUpdate()));
    connect(netClient, SIGNAL(loginStateChanged(bool)), this, SLOT(loginSync(bool)));
    connect(&syncT, SIGNAL(hostSyncFinished()), this, SLOT(syncHostFinished()));
    connect(netClient, SIGNAL(syncHostPoint(QDateTime)), this, SLOT(syncHostPointSave(QDateTime)));
}

void NetSync::syncAll()
{
    netClient->netSync(-1);
}

void NetSync::loginSync(bool isLogin)
{
    if(isLogin)
        netClient->netSync(428, syncT.syncTime);
}

void NetSync::syncHostFinished()
{
    qDebug("\n\n\n\n\n\n\n\n");
    netClient->netSync(428, syncDate);
}

void NetSync::initWatcher()
{
    netWatcher.addPath(netConf->getSyncPath());
    connect(&netWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(syncDirChanged(QString)));
}

void NetSync::syncLocalGet()
{
    QDirIterator iter(netConf->getSyncPath(), QDirIterator::Subdirectories);

    while(!syncT.list_loacl_real.isEmpty())
    {
        QFileInfo *info = syncT.list_loacl_real.takeFirst();
        delete info;
    }

    while (iter.hasNext())
    {
        iter.next();
        QFileInfo* info = new QFileInfo(iter.fileInfo());
        syncT.list_loacl_real<<info;
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
        qDebug()<<"[local read]"<<qba;
        syncLocalInfo* info = new syncLocalInfo;
        QStringList lInfo = QString::fromUtf8(qba.left(qba.size()-1)).split("\t");
        info->fileId = QString(lInfo.at(0)).toDouble();
        info->syncPath = lInfo.at(1);
        info->fileName = info->syncPath.section('/',-1);
        info->fileMd5 = lInfo.at(2);
        info->fileSize = QString(lInfo.at(3)).toLongLong();
        info->isDir = QString(lInfo.at(4)).toInt();
        syncT.list_local<<info;
    }
    syncT.setLocalList();
}

void NetSync::syncLocalWrite(QList<syncLocalInfo *> l)
{
    QJsonDocument doc;
    QFile* f = new QFile(netConf->getSyncPath()+QString(FILE_SYNC));
    f->open(QFile::WriteOnly);

    for(int i=0; i<l.count(); i++)
    {
        syncLocalInfo* info = l.at(i);
        QString str = QString::number(info->fileId) +"\t"+ info->syncPath + info->fileName +"\t"+ info->fileMd5 +"\t"+ QString::number(info->fileSize) +"\t"+ QString::number(info->isDir) +"\n";
        f->write(str.toLocal8Bit());
    }

    f->close();
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
    syncT.syncTime = sTime;
    syncT.syncHostToLocal();
}

void NetSync::syncLocalUpdate()
{qDebug("syncLocalUpdate");
    syncLocalWrite(syncT.list_local);
}
