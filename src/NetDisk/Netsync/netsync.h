#ifndef NETSYNC_H
#define NETSYNC_H

#include <QObject>
#include <qfilesystemwatcher.h>
#include <qdir.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QList>
#include "netconfig.h"
#include "Http/nethttp.h"

#define FILE_SYNC ".sync"

class NetSync:public QObject
{
    Q_OBJECT
public:
    NetSync(QObject *parent = 0);
    void setNetClient(NetHttp* cli);

public slots:
    void syncAll();
    void loginSync(bool);

private:
    QFileSystemWatcher netWatcher;
    QList<QFileInfo*> listLocalReal;
    QList<syncLocalInfo*> listLocal;

    void initWatcher();
    void syncLocalGet();
    void syncLocalRead();
    void syncLocalWrite(QList<syncLocalInfo *> l);

    QJsonObject jSync;
    NetHttp* netClient;
    syncTable syncT;

private slots:
    void syncDirChanged(QString dir);
    void syncInfoRecv(QList<syncInfo*>, QDateTime);
};

#endif // NETSYNC_H
