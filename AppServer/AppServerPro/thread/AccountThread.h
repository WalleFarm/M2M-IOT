#ifndef ACCOUNTTHREAD_H
#define ACCOUNTTHREAD_H

#include <QObject>
#include "sqlite/AccountSqlite.h"

#include "user_opt.h"

class AccountThread : public QObject
{
    Q_OBJECT
public:
    typedef struct
    {
        QString account;
        QString phone;
        QString verCode;
        u32 reqTime;
    }ReqVerCodeStruct;
    
public:
    explicit AccountThread(QObject *parent = nullptr);
    QString takeVerCode(void);
    bool addReqVerCodeNode(QString account, QString phone, QString ver_code);
    void checkReqVerCodeList(void);
    QString makePubTopic(QString account, QString mac_str, int rand_num, QString key_str);
    
    void parseVerCodeTopic(QJsonObject root_obj);
    void parseRegTopic(QJsonObject root_obj);
    void parseLoginTopic(QJsonObject root_obj);
    void parsePasswdTopic(QJsonObject root_obj);
    
    
    void ackReqVerCodeState(QString account, QString mac_str, int rand_num, QString phone, int result, QString ack_str);
    void ackReqRegState(QString account, QString mac_str, int rand_num, QString phone, int result, QString ack_str);
    void ackLoginState(QString account,  QString phone, QString parent_account, QString mac_str, int rand_num, u32 auth, QString create_time, int result, QString ack_str);
    void ackResetPasswdState(QString account, QString mac_str, int rand_num, int result, QString ack_str);
    void ackChangePwdState(QString account, QString mac_str, int rand_num, int result, QString ack_str);
    
private:
    QTimer *checkTimer;
    u32 m_timerCounts, m_secCounts;
    DrvCommon drv_com;
    AccountSqlite *m_accountSqlite;
    
    QList<ReqVerCodeStruct> m_reqVerCodeList;
signals:
    void sigMqttPushMessage(QString topic, QByteArray msg_ba);
    
public slots:
    void slotCheckTimeout(void);
    void slotAccountThreadMessage(QString topic, QJsonObject root_obj);
};

#endif // ACCOUNTTHREAD_H
