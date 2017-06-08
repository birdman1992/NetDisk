#include "nethttp.h"
#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegExp>


NetHttp::NetHttp(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    managerSync = new QNetworkAccessManager(this);
    State = H_LOGIN;
    isLastPage = false;
    needLoginSync = true;
    currentPageNum = 0;
    lastSyncId = 0;
    token = QString();
    qRegisterMetaType<TaskInfo>("TaskInfo");
//    QString str;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(managerSync, SIGNAL(finished(QNetworkReply*)), this, SLOT(replySyncFinished(QNetworkReply*)));
    //    netLogin("admin","888888");
}

void NetHttp::netInit(TransList *trans)
{
    transList = trans;
}

/***登录接口***/
void NetHttp::netLogin(QString user, QString passwd)
{
    QString nUrl;
    QStringList paramList;
    State = H_LOGIN;
    paramList<<"password="+QString(passwd)+"&"<<"userName="+user+"&"<<QString(APP_ID)+"&";
    QByteArray sign = getSign(paramList);

    nUrl = netConf->getServerAddress() + "/api/user/login?" +QString("password=%1&userName=%2&sign=%3").arg(passwd).arg(user).arg(QString(sign.toHex()));
    nUrl+="&"+QString(APP_ID);//+"&"+QString(APP_KEY);
    qDebug()<<"[login]:"<<nUrl;
    manager->get(QNetworkRequest(QUrl(nUrl)));
}

