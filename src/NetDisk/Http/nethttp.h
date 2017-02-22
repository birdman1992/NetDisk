#ifndef NETHTTP_H
#define NETHTTP_H

#include <QObject>
#include <qnetworkaccessmanager.h>
#include <QDateTime>
#include <QList>

enum httpState
{
    H_LOGIN,
    H_LIST,
    H_DOWNLOAD,
    H_UPLOAD,
    H_NEW,
    H_DEL
};

struct fileInfo
{
    int ID;
    int MAST_ID;
    int USER_ID;
    int PARENT_ID;
    int STATUS;
    int VERSION;
    int IS_ENCRYPED;
    int FILE_TYPE;
    int TYPE;
    float SIZE;
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
    void netList(int pId=-1, int cPage=0, int pageSize=20, QString name=QString(), QString fileType=QString());

private:
    QNetworkAccessManager* manager;
    httpState State;
    bool isLastPage;
    bool isFirstPage;
    int currentPageNum;
    int totalRow;
    int totalPage;
    QList<fileInfo*> listInfo;

    void fileInfoRecv(QByteArray info);
    void fileInfoShow(fileInfo* info);
    void fileListClear();

private slots:
    void replyFinished(QNetworkReply*);

signals:
    void listUpdate(QList<fileInfo*>);
public slots:
};

#endif // NETHTTP_H
