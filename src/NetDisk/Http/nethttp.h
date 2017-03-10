#ifndef NETHTTP_H
#define NETHTTP_H

#include <QObject>
#include <qnetworkaccessmanager.h>
#include <QDateTime>
#include <QList>
#include "Http/nettrans.h"

class netTrans;

enum httpState
{
    H_LOGIN,
    H_LIST,
    H_DOWNLOAD,
    H_UPLOAD,
    H_NEW,
    H_DEL
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
    void netDownload(fileInfo info);
    void netDelete(double fId);

private:
    QNetworkAccessManager* manager;
    httpState State;
    netTrans* fTrans;//上传下载
    QList<netTrans*>listTask;//任务队列
    bool isLastPage;
    bool isFirstPage;
    int currentPageNum;
    int totalRow;
    int totalPage;
    QList<fileInfo*> listInfo;

    void fileInfoRecv(QByteArray info);
    void fileInfoShow(fileInfo* info);
    void fileListClear();
    void callbackNew(QByteArray info);
    void loginRst(QByteArray rst);

private slots:
    void replyFinished(QNetworkReply*);

signals:
    void listUpdate(QList<fileInfo*>);
    void updateRequest();
    void newTask(netTrans*);
    void loginStateChanged(bool);

public slots:
};

#endif // NETHTTP_H
