#ifndef NETCONFIG_H
#define NETCONFIG_H

#include <QObject>
#include <qfile.h>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <taesclass.h>
#include <qnetworkaccessmanager.h>

class NetConfig
{
public:
    NetConfig();
    QNetworkAccessManager* manager;
    void setServerAddress(QString address);
    QString getServerAddress();
    void setDownloadPath(QString path);
    QString getDownloadPath();
    void setMaxTaskNum(int num);
    int getMaxTaskNum();
    void setUsrname(QString usrname);
    QString getUsrname();
    void setPasswd(QString wd);
    QString getPasswd();
    void setSyncPath(QString path);
    QString getSyncPath();
    void setautoSync(bool autosync);
    bool autoSyncDir();

    void setremPassword(bool rempasswd);
    void setautoLogin(bool autologin);
    bool remPassword();
    bool autoLoginDisk();

    void saveAll();
private:
    TAesClass netAes;
    QJsonDocument jdoc;
    QString downloadPath;
    QString syncPath;
    int maxTaskNum;
    QString userName;
    QString passwd;
    QString passwdAes;
    QString serverAddress;
    bool remPasswd;
    bool autoLogin;
    bool autoSync;


    void creatDir();
    void creatConfig();
    void creatDefaultConfig();
    void readConfig();
    void saveConfig();

};
extern NetConfig* netConf;
#endif // NETCONFIG_H
