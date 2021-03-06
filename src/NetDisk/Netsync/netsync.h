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
#include <QFile>
#include <QTimer>
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
    bool syncLocalCheck();

public slots:
    void syncAll();
    void loginSync();
    void syncHostFinished();
    void syncTaskDownload();
    void syncTaskUpload();
    void syncUploadStart();
    void syncDownloadStart();
private:
    QDateTime syncDate;
    QTimer* syncTimer;
    QTimer* checkTimer;
    QFileSystemWatcher netWatcher;
    QList<QFileInfo*> listLocalReal;
    QList<syncLocalInfo*> listLocal;
    QList<syncLocalInfo*> listchanged;
    QList<syncLocalInfo*> listSyncUpload;
    QList<netTrans*> taskDownload;
    QList<netTrans*> taskUpload;
    int taskNum;
    int uptask;
    bool isSyncing;//是否正在同步

    void initWatcher();
    void syncLocalGet();
    void syncLocalRead();
    void syncLocalWrite(QList<syncLocalInfo *> l);
    void syncDateRead();
    void syncDateWrite(QDateTime date);
    QDateTime getSyncDate();
    void hideFile(QFile* file);
    double getIdByPath(QString path);
    QString getLocalPath(QString path);

    QJsonObject jSync;
    NetHttp* netClient;
    syncTable syncT;

private slots:
    void syncDirChanged(QString dir);
    void syncTimeOut();
    void checkTimeOut();
    void syncInfoRecv(QList<syncInfo*>, QDateTime);
    void syncHostPointSave(QDateTime);
    void syncLocalUpdate();
    void taskDownloadFinished(TaskInfo info);
    void taskUploadFinished(TaskInfo info);
    void syncfinish(bool);
    void dateFileUpdate();//刷新.date的修改时间，让.date文件作为同步文件夹的更新时间参照


signals:
    void newDownloadTask(netTrans*, bool);
    void newUploadTask(netTrans*, bool);
    void syncStateChanged(bool);
};

#endif // NETSYNC_H
