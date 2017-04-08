#ifndef NETDONWLOADER_H
#define NETDONWLOADER_H

#include <QObject>
#include <QThread>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <QString>
#include <QFile>
#include <QFileInfo>
//#define HTTP_ADDR "http://120.2 4.216.97:8888"
//#define HTTP_ADDR "http://120.76.52.78:8800/LinkRealSkyDrive"
//#define HTTP_ADDR "http://192.168.0.55:8888"
//#define APP_ID    "appId=PC123987456"
//#define APP_KEY     "secretKey=d9563ff28bca607fa367deb13cc45ca2"

#define CHUNK_SIZE (1024*100)
enum DownloadState
{
    DOWN_NEW,
    DOWN_GETADDR,
    DOWN_LOADING,
    DOWN_FINISH,
    DOWN_ERROR
};


class DownloadInfo
{
public:
    QString fileName;//必要参数
    QString filePath;
    QString downLoadPath;
    QString token;//必要参数
    QString fileMd5;//必要参数
    quint64 fileSize;
    quint64 curSize;
    DownloadState taskState;
    quint64 taskSpeed;
    QTime finishTime;
    double taskId;//必要参数
    double parentId;//必要参数
};

class downloadWork : public QObject
{
    Q_OBJECT
public:
    explicit downloadWork(QNetworkAccessManager* downloadManager,QObject *parent = 0);
    ~downloadWork();
    void initTask(DownloadInfo* info);
    void taskStart();

private:
    QNetworkAccessManager* manager;
    QNetworkReply* netReply;
    DownloadInfo* downloadInfo;
    QNetworkRequest request;
    QByteArray sign;
    QFile* pFile;
    QString downloadAddr;
    QByteArray getSign(QStringList param);
    QByteArray getPost(QStringList param);
    QByteArray getMd5(QFile *f);
signals:
    void taskFinished(DownloadInfo* info);

private slots:
    void getDownloadAddr();

    void fileRecv();
    void fileRecvFinished();
    void replyError(QNetworkReply::NetworkError errorCode);
};

class NetDonwloader : public QObject
{
    Q_OBJECT
public:
    explicit NetDonwloader(QObject *parent = 0);
    bool downloaderIsLeisure();//下载器是否空闲
    bool addTask(DownloadInfo* info);
    ~NetDonwloader();
public slots:


signals:


private:
    bool isLeisure;//下载器是否空闲
    QThread* Thread;
    downloadWork* work;
    QNetworkAccessManager* manager;
    DownloadInfo* downloadInfo;
};

#endif // NETDONWLOADER_H
