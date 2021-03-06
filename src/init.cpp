// Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>

#if QWX_DEBUG
#include <QFile>
#endif
#include <QJsonDocument>                                                           
#include <QJsonObject>                                                             
#include <QJsonArray>
#include <time.h>

#include "init.h"
#include "userobject.h"
#include "globaldeclarations.h"

Init::Init(HttpPost* parent) 
  : HttpPost(parent) 
{
#if QWX_DEBUG
    qDebug() << "DEBUG:" << __PRETTY_FUNCTION__;
#endif
}

Init::~Init() 
{
#if QWX_DEBUG
    qDebug() << "DEBUG:" << __PRETTY_FUNCTION__;
#endif
}

void Init::m_clear() 
{
    foreach (QObject* obj, m_contactList) {
        if (obj) delete obj; obj = nullptr;
    }
    m_contactList.clear();
    m_map.clear();
}

void Init::post(QString uin, QString sid, QString ticket) 
{
    m_clear();

    QString url = WX_SERVER_HOST + WX_CGI_PATH + "webwxinit?pass_ticket=" + 
        ticket + "&r=" + QString::number(time(NULL));
#if QWX_DEBUG
    qDebug() << "DEBUG:" << __PRETTY_FUNCTION__ << url;
#endif
    QString json = "{\"BaseRequest\":{\"Uin\":\"" + uin + "\",\"Sid\":\"" + 
        sid + "\",\"Skey\":\"\",\"DeviceID\":\"" + DEVICE_ID + "\"}}";
#if QWX_DEBUG
    qDebug() << "DEBUG:" << __PRETTY_FUNCTION__ << json;
#endif
    HttpPost::post(url, json, true);
}

QString Init::loginUserName() const { return m_loginUserName; }

QList<QObject*> Init::contactList() const { return m_contactList; }

QString Init::loginHeadImgUrl() const 
{ 
    return "file://" + QWXDIR + "/" + m_loginUserName;
}

void Init::finished(QNetworkReply* reply) 
{
    QString replyStr = QString(reply->readAll());
#if QWX_DEBUG
    QFile file("init.json"); 
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << replyStr;
        file.close();
    }
    qDebug() << "DEBUG:" << __PRETTY_FUNCTION__;
    qDebug() << "DEBUG:" << replyStr;
#endif

    QJsonDocument doc = QJsonDocument::fromJson(replyStr.toUtf8());                
    if (!doc.isObject()) { emit error(); return; }                                 
    QJsonObject obj = doc.object();
    QJsonObject user = obj["User"].toObject();
    m_loginUserName = user["UserName"].toString(); 
#if QWX_DEBUG
    qDebug() << "DEBUG:" << __PRETTY_FUNCTION__ << m_loginUserName;
#endif
    emit loginUserNameChanged();
    emit loginHeadImgUrlChanged();

    foreach (const QJsonValue & val, obj["ContactList"].toArray()) {
        QJsonObject user = val.toObject();
        QString userName = user["UserName"].toString();
        QString nickName = user["NickName"].toString();
        if (!m_map.contains(userName)) {
            m_contactList.append(new UserObject(
                        userName, nickName, 
                        WX_SERVER_HOST + user["HeadImgUrl"].toString()));
        }
        m_map.insert(userName, nickName);
    }
    emit contactListChanged();

    QString skey = obj["SKey"].toString();                                         
#if QWX_DEBUG                                                                      
    qDebug() << "DEBUG:" << __PRETTY_FUNCTION__ << skey;                           
#endif 
    QStringList syncKey;
    foreach (const QJsonValue & val, obj["SyncKey"].toObject()["List"].toArray()) {
        syncKey.append(QString::number(val.toObject()["Val"].toInt()));
    }
    emit skeyChanged(skey, syncKey);
}
