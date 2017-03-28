#ifndef NETTRANS_H
#define NETTRANS_H

#include <QObject>
#include <QThread>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QSemaphore>
#include "Http/nethttp.h"
//#define HTTP_ADDR "http://120.24.216.97:8888"
#define HTTP_ADDR "http://120.76.52.78:8800/LinkRealSkyDrive"
#define APP_ID    "appId=PC123987456"
#define APP_KEY     "secretKey=d9563ff28bca607fa367deb13cc45ca2"

#define CHUNK_SIZE (4*1024*1024)
class fileInfo;

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
    quint64 fileSize;
    quint64 curSize;
    TaskState taskState;
    quint64 taskSpeed;
    QTime finishTime;
};

class netWork : public QObject
{
    Q_OBJECT
public:
    explicit netWork(QObject *parent = 0);
    void netDownload(fileInfo info, QString downLoadPath, QString token);
    int netUpload(QString fileName, double pId);
    TaskInfo taskinfo();
    void taskStart();
    QString getTaskSpeed();
    ~netWork();


private:
    QNetworkAccessManager* manager;
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
    QByteArray getSign(QStringList param);
signals:
    void taskFinish(int ret);
    void transReady();

private slots:
    void replyFinished(QNetworkReply* reply);
    void replyError(QNetworkReply::NetworkError errorCode);
    void getServerAddr();
    void fileRecv();
    void fileRecvFinished();
};

class netTrans : public QObject
{
    Q_OBJECT
public:
    explicit netTrans(QObject *parent = 0);
    int netUpload(QString fileName, double pId);
    void netDownload(fileInfo info, QString downLoadPath, QString token);
    void taskStart();
    QString getTaskSpeed();
    TaskInfo taskinfo();
    ~netTrans();

public slots:
    void transReady();

private:
    netWork* work;
    QNetworkAccessManager* pmanager;
    QThread* Thread;
    bool readyTrans;
};

#endif // NETTRANS_H
