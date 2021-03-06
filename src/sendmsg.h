// Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>

#ifndef SEND_MSG_H
#define SEND_MSG_H

#include "httppost.h"

class SendMsg : public HttpPost 
{
    Q_OBJECT

public:
    SendMsg(HttpPost* parent = nullptr);
    ~SendMsg();

public:
    Q_INVOKABLE void send(QString uin, 
                          QString sid, 
                          QString skey, 
                          QString fromUserName, 
                          QString toUserName, 
                          QString content, 
                          QStringList syncKey);
    Q_INVOKABLE void post(QString uin, 
                          QString sid, 
                          QString skey, 
                          QString fromUserName, 
                          QString toUserName, 
                          QString content);
    Q_INVOKABLE void sync(QString uin, 
                          QString sid, 
                          QString skey, 
                          QStringList syncKey);

protected:
    void finished(QNetworkReply* reply);
};

#endif // SEND_MSG_H
