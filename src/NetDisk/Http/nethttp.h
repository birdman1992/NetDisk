#ifndef NETHTTP_H
#define NETHTTP_H

#include <QObject>
#include <qnetworkaccessmanager.h>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QByteArray>
#include "Http/nettrans.h"
#include "Http/nethttp.h"
#include "netconfig.h"
#define SYNC_ID -1
class netTrans;
class NetHttp;

enum httpState
{
    H_LOGIN,
    H_LIST,
    H_DOWNLOAD,
    H_UPLOAD,
    H_NEW,
    H_DEL,
    H_SHARE,
    H_SYNC
};

class syncLocalInfo
{
public:
    QString syncPath;
    QString fileName;
    QString fileMd5;
    quint64 fileSize;
    double fileId;
    double parentId;
    int isDir;
    QDateTime lastDate;
};

class syncInfo
{
public:
    syncInfo();
    syncInfo(syncInfo* info);
    QDateTime LAST_MOD_TIME;
    int USER_ID;
    int FILE_SERVER;
    QString MD5;
    QDateTime ADD_TIME;
    QString EXT;
    int STATUS;
    QString FILE_NAME;
    int VERSION;
    QString REAL_KEY;
    int FILE_TYPE;
    QString FILE_CODE;
    double ID;
    double MASTER_ID;
    int IS_ENCRYPED;
    double SIZE;
    double PARENT_ID;
    QString FILE_PATH;
    int TYPE;
    QString LOCAL_PATH;
};

class syncTable: public QObject
{
    Q_OBJECT
public:
    syncTable();
    void setLocalList();
    void setHttpClient(NetHttp* client);
    void syncHostToLocal();
    void syncLocalToHost();
    void syncInfoInsert(QList<syncInfo*> info);
    void syncNextDir();
    void updateParentDate(double id);
    void reportSyncNum();
    void creatSyncUploadList();
    syncInfo* getHostInfoById(double Id);
    double getIdByName(QString name, bool *isChanged=NULL);//从本地文件信息缓存中查询对应文件（夹）的ID
    QString getPathById(double Id);//从本地文件信息缓存中查询ID对应的路径
    QList<syncInfo*> getHostList();//获取服务端文件列表
    QList<syncInfo*> list_task;//遍历任务链表
    QList<syncLocalInfo*> list_local;
    QList<QFileInfo*> list_loacl_real;
    QList<syncInfo*> list_sync_download;//同步下载链表
    QList<syncInfo*> list_sync_upload;//同步上传链表
    QDateTime syncTime;
    bool isSyncing;//是否正在同步

private:
    QList<syncInfo*> list_all;//host端所有更新文件
    QList<syncInfo*> list_temp;//host端单层目录所有更新文件
    QList<syncInfo*> list_dir;//host端所有更新目录
    QList<syncInfo*> list_file;//host端所有更新文件

    QStringList list_index;//host端所有更新文件的id，和list_all一一对应
    QStringList list_local_index;//本地所有文件id，和list_local一一对应
    QStringList list_path;//同步路径表
    QString cur_path;
    NetHttp* syncClient;
    bool syncAll;//是否遍历同步所有目录
    void setSyncAll(bool syncAllDir);
    void setCurPath(double Id);
    QString getDirPath(double Id);
    void syncDir();
    void syncFile();
    void nextTask();
    void recvListClear();
    void tempListToHostList();

    void creatSyncDownloadList();
    void clearSyncList();
signals:
    void localListChanged();
    void hostSyncFinished();
    void syncUpload();
    void syncDownload();
    void syncUploadChanged(int uploadNum);
    void syncDownloadChanged(int uploadNum);
};

class fileInfo
{

public:
    fileInfo();
    fileInfo(const fileInfo &info);
    double ID;
    int MAST_ID;
    int USER_ID;
    double PARENT_ID;
    int STATUS;
    int VERSION;
    int IS_ENCRYPED;
    int FILE_TYPE;
    int TYPE;
    quint64 SIZE;
    QDateTime LAST_MOD_TIME;
    QDateTime ADD_TIME;
    QString MD5;
    QString EXT;
    QString FILE_SERVER;
    QString REAL_KEY;
    QString REAL_NAME;
    QString FILE_PATH;
    QString FILE_NAME;
};

class NetHttp : public QObject
{
    Q_OBJECT
public:
    explicit NetHttp(QObject *parent = 0);
    void netLogin(QString user, QString passwd);
    void netList(double pId=-1, int cPage=1, int pageSize=20, int showdelete=0, QString name=QString(), QString fileType=QString());
    void netMkdir(double pId=-1, QString fileName = QString("新建文件夹"));
    void netUpload(QString fileName, double pId);
    void netDownload(fileInfo info, QString downloadPath = QString());
    void netDelete(double fId);
    void netCreatShareLinks(QStringList fids);
    void netSync(double pId, QDateTime lastSyncTime=QDateTime());
    void syncTraversal();//遍历同步
    QString netToken();

private:
    QNetworkAccessManager* manager;
    QNetworkAccessManager* managerSync;
    httpState State;
    netTrans* fTrans;//上传下载
    QList<netTrans*>listTask;//任务队列
    QDateTime serverTime;
    QString token;
    bool isLastPage;
    bool isFirstPage;
    bool needLoginSync;
    int currentPageNum;
    int totalRow;
    int totalPage;
    double lastSyncId;
    QList<fileInfo*> listInfo;
    QList<syncInfo*> listSync;

    QString httpDateTran(QByteArray raw);
    void fileInfoRecv(QByteArray info);
    void fileInfoShow(fileInfo* info);
    void fileListClear();
    void syncListClear();
    void callbackNew(QByteArray info);
    void loginRst(QByteArray rst);
    void syncInfoRecv(QByteArray info, QDateTime syncTime);
    void syncListCreat(QJsonArray info, QDateTime syncTime);
    QByteArray getSign(QStringList param);
    QByteArray getPost(QStringList param);

private slots:
    void replyFinished(QNetworkReply*);
    void replySyncFinished(QNetworkReply*);

signals:
    void listUpdate(QList<fileInfo*>);
    void updateRequest();
    void newTask(netTrans*);
    void loginStateChanged(bool);
    void pageChanged(bool isFirst,bool isLast,int pageNum,int totalPageNum);
    void syncUpdate(QList<syncInfo*>, QDateTime);
    void syncHostPoint(QDateTime);
    void isTraversal(bool);

public slots:
};

#endif // NETHTTP_H