/***获取文件列表***/
void NetHttp::netList(double pId, int cPage, int pageSize, int showdelete, QString name, QString fileType)
{
    QString nUrl;
    nUrl = netConf->getServerAddress() + "/api/file/getMyFile";//
    QStringList param;
    param<<QString("pid=%1&").arg(pId)<<QString("cpage=%1&").arg(cPage)<<QString("pageSize=%1&").arg(pageSize)<<QString("showDelete=%1&").arg(showdelete)<<QString(APP_ID)+"&";

    if(!name.isEmpty())
        param<<QString("name=%1&").arg(name);
    if(!fileType.isEmpty())
        param<<QString("type=%1&").arg(fileType);

    param<<QString("token=%1&").arg(token);

    QByteArray qba = getPost(param);

    qDebug()<<"LIST"<<nUrl<<qba;
    State = H_LIST;
    QNetworkRequest request(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    manager->post(request,qba);
}

void NetHttp::netMkdir(double pId, QString fileName)
{
    QString nUrl;
    QStringList params;
//    nUrl = netConf->getServerAddress() + "api/file/createFolder?"+QString("pid=%1&name=%2").arg(pId).arg(QString(fileName.toUtf8().toPercentEncoding()).replace('%','\\x'));
    nUrl = netConf->getServerAddress() + "/api/file/createFolder";
    params<<QString("pid=%1&").arg(pId)<<QString("name=%1&").arg(fileName)<<QString(APP_ID)+"&";
//    QByteArray qba = QString("pid=%1&name=").arg(pId).toLocal8Bit()+fileName.toUtf8();
    params<<QString("token=%1&").arg(token);
    QByteArray qba = getPost(params);
    State = H_NEW;
    QNetworkRequest request(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    manager->post(request,qba);
}

void NetHttp::netUpload(QString fileName, double pId)
{
    fTrans = new netTrans;
    fTrans->netUpload(fileName, pId, token);
    fTrans->taskStart();
    emit newUPloadTask(fTrans,false);
    //    emit newTask(fTrans);
}

void NetHttp::netDownload(fileInfo info, QString downloadPath)
{
    fTrans = new netTrans;
    fTrans->netDownload(info, downloadPath,token);
    fTrans->taskStart();
    //    emit newTask(fTrans);
}

void NetHttp::netDownload(DownloadTaskInfo *info)
{
    fTrans = new netTrans;
    fTrans->netDownload(*(info->info), info->path, token);
//    fTrans->taskStart();
    fTrans->task = info;
    emit newDownloadTask(fTrans,false);
}

void NetHttp::netDelete(double fId)
{
    QString nUrl;
    QStringList params;

    params<<QString("fid=%1&").arg(fId)<<QString("token=%1&").arg(token)<<QString(APP_ID)+"&";
    QByteArray sign = getSign(params);
    nUrl = netConf->getServerAddress() + QString("/api/file/deleteFile?fid=%1&token=%2&sign=%3&").arg(fId).arg(token).arg(QString(sign.toHex()))+APP_ID+"&";
    qDebug()<<"DELETE"<<nUrl;
    State = H_DEL;
    manager->get(QNetworkRequest(QUrl(nUrl)));
}

void NetHttp::netDelete(QStringList fIds)
{
    QString nUrl;
    QString fidList = fIds.takeFirst();
    QStringList params;
    while(!fIds.isEmpty())
    {
        fidList += ",";
        fidList += fIds.takeFirst();
    }
    params<<QString("fids=%1&").arg(fidList)<<QString("token=%1&").arg(token)<<QString(APP_ID)+"&";
    QByteArray sign = getSign(params);
    nUrl = netConf->getServerAddress() + QString("/api/file/deleteFiles?fids=%1&token=%2&sign=%3&").arg(fidList).arg(token).arg(QString(sign.toHex()))+APP_ID+"&";
    qDebug()<<"DELETE"<<nUrl;
    State = H_DEL;
    manager->get(QNetworkRequest(QUrl(nUrl)));
}

void NetHttp::netCreatShareLinks(QStringList fids)
{
    QString nUrl;
    QString fidList = fids.takeFirst();
    QStringList params;

    while(!fids.isEmpty())
    {
        fidList += ",";
        fidList += fids.takeFirst();
    }
    params<<QString("fids=%1&").arg(fidList)<<QString(APP_ID)+"&"<<QString("token=%1&").arg(token);
    QByteArray sign = getSign(params);
    nUrl = netConf->getServerAddress() + QString("/api/file/createShareLink?fids=%1&token=%2&sign=%3&").arg(fidList).arg(token).arg(QString(sign.toHex()))+APP_ID+"&";
    qDebug()<<"[SHARE]"<<nUrl;
    State = H_SHARE;
    manager->get(QNetworkRequest(QUrl(nUrl)));
}

void NetHttp::netFilesRestore(QStringList fids)
{
    QString nUrl;
    QString fidList = fids.takeFirst();
    QStringList params;

    while(!fids.isEmpty())
    {
        fidList += ",";
        fidList += fids.takeFirst();
    }
    params<<QString("fids=%1&").arg(fidList)<<QString(APP_ID)+"&"<<QString("token=%1&").arg(token);
    QByteArray sign = getSign(params);
    nUrl = netConf->getServerAddress() + QString("/api/file/recoverFiles?fids=%1&token=%2&sign=%3&").arg(fidList).arg(token).arg(QString(sign.toHex()))+APP_ID+"&";
    qDebug()<<"[restore]"<<nUrl;
    State = H_DEL;
    manager->get(QNetworkRequest(QUrl(nUrl)));
}

void NetHttp::netFilesDownload(QList<fileInfo *> dirs, QList<fileInfo *> files)
{
    DownloadTaskInfo* info;

    //填充下载队列
    while(!files.isEmpty())
    {
        info = new DownloadTaskInfo;
        info->info = files.takeFirst();
        info->path = QString();
        if(!taskCheck(info->info))
        {
            delete info;
            continue;
        }
        listDownloading<<info->info->ID;
        listDownloadTask<<info;
    }

    //填充遍历队列
    QDir dir;
    while(!dirs.isEmpty())
    {
        info = new DownloadTaskInfo;
        info->info = dirs.takeFirst();
        info->path = netConf->getDownloadPath()+info->info->FILE_NAME+"/";

        if(!taskCheck(info->info))
        {
            delete info;
            continue;
        }

        if(!dir.exists(info->path))
            dir.mkdir(info->path);
        listDownloading<<info->info->ID;
        listDownloadCheck<<info;
    }
    netDownloadDirsCheck(listDownloadCheck);
}

void NetHttp::getUserOrgList()
{
    QString nUrl;
    QStringList params;
    params<<QString(APP_ID)+"&"<<QString("token=%1&").arg(netConf->token);
    QByteArray sign = getSign(params);

    nUrl = netConf->getServerAddress() + QString("/org/userOrgList?sign=%1&token=%2&").arg(QString(sign.toHex())).arg(netConf->token)+APP_ID;//
    qDebug()<<"[getUserOrgList]"<<nUrl;
    reply_userDeptInfo = netConf->manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_userDeptInfo, SIGNAL(finished()), this, SLOT(replyUserDeptInfoFinished()));
}

void NetHttp::netSync(double pId, QDateTime lastSyncTime)
{
    QString lastTime;
    if(lastSyncTime.isNull())
        lastTime = QString();
    else
        lastTime = lastSyncTime.toString("yyyy-MM-dd hh:mm:ss");

    QString nUrl;
    nUrl = netConf->getServerAddress() + "/api/file/sync";//
    QStringList param;
    param<<QString("pid=%1&").arg(pId)<<QString(APP_ID)+"&";
    if(!lastTime.isNull())
        param<<QString("syncTime=%1&").arg(lastTime);

    param<<QString("token=%1&").arg(token);
    QByteArray qba = getPost(param);
    lastSyncId = pId;

    qDebug()<<"SYNC"<<nUrl<<qba;
    QNetworkRequest request(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    managerSync->post(request,qba);
}

void NetHttp::syncTraversal()
{
    netSync(100);
}

void NetHttp::getUserInfo()
{
    QString nUrl;
    nUrl = netConf->getServerAddress() + "/api/user/showUserInfo";//
    QStringList param;
    param<<QString(APP_ID)+"&";
    param<<QString("token=%1&").arg(token);

    QByteArray qba = getPost(param);

    qDebug()<<"[getUserInfo]"<<nUrl<<qba;
    QNetworkRequest request(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    reply_userinfo = netConf->manager->post(request,qba);
    connect(reply_userinfo, SIGNAL(finished()), this, SLOT(replyUserInfoFinished()));
}

QString NetHttp::netToken()
{
    return token;
}

bool NetHttp::taskCheck(fileInfo *info)
{
    for(int i=0; i<listDownloading.count(); i++)
    {
        if(info->ID == listDownloading.at(i))
            return false;
    }

    for(int i=0; i<listDownloadTask.count(); i++)
    {
        if(info->ID == listDownloadTask.at(i)->info->ID)
            return false;
    }
    qDebug("pass");
    return true;
}

QString NetHttp::httpDateTran(QByteArray raw)
{
    QString str = QString(raw);
    QString pattern("*, * * * *:*:*");
    QRegExp rx(pattern);
    rx.setPatternSyntax(QRegExp::Wildcard);
    if(!rx.exactMatch(str))
        return QString();

    int i,j,k;
    QStringList l;
    l<<"Jan"<<"Feb"<<"Mar"<<"Apr"<<"May"<<"Jun"<<"Jul"<<"Aug"<<"Sep"<<"Oct"<<"Nov"<<"Dec"<<"";
    str = str.right(str.length() - str.indexOf(' ') - 1);
    i = str.indexOf(" ");
    j = str.indexOf(" ",i+1);
    QString month = str.mid(i+1,j-i-1);

    k = 0;
    while((l.at(k++) != month) && (k<l.count()));
    str.replace(i+1, j-i-1, QString::number(k));
    str.remove(str.length()-4,4);
    return str;
}

/***接收http返回内容槽***/
void NetHttp::replyFinished(QNetworkReply *reply)
{
    QByteArray nRecv = reply->readAll();
//    QString str = httpDateTran(reply->rawHeader("Date"));
//    serverTime = QDateTime::fromString(str,"dd M yyyy hh:mm:ss");
    qDebug()<<"http recv:"<<nRecv.size();
    qDebug()<<nRecv;

    switch(State)
    {
        case H_LOGIN:
            loginRst(nRecv);break;
        case H_DEL:
            emit updateRequest();break;
        case H_LIST:
            fileListClear();
            fileInfoRecv(nRecv);break;
        case H_NEW:
            callbackNew(nRecv);break;
        case H_SHARE:
            shareLinkRecv(nRecv);break;
        default:break;
    }
}

void NetHttp::replySyncFinished(QNetworkReply *reply)
{
    QByteArray nRecv = reply->readAll();
    if(reply->hasRawHeader("SERVER_TIME"))
    {
        QString str = QString(reply->rawHeader("SERVER_TIME"));
//        qDebug()<<"[raw date]"<<str;
        serverTime = QDateTime::fromString(str,"yyyy-MM-dd hh:mm:ss");
    }
    else
    {
        qDebug()<<"Raw header SERVER_TIME not found.";
    }
    qDebug()<<"http SYNC recv:"<<nRecv.size();
    qDebug()<<nRecv;
    syncInfoRecv(nRecv, serverTime);
}

void NetHttp::replyUserInfoFinished()
{
    UserInfo info;
    QJsonParseError jError;
    QJsonValue jval;
    QByteArray qba = reply_userinfo->readAll();
    qDebug()<<"[userInfoRecv]"<<qba;
    QJsonDocument parseDoc = QJsonDocument::fromJson(qba, &jError);

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
                    //解析返回列表的属性
                    if(obj.contains("result"))
                    {

                        jval = obj.take("result");
                        if(jval.isObject())
                        {
                            QJsonObject subObj = jval.toObject();
                            if(subObj.contains("PHONE"))
                            {
                                jval = subObj.take("PHONE");
                                info.PHONE = jval.toString();
                            }
                            if(subObj.contains("ADMIN"))
                            {
                                jval = subObj.take("ADMIN");
                                info.ADMIN = jval.toString();
                            }
                            if(subObj.contains("CURRENT_SIZE"))
                            {
                                jval = subObj.take("CURRENT_SIZE");
                                info.CURRENT_SIZE = jval.toDouble();
                            }
                            if(subObj.contains("LANGUAGE"))
                            {
                                jval = subObj.take("LANGUAGE");
                                info.LANGUAGE = jval.toString();
                            }
                            if(subObj.contains("REAL_NAME"))
                            {
                                jval = subObj.take("REAL_NAME");
                                info.REAL_NAME = jval.toString();
                            }
                            if(subObj.contains("MAX_SIZE"))
                            {
                                jval = subObj.take("MAX_SIZE");
                                info.MAX_SIZE = jval.toDouble();
                            }
                            if(subObj.contains("ROLE"))
                            {
                                jval = subObj.take("ROLE");
                                info.ROLE = jval.toDouble();
                            }
                            if(subObj.contains("USER_NAME"))
                            {
                                jval = subObj.take("USER_NAME");
                                info.USER_NAME = jval.toString();
                            }
                            if(subObj.contains("DEPTS"))
                            {
                                jval = subObj.take("DEPTS");
                                info.DEPTS = jval.toString();
                            }
                            if(subObj.contains("ID"))
                            {
                                jval = subObj.take("ID");
                                info.ID = jval.toDouble();
                            }
                            if(subObj.contains("EMAIL"))
                            {
                                jval = subObj.take("EMAIL");
                                info.EMAIL = jval.toString();
                            }
                            if(subObj.contains("ADD_TIME"))
                            {
                                jval = subObj.take("ADD_TIME");
                                info.ADD_TIME = QDateTime::fromString(jval.toString(), "yyyy-MM-dd hh:mm:ss");
                            }
                            if(subObj.contains("STATUS"))
                            {
                                jval = subObj.take("STATUS");
                                info.STATUS = jval.toDouble();
                            }
                            if(subObj.contains("PASSWORD"))
                            {
                                jval = subObj.take("PASSWORD");
                                info.PASSWORD = jval.toString();
                            }
                            if(subObj.contains("SKIN"))
                            {
                                jval = subObj.take("SKIN");
                                info.SKIN = jval.toString();
                            }
                            if(subObj.contains("COMNAME"))
                            {
                                jval = subObj.take("COMNAME");
                                info.COMNAME = jval.toString();
                            }
                            emit newUserInfo(info);
                        }
                    }
                }
                qDebug()<<"[replyUserInfoFinished]"<<obj.take("msg").toString();
            }
        }
    }
}

