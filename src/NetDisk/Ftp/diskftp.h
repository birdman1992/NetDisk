#ifndef DISKFTP_H
#define DISKFTP_H

#include <QObject>
#include <QFtp/qftp.h>

class DiskFtp : public QObject
{
    Q_OBJECT
public:
    explicit DiskFtp(QObject *parent = 0);
    void ftpLogin(QString user, QString passwd);
    void ftpList(QString dir);
    void ftpMkdir(QString dir);
    void ftpRmdir(QString dir);


private:
    QFtp* netFtp;
    char cmdState;//0:断开 1:连接中 2:已连接
    QString ftpUser;
    QString ftpPasswd;
    QString ftpListDIR;
    QString _FromSpecialEncoding(const QString &InputStr);
    QString _ToSpecialEncoding(const QString &InputStr);

signals:
    void listInfo(QUrlInfo);
public slots:

private slots:
    void ftpCommandFinished(int cmdID,bool error);
};

#endif // DISKFTP_H
