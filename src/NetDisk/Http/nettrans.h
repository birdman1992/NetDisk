#ifndef NETTRANS_H
#define NETTRANS_H

#include <QObject>
#include <QThread>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <QMutex>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QSemaphore>
#include "Http/nethttp.h"
//#define HTTP_ADDR "http://120.2 4.216.97:8888"
#define HTTP_ADDR "http://120.76.52.78:8800/LinkRealSkyDrive"
//#define HTTP_ADDR "http://192.168.0.55:8888"


#define CHUNK_SIZE (1024*100)
class fileInfo;
class syncInfo;

enum TaskState
{
    NO_STATE,
    DOWNLOAD_STATE,
    UPLOAD_STATE,
    FINISHI_STATE,
    ERROR_STATE
};

class TaskInfo
{
public:
    QString fileName;
    QString filePath;
    quint64 fileSize;
    quint64 curSize;
    TaskState taskState;
    quint64 taskSpeed;
    QTime finishTime;
    double taskId;
    double parentId;
    bool waitForDelete;//可以释放
};

class netWork : public QObject
{
    Q_OBJECT
public:
    explicit netWork(QObject *parent = 0);
    void netDownload(fileInfo info, QString downLoadPath, QString token);
    int netUpload(QString fileName, double pId, QString token, double fId=0);
    TaskInfo taskinfo();
    void taskStart();
    QString getTaskSpeed();
    int getTaskProgress();
    void setTaskInfo(double parentId);
    void netPost(QNetworkRequest postRequest, QByteArray postData);
    void netPost(QNetworkRequest postRequest, QHttpMultiPart* postData);
    void waitForDelete();
    ~netWork();


private:
    QNetworkAccessManager* manager;
    QNetworkAccessManager* managerUpload;
    QNetworkReply* netReply;
    QNetworkRequest request;
    QString contentType;
    QString endBoundary;
    QString boundary;
    QString crlf;
    QString b;
    QString nUrl;
    QString transToken;
    QFile* pFile;
    QFileInfo fInfo;
    QByteArray fileMd5;
    TaskInfo taskInfo;
    QByteArray sign;
    QMutex taskMutex;
    int timeoutCount;//超时计数

    double filepId;//文件父目录ID
    double fileId;//文件ID
//    QSemaphore semcheck;
//    QThread* thread;
    bool isupload;
    bool md5CheckOk;
    int chunks;//总分片数
    int chunk;//分片序号
    int chunksize;//单个分片大小
    int fileSize;//文件总大小
    int bytesToLoad;
    int netState;//网络状态
    void md5Check();
    QByteArray getMd5(QFile* f);
    int fileUpload(bool reload);
    int netFileUpload();
    QByteArray getSign(QStringList param);
    QByteArray getPost(QStringList param);
signals:
    void taskFinish(TaskInfo);
    void taskUpFinish(TaskInfo);
    void transReady();
    void needPost(QNetworkRequest postRequest, QByteArray postData);
    void needPost(QNetworkRequest postRequest, QHttpMultiPart* postData);
    void DownloadProgress(int);

private slots:
    void replyFinished(QNetworkReply* reply);
    void uploadRelpy();
    void replyError(QNetworkReply::NetworkError errorCode);
    void getServerAddr();
    void fileRecv();
    void fileRecvFinished();
    void getDownloadProgress(qint64,qint64);

};

class DownloadTaskInfo;

class netTrans : public QObject
{
    Q_OBJECT
public:
    explicit netTrans(QObject *parent = 0);
    int netUpload(QString fileName, double pId, QString token, double fId=0);
    void netDownload(fileInfo info, QString downLoadPath, QString token);
    void netDownload(syncInfo info, QString downLoadPath, QString token);
    void taskStart();
    bool taskIsStart();
    QString getTaskSpeed();
    int getTaskProgress();
    TaskInfo taskinfo();
    void waitForDelete();
    DownloadTaskInfo* task;
    ~netTrans();
public slots:
    void transReady();
    void netPost(QNetworkRequest postRequest, QByteArray postData);
    void netPost(QNetworkRequest postRequest, QHttpMultiPart* postData);

signals:
    void taskFinished(TaskInfo);
    void taskUpFinished(TaskInfo);
    void taskProgress(int);
    void downloadProgress(int);

private:
    netWork* work;
    QThread* Thread;
    bool readyTrans;
    bool isStart;
};

#endif // NETTRANS_H
