#include "netsync.h"
#include <QtDebug>
#include <QString>
#include <QDirIterator>
#include <QDateTime>

NetSync::NetSync(QObject *parent):
  QObject(parent)
{
    initWatcher();
    syncLocalRead();
    syncLocalGet();
}

void NetSync::setNetClient(NetHttp *cli)
{
    netClient = cli;
    syncT.setHttpClient(cli);
    connect(netClient, SIGNAL(syncUpdate(QList<syncInfo*>,QDateTime)), this, SLOT(syncInfoRecv(QList<syncInfo*>,QDateTime)));

}

void NetSync::syncAll()
{
    netClient->netSync(-1);
}

void NetSync::loginSync(bool isLogin)
{
    if(isLogin)
        netClient->netSync(-1);
}

void NetSync::initWatcher()
{
    netWatcher.addPath(netConf->getSyncPath());
    connect(&netWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(syncDirChanged(QString)));
}

void NetSync::syncLocalGet()
{
    QDirIterator iter(netConf->getSyncPath(), QDirIterator::Subdirectories);

    while(!listLocalReal.isEmpty())
    {
        QFileInfo *info = listLocalReal.takeFirst();
        delete info;
    }

    while (iter.hasNext())
    {
        iter.next();
        QFileInfo* info = new QFileInfo(iter.fileInfo());
        listLocalReal<<info;
    }
}

void NetSync::syncLocalRead()
{
    QFile* f = new QFile(netConf->getSyncPath()+QString(FILE_SYNC));
    if(!f->exists())
    {
        syncT.setLocalList(listLocal);
        return;
    }
    else
        f->open(QFile::ReadOnly);

    while(!listLocal.isEmpty())
    {
        syncLocalInfo* info = listLocal.takeFirst();
        delete info;
    }

    QByteArray qba;
    qba.resize(1024);
    while(0 < f->readLine(qba.data(),qba.size()))
    {
        qDebug()<<"[local read]"<<qba;
        syncLocalInfo* info = new syncLocalInfo;
        QStringList lInfo = QString(qba).split("\t");
        info->fileId = QString(lInfo.at(0)).toDouble();
        info->syncPath = lInfo.at(1);
        info->fileName = lInfo.at(2);
        info->fileMd5 = lInfo.at(3);
        info->fileSize = QString(lInfo.at(4)).toLongLong();
        info->isDir = QString(lInfo.at(5)).toInt();
        listLocal<<info;
    }
    syncT.setLocalList(listLocal);
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

void NetSync::syncDirChanged(QString dir)
{
    qDebug("Local dir changed!");
}

void NetSync::syncInfoRecv(QList<syncInfo *>sInfo, QDateTime sTime)
{
    qDebug("syncInfoRecv");
    syncT.syncInfoInsert(sInfo);
}