void NetHttp::replyTaskFinished()
{
    fileInfo* fInfo;
    DownloadTaskInfo* dInfo;
    QJsonParseError jError;
    QJsonValue jval;
    QByteArray info = reply_task->readAll();
    disconnect(reply_task, SIGNAL(finished()), this, SLOT(replyTaskFinished()));
    reply_task->deleteLater();
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
                                        dInfo = new DownloadTaskInfo;
                                        dInfo->info = fInfo;
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
                                        if(dInfo->info->EXT.isEmpty())//文件夹
                                        {
                                            dInfo->path = curTaskPath+dInfo->info->FILE_NAME+"/";
                                            listDownloadCheck<<dInfo;
                                            QDir dir;
                                            if(!dir.exists(dInfo->path))
                                            {
                                                dir.mkdir(dInfo->path);
                                            }
                                        }
                                        else//文件
                                        {
                                            dInfo->path = curTaskPath;
                                            listDownloadTask<<dInfo;
                                        }
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

    netDownloadDirsCheck(listDownloadCheck);
//    if(needLoginSync)
//    {
//        emit needSync();
//        needLoginSync = false;
//    }
//    emit listUpdate(listInfo);
    //    emit pageChanged(isFirstPage, isLastPage, currentPageNum, totalPage);
}

void NetHttp::replyUserDeptInfoFinished()
{
    QByteArray info = reply_userDeptInfo->readAll();
    reply_userDeptInfo->deleteLater();
    qDebug()<<"replyUserDeptInfoFinished"<<info;
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
                if(obj.contains("msg"))
                {
                    jval = obj.take("msg");
                    qDebug()<<"[SYNC MSG]:"<<jval.toString();
                }
                jval = obj.take("code");
                if(!(jval.isString() && (jval.toString() == "200")))
                {
                    return;
                }
            }
            if(obj.contains("result"))
            {
                jval = obj.take("result");
                QJsonArray infoArray = jval.toArray();
                if(infoArray.isEmpty())
                {
                   return;
                }
                QList<User*> l_user;
                QList<Dept*> l_dept;
                for(int i=0; i<infoArray.count(); i++)
                {
                    jval = infoArray[i];
                    QJsonObject subObj = jval.toObject();
                    if(subObj.take("lev").toInt() == 1)
                    {
                        Dept* dept = new Dept;
                        dept->id = subObj.take("id").toString();
                        dept->name = subObj.take("text").toString();
                        dept->parentId = subObj.take("parent").toString();
                        l_dept<<dept;
                    }
                    else if(subObj.take("lev").toInt() == 2)
                    {
                        User* user = new User;
                        user->id = subObj.take("id").toString();
                        user->name = subObj.take("text").toString();
                        user->parentId = subObj.take("parent").toString();
                        l_user<<user;
                    }
                }
                creatOrz(l_dept,l_user);
            }
        }
    }
}

/************************************
  私有接口
************************************/
void NetHttp::shareLinkRecv(QByteArray info)
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
                    if(obj.contains("result"))
                    {
                        jval = obj.take("result");
                        QJsonObject subObj = jval.toObject();
                        emit shareLink(subObj.take("link").toString(), subObj.take("password").toString());
                    }
                }
                else
                {
                    qDebug()<<obj.take("msg").toString();
                }
            }
        }
    }
}

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
    if(needLoginSync)
    {
        emit needSync();
        needLoginSync = false;
    }
    emit listUpdate(listInfo);
    emit pageChanged(isFirstPage, isLastPage, currentPageNum, totalPage);
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

void NetHttp::syncListClear()
{
    while(!listSync.isEmpty())
    {
        syncInfo* info = listSync.takeFirst();
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

void NetHttp::loginRst(QByteArray rst)
{
    QJsonParseError jError;
    QJsonValue jval;
    QJsonDocument parseDoc = QJsonDocument::fromJson(rst, &jError);


    if(jError.error == QJsonParseError::NoError)
    {
        if(parseDoc.isObject())
        {
            QJsonObject obj = parseDoc.object();

            if(obj.contains("msg"))
            {
                jval = obj.take("msg");
                qDebug()<<"[LOGIN MSG]"<<jval.toString();
            }
            if(obj.contains("result"))
            {
                jval = obj.take("result");
                token = jval.toString();
                netConf->token = token;
                qDebug()<<"[LOGIN TOKEN]"<<jval.toString();
                getUserInfo();
            }
            if(obj.contains("code"))
            {
                //解析返回的状态码
                jval = obj.take("code");
                if(jval.isString() && (jval.toString() == "200"))
                {
                    emit loginStateChanged(true);
                }
                else
                    emit loginStateChanged(false);
            }
        }
    }
}

void NetHttp::syncInfoRecv(QByteArray info, QDateTime syncTime)
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
                if(obj.contains("msg"))
                {
                    jval = obj.take("msg");
                    qDebug()<<"[SYNC MSG]:"<<jval.toString();
                }
                jval = obj.take("code");
                if(!(jval.isString() && (jval.toString() == "200")))
                {
                    return;
                }
            }
            if(obj.contains("result"))
            {
                jval = obj.take("result");
                QJsonArray syncArray = jval.toArray();
                if(syncArray.isEmpty() && (lastSyncId == -1))
                {qDebug("SYNC over!!");
                    emit syncHostPoint(syncTime);
                    return;
                }
                syncListCreat(syncArray, syncTime);
            }
        }
    }
}

