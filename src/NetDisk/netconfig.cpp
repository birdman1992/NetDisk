#include "netconfig.h"
#include <QChar>
#include <QtDebug>
#include <QCryptographicHash>

#define CONF_FILENAME "Netdisk.conf"

NetConfig* netConf;

NetConfig::NetConfig()
{
    downloadPath = "download/";
    maxTaskNum = 3;
    userName = QString();
    passwd = QString();
    passwdAes = QString();
    remPasswd = false;
    autoLogin = false;
    netAes.InitializePrivateKey(18,(UCHAR*)"Linkrealby@Birdman");
    creatDir();
    creatDefaultConfig();
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

void NetConfig::setUsrname(QString usrname)
{
    userName = usrname;
}

QString NetConfig::getUsrname()
{
    return userName;
}

void NetConfig::setPasswd(QString wd)
{
    passwd = wd;
}

QString NetConfig::getPasswd()
{
    return passwd;
}

void NetConfig::setremPassword(bool rempasswd)
{
    remPasswd = rempasswd;
}

void NetConfig::setautoLogin(bool autologin)
{
    autoLogin = autologin;
}

bool NetConfig::remPassword()
{
    return remPasswd;
}

bool NetConfig::autoLoginDisk()
{
    return autoLogin;
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
{qDebug("sdadasda");
    saveConfig();
}

void NetConfig::creatDefaultConfig()
{
    QFile* pFile = new QFile("conf/"+QString(CONF_FILENAME));

    if(pFile->exists())
    {
        delete pFile;
        return;
    }
    else
    {
        delete pFile;

        //设置默认配置
        downloadPath = "download/";
        maxTaskNum = 3;
        userName = QString();
        passwd = QString();
        remPasswd = false;
        autoLogin = false;
        saveConfig();
    }
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
            //读取用户名
            if(obj.contains("userName"))
            {
                jval = obj.take("userName");
                userName = jval.toString();
                qDebug()<<"userName:"<<userName;
            }
            else
            {
                creatConfig();
            }
            //读取密码密文
            if(obj.contains("passwdAes"))
            {
                jval = obj.take("passwdAes");
                passwdAes = jval.toString();
                if(!passwdAes.isEmpty())
                {
                    QByteArray qba = QByteArray::fromHex(passwdAes.toLocal8Bit());
                    passwd = QString(netAes.AesUncrypt(qba));
                    qDebug()<<"passwd:"<<passwd;
                }
            }
            else
            {
                creatConfig();
            }
            //读取是否记住密码
            if(obj.contains("remPasswd"))
            {
                jval = obj.take("remPasswd");
                remPasswd = jval.toBool();
                qDebug()<<"remPasswd:"<<remPasswd;
            }
            else
            {
                creatConfig();
            }
            //读取是否自动登录
            if(obj.contains("autoLogin"))
            {
                jval = obj.take("autoLogin");
                autoLogin = jval.toBool();
                qDebug()<<"autoLogin:"<<autoLogin;
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

//    if(pFile->exists())
//    {
//        delete pFile;
//        return;
//    }

    //插入默认配置内容
    QJsonObject obj;
    obj.insert("downloadPath",downloadPath);
    obj.insert("maxTaskNum",maxTaskNum);
    obj.insert("userName",userName);
    if(!remPasswd)
        passwdAes = QString();
    else
    {
        QByteArray qba = passwd.toLocal8Bit();
        passwdAes = QString(netAes.AesEncrypt(qba).toHex());
    }
    obj.insert("passwdAes",passwdAes);
    obj.insert("remPasswd",remPasswd);
    obj.insert("autoLogin",autoLogin);

    QJsonDocument doc;
    doc.setObject(obj);
    QByteArray qba = doc.toJson();qDebug()<<"[NetConfig]:"<<qba;
    pFile->open(QFile::WriteOnly);
    pFile->write(qba);
    pFile->close();

    delete pFile;
}
