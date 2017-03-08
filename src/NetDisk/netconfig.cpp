#include "netconfig.h"
#include <QChar>
#include <QtDebug>

#define CONF_FILENAME "Netdisk.conf"

NetConfig* netConf;

NetConfig::NetConfig()
{
    downloadPath = "download/";
    maxTaskNum = 3;
    creatDir();
    creatConfig();
    readConfig();
}

void NetConfig::setDownloadPath(QString path)
{
    if(!path.endsWith("/"))
        path += "/";
    downloadPath = path;
}

QString NetConfig::getDownloadPath()
{
    return downloadPath;
}

void NetConfig::setMaxTaskNum(int num)
{
    maxTaskNum = num;
}

int NetConfig::getMaxTaskNum()
{
    return maxTaskNum;
}

void NetConfig::saveAll()
{
    saveConfig();
}


void NetConfig::creatDir()
{
    QDir* dir = new QDir;
    if(!dir->exists("conf"))
        dir->mkdir("conf");
    if(!dir->exists(downloadPath))
        dir->mkdir(downloadPath);

    delete dir;
    return;
}

void NetConfig::creatConfig()
{
    //设置默认配置
    downloadPath = "download/";
    maxTaskNum = 3;

    saveConfig();
}

void NetConfig::readConfig()
{
    QJsonParseError jError;
    QJsonValue jval;
    QFile* pFile = new QFile("conf/"+ QString(CONF_FILENAME));
    if(!pFile->open(QFile::ReadOnly))
    {
        qDebug()<<"[config file]:open error";
    }
    QByteArray qba = pFile->readAll();
    jdoc = QJsonDocument::fromJson(qba, &jError);

    if(jError.error == QJsonParseError::NoError)
    {
        if(jdoc.isObject())
        {
            qDebug()<<"[NetConfig]:";
            QJsonObject obj = jdoc.object();
            //读取下载路径
            if(obj.contains("downloadPath"))
            {
                jval = obj.take("downloadPath");
                downloadPath = jval.toString();
                qDebug()<<"downloadPath:"<<downloadPath;
            }
            else
            {
                creatConfig();
            }
            //读取最大同时执行任务数
            if(obj.contains("maxTaskNum"))
            {
                jval = obj.take("maxTaskNum");
                maxTaskNum = jval.toInt();
                qDebug()<<"maxTaskNum:"<<maxTaskNum;
            }
            else
            {
                creatConfig();
            }
            qDebug(" ");
        }
    }
    delete pFile;
}

void NetConfig::saveConfig()
{
    QFile* pFile = new QFile("conf/"+QString(CONF_FILENAME));

    if(pFile->exists())
    {
        delete pFile;
        return;
    }

    //插入默认配置内容
    QJsonObject obj;
    obj.insert("downloadPath",downloadPath);
    obj.insert("maxTaskNum",maxTaskNum);

    QJsonDocument doc;
    doc.setObject(obj);
    QByteArray qba = doc.toJson();qDebug()<<"[NetConfig]:"<<qba;
    pFile->open(QFile::WriteOnly);
    pFile->write(qba);
    pFile->close();

    delete pFile;
}