void NetHttp::syncListCreat(QJsonArray info, QDateTime syncTime)
{
    int i = 0;
    QJsonValue jval;
    syncListClear();
    while(i<info.count())
    {
        QJsonValue value = info.at(i++);
        QJsonObject obj = value.toObject();
        syncInfo* sInfo = new syncInfo;

        if(obj.contains("STATUS"))
        {
            jval = obj.take("STATUS");
            sInfo->STATUS = jval.toInt();
            qDebug()<<sInfo->STATUS;
//            if(sInfo->STATUS != 0)
//            {
//                delete sInfo;
//                continue;
//            }
        }
        else
            goto syncListCreatError;
        if(obj.contains("LAST_MOD_TIME"))
        {
            jval = obj.take("LAST_MOD_TIME");
            sInfo->LAST_MOD_TIME = QDateTime::fromString(jval.toString(), "yyyy-MM-dd hh:mm:ss");
        }
        else
            goto syncListCreatError;
        if(obj.contains("USER_ID"))
        {
            jval = obj.take("USER_ID");
            sInfo->USER_ID = jval.toInt();
        }
        else
            goto syncListCreatError;
        if(obj.contains("FILE_SERVER"))
        {
            jval = obj.take("FILE_SERVER");
            sInfo->FILE_SERVER = jval.toInt();
        }
        else
            goto syncListCreatError;
        if(obj.contains("MD5"))
        {
            jval = obj.take("MD5");
            sInfo->MD5 = jval.toString();
        }
        else
            goto syncListCreatError;
        if(obj.contains("ADD_TIME"))
        {
            jval = obj.take("ADD_TIME");
            sInfo->ADD_TIME = QDateTime::fromString(jval.toString(), "yyyy-MM-dd hh:mm:ss");;
        }
        else
            goto syncListCreatError;
        if(obj.contains("EXT"))
        {
            jval = obj.take("EXT");
            sInfo->EXT = jval.toString();
        }
        else
            goto syncListCreatError;
        if(obj.contains("FILE_NAME"))
        {
            jval = obj.take("FILE_NAME");
            sInfo->FILE_NAME = jval.toString();
        }
        else
            goto syncListCreatError;
        if(obj.contains("VERSION"))
        {
            jval = obj.take("VERSION");
            sInfo->VERSION = jval.toInt();
        }
        else
            goto syncListCreatError;
        if(obj.contains("REAL_KEY"))
        {
            jval = obj.take("REAL_KEY");
            sInfo->REAL_KEY = jval.toString();
        }
        else
            goto syncListCreatError;
        if(obj.contains("FILE_TYPE"))
        {
            jval = obj.take("FILE_TYPE");
            sInfo->FILE_TYPE = jval.toInt();
        }
        else
            goto syncListCreatError;
        if(obj.contains("FILE_CODE"))
        {
            jval = obj.take("FILE_CODE");
            sInfo->FILE_CODE = jval.toString();
        }
        else
            goto syncListCreatError;
        if(obj.contains("ID"))
        {
            jval = obj.take("ID");
            sInfo->ID = jval.toDouble();
        }
        else
            goto syncListCreatError;
        if(obj.contains("MASTER_ID"))
        {
            jval = obj.take("MASTER_ID");
            sInfo->MASTER_ID = jval.toDouble();
        }
        else
            goto syncListCreatError;
        if(obj.contains("IS_ENCRYPED"))
        {
            jval = obj.take("IS_ENCRYPED");
            sInfo->IS_ENCRYPED = jval.toInt();
        }
        else
            goto syncListCreatError;
        if(obj.contains("SIZE"))
        {
            jval = obj.take("SIZE");
            sInfo->SIZE = jval.toDouble();
        }
        else
            goto syncListCreatError;
        if(obj.contains("PARENT_ID"))
        {
            jval = obj.take("PARENT_ID");
            sInfo->PARENT_ID = jval.toDouble();
        }
        else
            goto syncListCreatError;
        if(obj.contains("FILE_PATH"))
        {
            jval = obj.take("FILE_PATH");
            sInfo->FILE_PATH = jval.toString();
        }
        else
            goto syncListCreatError;
        if(obj.contains("TYPE"))
        {
            jval = obj.take("TYPE");
            sInfo->TYPE = jval.toInt();
        }
        else
            goto syncListCreatError;

        listSync<<sInfo;
    }

    emit syncUpdate(listSync, syncTime);
    return;
    syncListCreatError:
    qDebug("[SYNC INFO]:read error.");
    return;
}

void NetHttp::netDownloadDirsCheck(QList<DownloadTaskInfo *> &listCheck)
{
    QString nUrl;
    if(listCheck.isEmpty())
    {
        qDebug()<<"download list";
        for(int i=0; i<listDownloadTask.count(); i++)
        {
            netDownload(listDownloadTask.at(i));
            qDebug()<<listDownloadTask.at(i)->path+listDownloadTask.at(i)->info->FILE_NAME;
        }
        listDownloadingTask<<listDownloadTask;
        listDownloadTask.clear();
//        qDebug()<<listDownloadTask.count()<<listDownloadingTask.at(0)->info->ID<<listDownloadingTask.count();
        return;
    }
    DownloadTaskInfo* info = listCheck.takeFirst();
    qDebug()<<"netDownloadDirsCheck"<<info->info->FILE_NAME<<info->info->ID;
    double pId = info->info->ID;
    curTaskPath = info->path;
    delete info;

    nUrl = netConf->getServerAddress() + "/api/file/getMyFile";//
    QStringList param;
    param<<QString("pid=%1&").arg(pId)<<QString(APP_ID)+"&";
    param<<QString("token=%1&").arg(token);

    QByteArray qba = getPost(param);
    qDebug()<<"LIST"<<nUrl<<qba;
//    State = H_LIST;
    QNetworkRequest request(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    reply_task = netConf->manager->post(request,qba);
    connect(reply_task, SIGNAL(finished()), this, SLOT(replyTaskFinished()));
}

QByteArray NetHttp::getSign(QStringList param)
{
    qSort(param.begin(), param.end());
    QString str = QString();
    for(int i=0; i<param.count(); i++)
    {
        str += param.at(i);
    }

    str += QString(APP_KEY);
    qDebug()<<str;

    QByteArray qba = QCryptographicHash::hash(str.toLocal8Bit(), QCryptographicHash::Md5);
    qDebug()<<qba.toHex();
    return qba;
}

QByteArray NetHttp::getPost(QStringList param)
{
    QByteArray postData = QByteArray();
    qSort(param.begin(), param.end());
    QString str = QString();
    for(int i=0; i<param.count(); i++)
    {
        str += param.at(i);
    }
//    str += QString("token=%1&").arg(token);
    postData += str.toUtf8();
    str += QString(APP_KEY);
//    qDebug()<<"[sign params]"<<str;
    QByteArray sign = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5);
    qDebug()<<sign.toHex();
    postData += QString("sign=%1").arg(QString(sign.toHex())).toLocal8Bit();
//    qDebug()<<"[post params]"<<postData;
    return postData;
}

