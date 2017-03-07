#include "nethttp.h"
#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


NetHttp::NetHttp(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    State = H_LOGIN;
    isLastPage = false;
    currentPageNum = 0;
//    QString str;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
//    netLogin("admin","888888");
}

/***登录接口***/
void NetHttp::netLogin(QString user, QString passwd)
{
    QString nUrl;
    QByteArray qba = QCryptographicHash::hash(passwd.toLocal8Bit(), QCryptographicHash::Md5);
//    qDebug()<<qba.toHex();
    QString passwdMD5 =qba.toHex();
    nUrl = QString(HTTP_ADDR) + "/login/userlogin?" +QString("password=%1&username=%2").arg(passwdMD5).arg(user);
//    nUrl = QString("http://120.24.216.97:8888/login/userlogin?password=%1&username=%2")
//            .arg(passwdMD5).arg(user);
    qDebug()<<"[login]:"<<nUrl;
    manager->get(QNetworkRequest(QUrl(nUrl)));

}

/***获取文件列表***/
void NetHttp::netList(double pId, int cPage, int pageSize, int showdelete, QString name, QString fileType)
{
    QString nUrl;
    nUrl = QString(HTTP_ADDR) + "/api/file/getMyFile?"+QString("fileType%1=&name=%2&pageSize=%3&cpage=%4&pid=%5&showDelete=%6")\
            .arg(fileType).arg(name).arg(pageSize).arg(cPage).arg(pId).arg(showdelete).toLocal8Bit();
//    pData =
    qDebug()<<"LIST"<<nUrl;
    State = H_LIST;
    manager->get(QNetworkRequest(QUrl(nUrl)));
}

