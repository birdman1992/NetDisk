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
#include "Http/nettrans.h"

#define FILE_SYNC ".sync"
#define FILE_SYNC_DATE ".date"


class NetSync:public QObject
{
    Q_OBJECT
public:
    NetSync(QObject *parent = 0);
    void setNetClient(NetHttp* cli);
    syncTable* getTable();

public slots:
    void syncAll();
    void loginSync(bool);
    void syncHostFinished();

private:
    QDateTime syncDate;
    QFileSystemWatcher netWatcher;
    QList<QFileInfo*> listLocalReal;
    QList<syncLocalInfo*> listLocal;
    QList<syncLocalInfo*> listchanged;
    QList<syncLocalInfo*> listSyncUpload;
    QList<netTrans*> taskDownload;
    QList<netTrans*> taskUpload;
    int taskNum;
    int uptask;

    void initWatcher();
    void syncLocalGet();
    void syncLocalRead();
    void syncLocalWrite(QList<syncLocalInfo *> l);
    void syncDateRead();
    void syncDateWrite(QDateTime date);
    double getIdByPath(QString path);
    QString getLocalPath(QString path);

    QJsonObject jSync;
    NetHttp* netClient;
    syncTable syncT;

private slots:
    void syncDirChanged(QString dir);
    void syncInfoRecv(QList<syncInfo*>, QDateTime);
    void syncHostPointSave(QDateTime);
    void syncLocalUpdate();
    void syncTaskDownload();
    void syncTaskUpload();
    void taskDownloadFinished(TaskInfo info);
    void taskUploadFinished(TaskInfo info);
};

#endif // NETSYNC_H