void NetHttp::creatOrz(QList<Dept *> depts, QList<User *> users)
{
    //寻找最上层部门
    for(int i=0; i<depts.count(); i++)
    {
        if(depts.at(i)->parentId == "#")
        {
            orzStruct = *(depts.at(i));
            break;
        }
        if(i == depts.count()-1)
            return;
    }

    qDebug()<<orzStruct.name<<orzStruct.parentId;

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


/***********************************************
syncInfo
**************************************************/
syncInfo::syncInfo()
{

}

syncInfo::syncInfo(syncInfo *info)
{
    ADD_TIME = info->ADD_TIME;
    EXT = info->EXT;
    FILE_CODE = info->FILE_CODE;
    FILE_NAME = info->FILE_NAME;
    FILE_PATH = info->FILE_PATH;
    FILE_SERVER = info->FILE_SERVER;
    FILE_TYPE = info->FILE_TYPE;
    ID = info->ID;
    IS_ENCRYPED = info->IS_ENCRYPED;
    LAST_MOD_TIME = info->LAST_MOD_TIME;
    LOCAL_PATH = info->LOCAL_PATH;
    MASTER_ID = info->MASTER_ID;
    MD5 = info->MD5;
    PARENT_ID = info->PARENT_ID;
    REAL_KEY = info->REAL_KEY;
    SIZE = info->SIZE;
    STATUS = info->STATUS;
    TYPE = info->TYPE;
    USER_ID = info->USER_ID;
    VERSION = info->VERSION;
}


/***************************
syncTable
*****************************/
syncTable::syncTable()
{
    cur_path = netConf->getSyncPath();
    isSyncing = false;
    syncInfoNeedUpdate = false;
    syncLocalInfo* info = new syncLocalInfo;
    info->fileId = SYNC_ID;
    info->syncPath = netConf->getSyncPath();
    info->syncPath = info->syncPath.left(info->syncPath.length()-1);
    info->fileName = info->syncPath.section('/',-1);
    info->fileSize = 0;
    info->fileMd5 = QString();
    info->isDir = 1;
    info->parentId = 0;
    info->lastDate = QFileInfo(netConf->getSyncPath()).lastModified();
    list_local<<info;
}

void syncTable::setLocalList()
{
    int i=0;
    list_local_index.clear();

    for(i=0; i<list_local.count(); i++)
    {
        list_local_index<<QString::number(list_local.at(i)->fileId);
    }
}

void syncTable::setHttpClient(NetHttp *client)
{
    syncClient = client;
}

void syncTable::syncHostToLocal()
{
    syncDir();
}

void syncTable::syncLocalToHost()
{
    syncMkDir();
//    creatSyncUploadList();
}

void syncTable::syncInfoInsert(QList<syncInfo *> info)
{
    int i;
//    int dir_pos = list_dir.count();

//    recvListClear();//清除临时链表
    //创建数据链表
    for(i=0; i<info.count(); i++)
    {
        qDebug()<<info.at(i)->ID;
        if(info.at(i)->STATUS)
        {
            qDebug()<<"[delete sync]"<<info.at(i)->ID;
            localDelete(info.at(i)->ID);
            continue;
        }
        if(list_index.indexOf(QString::number(info.at(i)->ID)) != -1)
        {qDebug("repeat");
            continue;
        }
        syncInfo* nInfo = new syncInfo(info.at(i));qDebug()<<"[SYNC from host]"<<nInfo->FILE_NAME<<"id:"<<nInfo->ID;
        list_temp<<nInfo;
        if(nInfo->TYPE == 0)
        {
            list_dir<<nInfo;
            list_task<<nInfo;qDebug()<<"list_task"<<list_task.count();
        }
        else
            list_file<<nInfo;
    }
    tempListToHostList();
    qDebug()<<"list_file"<<list_file.count();

    syncNextDir();
}

syncInfo *syncTable::getHostInfoById(double Id)
{
    QString strId = QString::number(Id);
    int ret = list_index.indexOf(strId);
    if(ret == -1)
    {
        qDebug()<<"[getHostInfoById]:"<<Id<<"not found.";
        return NULL;
    }
    qDebug()<<"id:"<<strId<<"name:"<<list_all.at(ret)->FILE_NAME;
    return list_all.at(ret);
}

int syncTable::getLocalInfoIndexByName(QString filename)
{
    double fId = getIdByName(filename);
    qDebug()<<"find id"<<fId<<filename;
    if(fId == 0)
        return -1;

    int ret = list_local_index.indexOf(QString::number(fId));
    if(ret == -1)
    {
        qDebug()<<"[getLocalInfoByName]:"<<fId<<filename<<"not found.";
        return -1;
    }
    return ret;
}

bool syncTable::fileIsDownloading(QString name)
{
    int i = 0;
    QString info;

    for(i=0; i< list_download_task.count(); i++)
    {
        info = list_download_task.at(i);

        if(info == name)
        {
            return true;
        }
    }
    return false;
}

double syncTable::getIdByName(QString name, bool* isChanged)
{
    syncLocalInfo* info = NULL;
    int i = 0;
    double ret = 0;
//    qDebug()<<"[getIdByName]"<<name;

    for(i=0; i<list_local.count(); i++)
    {
        info = list_local.at(i);
//        qDebug()<<info->syncPath;
        if(info->syncPath == name)
        {
            ret = info->fileId;
            break;
        }
    }
//    qDebug()<<ret;
    if(isChanged == NULL)
        return ret;

    if(!ret)
        *isChanged = true;
    else
    {
        QFileInfo fInfo = QFileInfo(name);
//        qDebug()<<"[date]"<<fInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss")<<info->lastDate.toString("yyyy-MM-dd hh:mm:ss");
//        qDebug()<<info->syncPath;
        if((fInfo.lastModified().toMSecsSinceEpoch()/1000) > (info->lastDate.toMSecsSinceEpoch()/1000)+5)
        {
            *isChanged = true;
            qDebug()<<fInfo.absoluteFilePath()<<"changed:";
            qDebug()<<"[date]"<<"local"<<fInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss")<<"host"<<info->lastDate.toString("yyyy-MM-dd hh:mm:ss");
        }
        else
        {
            *isChanged = false;
//            qDebug()<<fInfo.absoluteFilePath()<<"not changed";
        }
    }
    return ret;
}

QString syncTable::getPathById(double Id)
{
    int index = list_local_index.indexOf(QString::number(Id));
//    qDebug()<<"[getPathById] index"<<index<<">>"<<list_local.count()<<list_local_index.count();
    if(index == -1)
        return QString();

    return list_local.at(index)->syncPath;
}

QList<syncInfo *> syncTable::getHostList()
{
    return list_all;
}

void syncTable::setSyncAll(bool syncAllDir)
{
    syncAll = syncAllDir;
}

void syncTable::setCurPath(double)
{

}

QString syncTable::getDirPath(double Id)
{
    int ret = 0;
    double id = Id;
    syncInfo* info = NULL;
    QString path = QString();

    while(1)
    {
        ret = list_index.indexOf(QString::number(id));
        qDebug()<<"ret"<<ret<<"id"<<id;
        if(ret == -1)
            return QString();
        info = list_all.at(ret);
        path = info->FILE_NAME + "/" + path;
        qDebug()<<"PATH>>>>>>>>"<<path;
        id = info->PARENT_ID;
        if(id == SYNC_ID)
            break;
    }
    path = path.left(path.length()-1);
    return path;
}

void syncTable::syncDir()
{
    syncInfo* info;
    int i=0;
    int ret = 0;
    qDebug("[syncDir]");
    isSyncing = true;
    for(i=0; i<list_dir.count(); i++)
    {
        info = list_dir.at(i);
        ret = list_local_index.indexOf(QString::number(info->ID));
        if(ret == -1)
        {
            QDir dir;
            QString dirPath = getDirPath(info->ID);qDebug()<<"[h->l:dir]"<<dirPath<<"\n";
            if(dir.exists(cur_path+dirPath))
            {
                continue;
            }
            else
            {
                dir.mkdir(cur_path+dirPath);
                syncLocalInfo* lInfo = new syncLocalInfo;
                lInfo->syncPath = cur_path+dirPath;
                lInfo->fileId = info->ID;
                lInfo->fileMd5 = info->MD5;
                lInfo->fileName = info->FILE_NAME;
                lInfo->fileSize = info->SIZE;
                lInfo->isDir = 1;
                lInfo->parentId = info->PARENT_ID;
                lInfo->lastDate = QFileInfo(cur_path+dirPath).lastModified();
                addSyncLocalInfo(lInfo);
//                list_local<<lInfo;
                emit localListChanged();
            }
        }
    }

    isSyncing = false;
    setLocalList();
    creatSyncDownloadList();
}

void syncTable::syncFile()
{
    syncInfo* info;
    int i=0;
    int ret = 0;
    fileInfo downloadInfo;

    qDebug("[syncFile]");
    for(i=0; i<list_file.count(); i++)
    {
        info = list_file.takeAt(i);
        ret = list_local_index.indexOf(QString::number(info->ID));

        if(ret == -1)
        {
            downloadInfo.FILE_NAME = info->FILE_NAME;
            downloadInfo.SIZE = info->SIZE;
            downloadInfo.MD5 = info->MD5;
            downloadInfo.ID = info->ID;
//            syncClient->netDownload(downloadInfo,cur_path);
            return;
        }
    }
}

void syncTable::syncMkDir()
{
    qDebug()<<"[syncMkDir]";
    //创建同步上传链表
    QFileInfo* localInfoReal;
    double parentId;
    double retId;
    QString dirName;


    int i=0;
    bool isUpdated;

    for(i=0; i<list_local_real.count(); i++)
    {

        localInfoReal = list_local_real.at(i);
//        if(fileIsDownloading(localInfoReal->absoluteFilePath()))
//            continue;
        if(!localInfoReal->isDir())
        {
            continue;
        }
        parentId = getIdByName(localInfoReal->absolutePath());
        retId = getIdByName(localInfoReal->absoluteFilePath(),&isUpdated);
        dirName = localInfoReal->fileName();//此处使用FILE_NAME保存待上传的本地文件路径
//        qDebug()<<"isupdated"<<isUpdated;
//        if(isUpdated)
//            updateFileDate(retId);
        if(retId)
            continue;
        qDebug()<<"[mkdirInHost]"<<parentId<<dirName;
        mkdirInHost(parentId, dirName);
        return;
    }
    //文件夹同步完成，创建上传列表
    creatSyncUploadList();
}

void syncTable::syncDelete(QFileInfo file)
{
    QString nUrl;
    QStringList params;
    delFile = file;
    double fId = getIdByName(delFile.absoluteFilePath());

    params<<QString("fid=%1&").arg(fId)<<QString("token=%1&").arg(netConf->token)<<QString(APP_ID)+"&";
    QByteArray sign = getSign(params);
    nUrl = netConf->getServerAddress() + QString("/api/file/deleteFile?fid=%1&token=%2&sign=%3&").arg(fId).arg(netConf->token).arg(QString(sign.toHex()))+APP_ID+"&";
    qDebug()<<"DELETE"<<nUrl;
    syncInfoNeedUpdate = true;
    deleteReply = netConf->manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(deleteReply, SIGNAL(finished()), this, SLOT(recvDeleteRst()));
}

void syncTable::localDelete(QFileInfo deleteFileInfo)
{
    bool isdeleted = false;
    int index = 0;
    if(deleteFileInfo.isFile())
    {
        QFile file(deleteFileInfo.absoluteFilePath());
        if(file.exists())
            isdeleted = file.remove();
        if(isdeleted)
        {
            index = getLocalInfoIndexByName(deleteFileInfo.absoluteFilePath());
            if(index>=0)
            {qDebug()<<"remove"<<index;
                list_local.removeAt(index);
                list_local_index.removeAt(index);
                emit localListChanged();
            }
        }
    }
    else
    {
        int i = list_local_real.count()-1;
        QString deleteDir = deleteFileInfo.absoluteFilePath();
        QString str;

        for(; i>=0; i--)
        {
            str = list_local_real.at(i)->absoluteFilePath();
            qDebug()<<"delete ready"<<deleteDir;
            if(str.size() < deleteDir.size())
                continue;
            if(deleteDir == str.left(deleteDir.size()))
            {
                if(list_local_real.at(i)->isDir())
                {
                    isdeleted = QDir().rmdir(str);
                    qDebug()<<"deletedir"<<deleteDir<<isdeleted;
                }
                else
                {
                    isdeleted = QFile().remove(str);
                    qDebug()<<"deletefile"<<str<<isdeleted;
                }
                if(isdeleted)
                {
                    index = getLocalInfoIndexByName(str);

                    if(index>=0)
                    {
                        list_local.removeAt(index);
                        list_local_index.removeAt(index);
                        emit localListChanged();
                    }
                }
            }
        }
//        list_local_real.at()
    }
}

void syncTable::localDelete(double deleteFileId)
{
    bool isdeleted = false;

    //获取本地文件信息
    int index = list_local_index.indexOf(QString::number(deleteFileId));
    if(index == -1)
    {
        qDebug()<<"[localDelete]:can't find local file"<<deleteFileId;
        return;
    }


    syncLocalInfo* lInfo = list_local.at(index);

    if(!lInfo->isDir)
    {
        QFile file(lInfo->syncPath);
        if(file.exists())
        {
            isdeleted = file.remove();
            qDebug()<<"deletefile"<<lInfo->syncPath<<isdeleted;
        }
        if(isdeleted)
        {
            index = getLocalInfoIndexByName(lInfo->syncPath);
            if(index>=0)
            {//qDebug()<<"remove"<<index;
                lInfo = list_local.takeAt(index);
                list_local_index.removeAt(index);
                delete lInfo;
                emit localListChanged();
            }
        }
    }
    else
    {
        int i = list_local_real.count()-1;
        QString deleteDir = lInfo->syncPath;
        QString str;
        qDebug()<<"delete ready"<<deleteDir;

        for(; i>=0; i--)
        {
            if(!list_local_real.at(i)->exists())
            {
                qDebug()<<list_local_real.at(i)->absoluteFilePath()<<"not exist";
                continue;
            }

            str = list_local_real.at(i)->absoluteFilePath();
            qDebug()<<str;
            if(str.size() < deleteDir.size())
                continue;
            if(deleteDir == str.left(deleteDir.size()))
            {
                if(list_local_real.at(i)->isDir())
                {
                    isdeleted = QDir().rmdir(str);
                    qDebug()<<"deletedir"<<str<<isdeleted;
                }
                else if(list_local_real.at(i)->isFile())
                {
                    isdeleted = QFile().remove(str);
                    qDebug()<<"deletefile"<<str<<isdeleted;
                }
                if(isdeleted)
                {
                    index = getLocalInfoIndexByName(str);

                    if(index>=0)
                    {
                        list_local.removeAt(index);
                        list_local_index.removeAt(index);
                        emit localListChanged();
                    }
                }
            }
        }
//        list_local_real.at()
    }
}

void syncTable::addSyncLocalInfo(syncLocalInfo *info)
{
    int i=0;
    syncLocalInfo* lInfo;
    for(i=0; i<list_local.count(); i++)
    {
        if(info->fileId == list_local.at(i)->fileId)
        {//qDebug()<<"[addSyncLocalInfo]"<<"repeat"<<info->fileId<<list_local.count();
            lInfo = list_local.takeAt(i);
            delete lInfo;
            list_local.insert(i, info);
           // qDebug()<<list_local.count();
            return;
        }
    }
    list_local<<info;//qDebug()<<"[addSyncLocalInfo]"<<info->syncPath;
    list_local_index<<QString::number(info->fileId);
}

void syncTable::addSyncDownloadInfo(syncInfo *info)
{
    int i=0;
//    syncInfo* lInfo;
    for(i=0; i<list_sync_download.count(); i++)
    {
        if(info->ID == list_sync_download.at(i)->ID)
        {//qDebug()<<"repeat"<<info->ID;
            return;
        }
    }
    list_sync_download<<info;
}

void syncTable::syncNextDir()
{
    if(list_task.isEmpty())
    {
        emit hostSyncFinished();
//        syncClient->netSync();
        return;
    }
    syncInfo* info = list_task.takeFirst();
    syncClient->netSync(info->ID, syncTime);
    qDebug()<<"[SYNC DIR]"<<info->FILE_NAME<<list_task.count()<<info->ID<<info->PARENT_ID;
}

void syncTable::updateParentDate(double id)
{
    int pId = id;
    int index = 0;
    syncLocalInfo* info;

    while(pId != -1)
    {
        index = list_local_index.indexOf(QString::number(pId));
        if(index == -1)
            return;
        info = list_local.takeAt(index);
        qDebug()<<"[updateParentDate]"<<info->lastDate.toString("yyyy/MM/dd hh:mm:ss")<<QFileInfo(info->syncPath).lastModified().toString("yyyy/MM/dd hh:mm:ss");
        info->lastDate = QFileInfo(info->syncPath).lastModified();
        pId = info->parentId;
        list_local.insert(index, info);
    }
}

void syncTable::updateFileDate(double id)
{
    int pId = id;
    int index = 0;
    syncLocalInfo* info;

    index = list_local_index.indexOf(QString::number(pId));
    if(index == -1)
        return;
    info = list_local.takeAt(index);
    qDebug()<<"[updateParentDate]"<<info->lastDate.toString("yyyy/MM/dd hh:mm:ss")<<QFileInfo(info->syncPath).lastModified().toString("yyyy/MM/dd hh:mm:ss");
    info->lastDate = QFileInfo(info->syncPath).lastModified();
    pId = info->parentId;
    list_local.insert(index, info);

}

void syncTable::reportSyncNum()
{
    emit syncUploadChanged(list_sync_upload.count());//syncList::syncUploadMsg
    emit syncDownloadChanged(list_sync_download.count());//syncList::syncDownloadMsg
}

void syncTable::recvListClear()
{qDebug()<<"[recvListClear]";
    list_dir.clear();
    list_file.clear();
    list_index.clear();
    qDeleteAll(list_all.begin(), list_all.end());
    list_all.clear();

//    while(!list_all.isEmpty())
//    {
//        info = list_all.takeFirst();
//        qDebug()<<(int)info;
//        delete info;
//    }
}

void syncTable::tempListToHostList()
{
    syncInfo* info;

    while(!list_temp.isEmpty())
    {
        info = list_temp.takeFirst();
        list_all<<info;
        list_index<<QString::number(info->ID);
    }
    //    qDebug()<<"all size"<<list_all.count()<<"index size"<<list_index.count();
}

void syncTable::mkdirInHost(double pId, QString dirName)
{
    qDebug()<<"mkdir"<<pId<<dirName;
    QString nUrl;
    QStringList params;
//    nUrl = netConf->getServerAddress() + "api/file/createFolder?"+QString("pid=%1&name=%2").arg(pId).arg(QString(fileName.toUtf8().toPercentEncoding()).replace('%','\\x'));
    nUrl = netConf->getServerAddress() + "/api/file/createFolder";
    params<<QString("pid=%1&").arg(pId)<<QString("name=%1&").arg(dirName)<<QString(APP_ID)+"&";
//    QByteArray qba = QString("pid=%1&name=").arg(pId).toLocal8Bit()+fileName.toUtf8();
    QByteArray qba = getPost(params);
    qDebug()<<"[MKDIR]"<<qba;
    QNetworkRequest request(nUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    reply = netConf->manager->post(request,qba);
    connect(reply, SIGNAL(finished()), this, SLOT(recvMkdirRst()));
}

QByteArray syncTable::getPost(QStringList param)
{
    QByteArray postData = QByteArray();
    qSort(param.begin(), param.end());
    QString str = QString();
    for(int i=0; i<param.count(); i++)
    {
        str += param.at(i);
    }
    str += QString("token=%1&").arg(netConf->token);
    postData += str.toUtf8();
    str += QString(APP_KEY);

    QByteArray sign = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5);
    postData += QString("sign=%1").arg(QString(sign.toHex())).toLocal8Bit();
    qDebug()<<"[post params]"<<postData;
    return postData;
}

QByteArray syncTable::getSign(QStringList param)
{
    qSort(param.begin(), param.end());
    QString str = QString();
    for(int i=0; i<param.count(); i++)
    {
        str += param.at(i);
    }

    str += QString(APP_KEY);
    qDebug()<<str;

    QByteArray qba = QCryptographicHash::hash(str.toLocal8Bit(), QCryptographicHash::Md5);
    qDebug()<<qba.toHex();
    return qba;
}

void syncTable::recvMkdirRst()
{
    disconnect(reply, SIGNAL(finished()), this, SLOT(recvMkdirRst()));
    QByteArray qba = reply->readAll();
    qDebug()<<qba;
    syncLocalInfo* lInfo = new syncLocalInfo;
    QJsonParseError jError;
    QJsonValue jval;
    QJsonDocument parseDoc = QJsonDocument::fromJson(qba, &jError);

    if(jError.error == QJsonParseError::NoError)
    {
        if(parseDoc.isObject())
        {
            QJsonObject obj = parseDoc.object();

            if(obj.contains("msg"))
            {
                jval = obj.take("msg");
                qDebug()<<"[recvMkdirRst]"<<jval.toString();
            }
            if(obj.contains("code"))
            {
                jval = obj.take("code");
                if(jval.toString() != "200")
                    return;
            }
            if(obj.contains("result"))
            {
                jval = obj.take("result");
                obj = jval.toObject();

                if(obj.contains("FILE_NAME"))
                {
                    jval = obj.take("FILE_NAME");
                    lInfo->fileName = jval.toString();
                }
                if(obj.contains("ID"))
                {
                    jval = obj.take("ID");
                    lInfo->fileId = jval.toDouble();
                }
                if(obj.contains("PARENT_ID"))
                {
                    jval = obj.take("PARENT_ID");
                    lInfo->parentId = jval.toDouble();
                }
                lInfo->isDir = 1;
                lInfo->fileMd5 = QString();
                lInfo->fileSize = 0;
                lInfo->syncPath = getPathById(lInfo->parentId)+"/"+lInfo->fileName;
                lInfo->lastDate = QFileInfo(lInfo->syncPath).lastModified();
                qDebug()<<"recv"<<lInfo->syncPath;
                addSyncLocalInfo(lInfo);
//                list_local<<lInfo;
//                list_local_index<<QString::number(lInfo->fileId);
                emit localListChanged();
            }
        }
    }
    syncMkDir();
}

void syncTable::recvDeleteRst()
{
    disconnect(deleteReply, SIGNAL(finished()), this, SLOT(recvDeleteRst()));
    QByteArray qba = deleteReply->readAll();
    qDebug()<<"[recvDeleteRst]"<<qba;
    QJsonParseError jError;
    QJsonValue jval;
    QJsonDocument parseDoc = QJsonDocument::fromJson(qba, &jError);

    if(jError.error == QJsonParseError::NoError)
    {
        if(parseDoc.isObject())
        {
            QJsonObject obj = parseDoc.object();

            if(obj.contains("msg"))
            {
                jval = obj.take("msg");
                qDebug()<<jval.toString();
            }
            if(obj.contains("code"))
            {
                jval = obj.take("code");
                if(jval.toString() == "200")
                    localDelete(delFile);
            }
        }
    }
}


void syncTable::creatSyncUploadList()
{
    if(syncInfoNeedUpdate)
        return;
    qDebug()<<"[creatSyncUploadList]";
    //创建同步上传链表
    QFileInfo* localInfoReal;
    syncInfo* localInfo;
    int i=0;
    bool isUpdated;

    while(!list_sync_upload.isEmpty())
    {
        localInfo = list_sync_upload.takeFirst();
        delete localInfo;
    }

    localInfo = new syncInfo;
    for(i=0; i<list_local_real.count(); i++)
    {

        localInfoReal = list_local_real.at(i);
        if(fileIsDownloading(localInfoReal->absoluteFilePath()))
            continue;
        if(localInfoReal->isDir())
        {
            continue;
        }
        localInfo->PARENT_ID = getIdByName(localInfoReal->absolutePath());
        localInfo->ID = getIdByName(localInfoReal->absoluteFilePath(),&isUpdated);
        localInfo->FILE_NAME = localInfoReal->absoluteFilePath();//此处使用FILE_NAME保存待上传的本地文件路径
//        qDebug()<<"isupdated"<<isUpdated;
        if(!isUpdated)
            continue;
        list_sync_upload<<localInfo;
        localInfo = new syncInfo;
    }
    qDebug()<<"creat up list count"<<list_sync_upload.count();
    if(list_sync_upload.count()>0)
        reportSyncNum();
//    if(netConf->autoSyncDir())
    emit syncUpload();//NetSync::syncUploadStart()
}

void syncTable::creatSyncDownloadList()
{
    if(syncInfoNeedUpdate)
        return;
    int i = 0;
    syncInfo* sInfo;
    syncLocalInfo* lInfo;
    qDebug()<<"[creatSyncDownloadList]";
    for(i=0; i<list_file.count(); i++)
    {
        sInfo = list_file.at(i);
        if(getPathById(sInfo->ID).isEmpty())
            addSyncDownloadInfo(sInfo);
    }

    for(i=0; i<list_local.count(); i++)
    {
        lInfo = list_local.at(i);
        if((lInfo->isDir)&&(!QFileInfo(lInfo->syncPath).isDir()))
        {
            QDir dir;
            dir.mkdir(lInfo->syncPath);
            lInfo->lastDate = QFileInfo(lInfo->syncPath).lastModified();
        }
        else if((!lInfo->isDir)&&(!QFileInfo(lInfo->syncPath).isFile()))
        {
            sInfo = new syncInfo;
            sInfo->FILE_NAME = lInfo->fileName;
            sInfo->MD5 = lInfo->fileMd5;
            sInfo->ID = lInfo->fileId;
            sInfo->PARENT_ID = lInfo->parentId;
            addSyncDownloadInfo(sInfo);
        }
    }
    qDebug()<<"down list count"<<list_sync_download.count();
    emit syncDownload();//NetSync::syncDownloadStart()
}

int syncTable::getTaskNum(int &down, int &up)
{
    up = getUploadTaskNum();
    down = getDownloadTaskNum();
    emit syncTaskChanged(uploadTaskNum, downloadTaskNum);
    return (up+down);
}

int syncTable::getUploadTaskNum()
{
    if(syncInfoNeedUpdate)
        return -1;
    qDebug()<<"[getUploadTaskNum]";
    //创建同步上传链表
    QFileInfo* localInfoReal;
    int i=0;
    bool isUpdated;
    uploadTaskNum = 0;

    for(i=0; i<list_local_real.count(); i++)
    {
        localInfoReal = list_local_real.at(i);
        if(fileIsDownloading(localInfoReal->absoluteFilePath()))
            continue;
        if(localInfoReal->isDir())
        {
            continue;
        }
        getIdByName(localInfoReal->absoluteFilePath(),&isUpdated);
//        qDebug()<<"isUpdated"<<isUpdated;
        if(!isUpdated)
            continue;
        uploadTaskNum++;
    }
    qDebug()<<"up list count"<<uploadTaskNum;
//    emit syncUploadChanged(uploadTaskNum);
    return uploadTaskNum;
}

int syncTable::getDownloadTaskNum()
{
    if(syncInfoNeedUpdate)
        return -1;
    int i = 0;
    syncInfo* sInfo;
    syncLocalInfo* lInfo;
    qDebug()<<"[getDownloadTaskNum]";

    downloadTaskNum = 0;

    for(i=0; i<list_file.count(); i++)
    {
        sInfo = list_file.at(i);
        if(getPathById(sInfo->ID).isEmpty())//文件id在服务器返回的同步列表里，但不在本地.sync缓存文件中
        {
            qDebug()<<sInfo->FILE_NAME<<sInfo->ID;
            downloadTaskNum++;
            qDebug("++1");
        }
    }

    for(i=0; i<list_local.count(); i++)
    {
        lInfo = list_local.at(i);
        if((!lInfo->isDir)&&(!QFileInfo(lInfo->syncPath).isFile()))//文件在.sync缓存中，但本地无此文件
        {
//            sInfo = new syncInfo;
//            sInfo->FILE_NAME = lInfo->fileName;
//            sInfo->MD5 = lInfo->fileMd5;
//            sInfo->ID = lInfo->fileId;
//            sInfo->PARENT_ID = lInfo->parentId;
//            addSyncDownloadInfo(sInfo);
            downloadTaskNum++;
            qDebug("++2");
        }
        else if((lInfo->isDir)&&(!QFileInfo(lInfo->syncPath).isDir()))//目录在.sync缓存中，但本地无此文件夹
        {
            QDir dir;
            dir.mkdir(lInfo->syncPath);
            lInfo->lastDate = QFileInfo(lInfo->syncPath).lastModified();
        }
    }
    qDebug()<<"down list count"<<downloadTaskNum;
//    emit syncDownloadChanged(downloadTaskNum);//syncList::syncDownloadMsg
//    if(netConf->autoSyncDir())
//        emit syncDownload();
    return downloadTaskNum;
}

void syncTable::clearSyncList()
{

}

DownloadTaskInfo::DownloadTaskInfo()
{

}

DownloadTaskInfo::~DownloadTaskInfo()
{
    if(info == NULL)
        delete info;
}
