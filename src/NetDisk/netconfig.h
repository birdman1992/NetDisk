#ifndef NETCONFIG_H
#define NETCONFIG_H

#include <QObject>
#include <qfile.h>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>

class NetConfig
{
public:
    NetConfig();
    void setDownloadPath(QString path);
    QString getDownloadPath();
    void setMaxTaskNum(int num);
    int getMaxTaskNum();
    void saveAll();
private:
    QJsonDocument jdoc;
    QString downloadPath;
    int maxTaskNum;

    void creatDir();
    void creatConfig();
    void readConfig();
    void saveConfig();

};
extern NetConfig* netConf;
#endif // NETCONFIG_H
