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
#define HTTP_ADDR "http://120.24.216.97:8888"
#define CHUNK_SIZE (4*1024*1024)

class netWork : public QObject
{
    Q_OBJECT
public:
    explicit netWork(QObject *parent = 0);
    int netUpload(QString fileName, double pId);
    void netDownload(QString fileName, double fileId);
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
    QFile* pFile;
    QFileInfo FileInfo;
    QByteArray fileMd5;


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
    int fileUpload(bool reload);

signals:
    void taskFinish(int ret);

private slots:
    void replyFinished(QNetworkReply* reply);
    void replyError(QNetworkReply::NetworkError errorCode);
    void fileRecv();
    void fileRecvFinished();
};

class netTrans : public QObject
{
    Q_OBJECT
public:
    explicit netTrans(QObject *parent = 0);
    int netUpload(QString fileName, double pId);
    void netDownload(QString fileName, double fileId);
    ~netTrans();

private:
    netWork* work;
    QNetworkAccessManager* pmanager;
    QThread* Thread;
};

#endif // NETTRANS_H