void NetHttp::netMkdir(double pId, QString fileName)
{
    QString nUrl;
//    nUrl = QString(HTTP_ADDR) + "api/file/createFolder?"+QString("pid=%1&name=%2").arg(pId).arg(QString(fileName.toUtf8().toPercentEncoding()).replace('%','\\x'));
    nUrl = QString(HTTP_ADDR) + "/api/file/createFolder";
    QByteArray qba = QString("pid=%1&name=").arg(pId).toLocal8Bit()+fileName.toUtf8();
    State = H_NEW;
    QNetworkRequest request(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    manager->post(request,qba);

}

void NetHttp::netUpload(QString fileName, double pId)
{
    fTrans = new netTrans;
    fTrans->netUpload(fileName, pId);
    emit newTask(fTrans);
}

void NetHttp::netDownload(fileInfo info)
{
    fTrans = new netTrans;
    fTrans->netDownload(info);
    emit newTask(fTrans);
}

void NetHttp::netDelete(double fId)
{
    QString nUrl;
    nUrl = QString(HTTP_ADDR) + "/api/file/deleteFile?"+QString("fid=%1")\
            .arg(fId).toLocal8Bit();
//    pData =
    qDebug()<<"DELETE"<<nUrl;
    State = H_DEL;
    manager->get(QNetworkRequest(QUrl(nUrl)));
}

/***接收http返回内容槽***/
void NetHttp::replyFinished(QNetworkReply *reply)
{
    QByteArray nRecv = reply->readAll();
    qDebug()<<"http recv:"<<nRecv;
    switch(State)
    {
        case H_LOGIN:break;
        case H_DEL:
            emit updateRequest();break;
        case H_LIST:
            fileListClear();
            fileInfoRecv(nRecv);break;
        case H_NEW:
            callbackNew(nRecv);break;
        default:break;
    }
}

/************************************
  私有接口
************************************/
void NetHttp::fileInfoRecv(QByteArray info)
{
    fileInfo* fInfo;
    fInfo = new fileInfo;
    QJsonParseError jError;
    QJsonValue jval;
    QJsonDocument parseDoc = QJsonDocument::fromJson(info, &jError);


    if(jError.error == QJsonParseError::NoError)
    {
        if(parseDoc.isObject())
        {
            QJsonObject obj = parseDoc.object();

            if(obj.contains("code"))
            {
                //解析返回的状态码
                jval = obj.take("code");
                if(jval.isString() && (jval.toString() == "200"))
                {
//                    qDebug("---------------------\nList info:");
//                    qDebug()<<"code:"<<jval.toString()<<obj.take("msg").toString();
                    //解析返回列表的属性
                    if(obj.contains("result"))
                    {

                        jval = obj.take("result");
                        if(jval.isObject())
                        {
                            QJsonObject subObj = jval.toObject();
                            //判断是否是第一页
                            if(subObj.contains("firstPage"))
                            {
                                jval = subObj.take("firstPage");
                                isFirstPage = jval.toBool();
//                                qDebug()<<"Is first page:"<<isFirstPage;
                            }
                            //判断是否是最后一页
                            if(subObj.contains("lastPage"))
                            {
                                jval = subObj.take("lastPage");
                                isLastPage = jval.toBool();
//                                qDebug()<<"Is last page:"<<isLastPage;
                            }
                            //解析列表行数
                            if(subObj.contains("totalRow"))
                            {
                                jval = subObj.take("totalRow");
                                totalRow = jval.toDouble();
//                                qDebug()<<"Total row:"<<totalRow;
                            }
                            //解析列表页数
                            if(subObj.contains("totalPage"))
                            {
                                jval = subObj.take("totalPage");
                                totalPage = jval.toDouble();
//                                qDebug()<<"Total page:"<<totalPage;
                            }
                            //解析列表大小
                            if(subObj.contains("pageSize"))
                            {
                                jval = subObj.take("pageSize");
//                                int pageSize; = jval.toDouble();
//                                qDebug()<<"Page size:"<<pageSize;
                            }
                            else return;

                            //解析当前页码号
                            if(subObj.contains("pageNumber"))
                            {
                                jval = subObj.take("pageNumber");
                                currentPageNum = jval.toDouble();
//                                qDebug()<<"Current pageNum:"<<currentPageNum;
//                                qDebug("\n");
                            }
                            //解析列表文件信息
                            if(subObj.contains("list"))
                            {
                                jval = subObj.take("list");
                                if(jval.isArray())
                                {
                                    QJsonArray jArray = jval.toArray();

                                    for(int i=0; i<jArray.count(); i++)
                                    {
                                        subObj = jArray.at(i).toObject();
                                        fInfo = new fileInfo;
                                        fInfo->ADD_TIME = QDateTime::fromString(subObj.take("ADD_TIME").toString(),"yyyy-MM-dd hh:mm:ss");
                                        fInfo->EXT = subObj.take("EXT").toString();
                                        fInfo->FILE_PATH = subObj.take("FILE_PATH").toString();
                                        fInfo->FILE_TYPE = subObj.take("FILE_TYPE").toDouble();
                                        fInfo->ID = subObj.take("ID").toDouble();
                                        fInfo->IS_ENCRYPED = subObj.take("IS_ENCRYPED").toBool();
                                        fInfo->LAST_MOD_TIME = QDateTime::fromString(subObj.take("LAST_MOD_TIME").toString(),"yyyy-MM-dd hh:mm:ss");
                                        fInfo->MAST_ID = subObj.take("MAST_ID").toDouble();
                                        fInfo->MD5 = subObj.take("MD5").toString();
                                        fInfo->PARENT_ID = subObj.take("PARENT_ID").toDouble();
                                        fInfo->REAL_KEY = subObj.take("REAL_KEY").toString();
                                        fInfo->SIZE = subObj.take("SIZE").toDouble();
                                        fInfo->STATUS = subObj.take("STATUS").toDouble();
                                        fInfo->USER_ID = subObj.take("USER_ID").toDouble();
                                        fInfo->VERSION = subObj.take("VERSION").toDouble();
                                        fInfo->REAL_NAME = subObj.take("REAL_NAME").toString();
                                        fInfo->TYPE = subObj.take("TYPE").toDouble();
                                        fInfo->FILE_SERVER = subObj.take("FILE_SERVER").toString();
                                        fInfo->FILE_NAME = subObj.take("FILE_NAME").toString();
//                                        fileInfoShow(fInfo);
                                        listInfo<<fInfo;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    qDebug()<<"code:"<<jval.toString()<<obj.take("msg").toString();
                    return;
                }

            }
        }
    }
    else return;
    emit listUpdate(listInfo);
}

/***打印文件信息***/
void NetHttp::fileInfoShow(fileInfo *info)
{
    if(info == NULL)
        return;
    qDebug("---------------------");
    qDebug()<<"FILE_NAME:"<<info->FILE_NAME;
    qDebug()<<"FILE_PATH:"<<info->FILE_PATH;
    qDebug()<<"FILE_TYPE:"<<info->FILE_TYPE;
    qDebug()<<"FILE_SERVER:"<<info->FILE_SERVER;
    qDebug()<<"EXT:"<<info->EXT;
    qDebug()<<"ID:"<<info->ID;
    qDebug()<<"IS_ENCRYPED:"<<info->IS_ENCRYPED;
    qDebug()<<"ADD_TIME:"<<info->ADD_TIME.toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<<"LAST_MOD_TIME:"<<info->LAST_MOD_TIME.toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<<"MAST_ID:"<<info->MAST_ID;
    qDebug()<<"MD5:"<<info->MD5;
    qDebug()<<"PARENT_ID:"<<info->PARENT_ID;
    qDebug()<<"REAL_KEY:"<<info->REAL_KEY;
    qDebug()<<"REAL_NAME:"<<info->REAL_NAME;
    qDebug()<<"SIZE:"<<info->SIZE;
    qDebug()<<"STATUS:"<<info->STATUS;
    qDebug()<<"TYPE:"<<info->TYPE;
    qDebug()<<"USER_ID:"<<info->USER_ID;
    qDebug()<<"VERSION:"<<info->VERSION;
    qDebug("\n");

}

void NetHttp::fileListClear()
{
    while(!listInfo.isEmpty())
    {
        fileInfo* info = listInfo.takeFirst();
        delete info;
    }

    return;
}

void NetHttp::callbackNew(QByteArray info)
{
    QJsonParseError jError;
    QJsonValue jval;
    QJsonDocument parseDoc = QJsonDocument::fromJson(info, &jError);


    if(jError.error == QJsonParseError::NoError)
    {
        if(parseDoc.isObject())
        {
            QJsonObject obj = parseDoc.object();

            if(obj.contains("code"))
            {
                //解析返回的状态码
                jval = obj.take("code");
                if(jval.isString() && (jval.toString() == "200"))
                {
                    qDebug()<<"[新建目录]"<<obj.take("msg").toString();
                }
                else
                {
                    qDebug()<<"[新建目录]:"<<jval.toString()<<obj.take("msg").toString();
                    return;
                }

            }
        }
    }
    else return;
    emit updateRequest();
}

fileInfo::fileInfo()
{

}

fileInfo::fileInfo(const fileInfo &info)
{
    ID = info.ID;
    MAST_ID = info.MAST_ID;
    USER_ID = info.USER_ID;
    PARENT_ID = info.PARENT_ID;
    STATUS = info.STATUS;
    VERSION = info.VERSION;
    IS_ENCRYPED = info.IS_ENCRYPED;
    FILE_TYPE = info.FILE_TYPE;
    TYPE = info.TYPE;
    SIZE = info.SIZE;
    LAST_MOD_TIME = info.LAST_MOD_TIME;
    ADD_TIME = info.ADD_TIME;
    MD5 = info.MD5;
    EXT = info.EXT;
    FILE_SERVER = info.FILE_SERVER;
    REAL_KEY = info.REAL_KEY;
    REAL_NAME = info.REAL_NAME;
    FILE_PATH = info.FILE_PATH;
    FILE_NAME = info.FILE_NAME;
}
