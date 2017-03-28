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
    void syncInfoInsert(QList<syncInfo*> info);
    syncInfo* getHostInfoById(double Id);
    QList<syncInfo*> getHostList();//获取服务端文件列表
    QList<syncLocalInfo*> list_local;
    QList<QFileInfo*> list_loacl_real;
    QDateTime syncTime;

private:
    QList<syncInfo*> list_all;
    QList<syncInfo*> list_temp;
    QList<syncInfo*> list_dir;
    QList<syncInfo*> list_file;
    QList<syncInfo*> list_task;
    QStringList list_index;
    QStringList list_local_index;
    QStringList list_path;//同步路径表
    QString cur_path;
    NetHttp* syncClient;
    bool syncAll;//是否遍历同步所有目录
    void setSyncAll(bool syncAllDir);
    void setCurPath(double Id);
    void syncDir();
    void syncFile();
    void syncNextDir();
    void nextTask();
    void recvListClear();
    void tempListToHostList();
signals:
    void localListChanged(syncLocalInfo*);
    void hostSyncFinished();
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
    void isTraversal(bool);

public slots:
};

#endif // NETHTTP_H
