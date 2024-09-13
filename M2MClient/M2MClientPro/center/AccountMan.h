#ifndef ACCOUNTMAN_H
#define ACCOUNTMAN_H

#include <QObject>
#include <QSysInfo>
#include "drivers/DrvCommon.h"
#include "user_opt.h"

class AccountMan : public QObject
{
    Q_OBJECT
public:
    typedef struct
    {
        QString account;
        qint64 login_time;
        u32 auth;
        int login_state;
        QString parent_account;
        QString phone;
        QString create_time;
    }AccountWorkStruct;
    
public:
    explicit AccountMan(QObject *parent = nullptr);
    void setMacInfo(QString mac, int rand_num);
    QString makePubTopic(QString key_str);
    void readConfig(void);
    void writeConfig(void);
    Q_INVOKABLE void setViewByLoginState(int state);
    Q_INVOKABLE int isPhoneNumber(QString phone_str);
    Q_INVOKABLE int isNumber(QString num_str);
    Q_INVOKABLE int checkAccount(QString str);
    Q_INVOKABLE int checkPasswd(QString str);
    Q_INVOKABLE void setMainCurrView(QString view_name);
    Q_INVOKABLE void setHomeCurrView(QString view_name);
    
    Q_INVOKABLE void requestVerCode(QString account, QString phone);
    Q_INVOKABLE void requestReg(QString account, QString passwd, QString phone, QString ver_code);
    Q_INVOKABLE void requestLogin(QString account, QString pass_word, int remember);
    Q_INVOKABLE void requestLoginByVerCode(QString phone, QString ver_code);
    Q_INVOKABLE void requestResetPasswd(QString account, QString pass_word, QString phone, QString ver_code);
    Q_INVOKABLE void requestChangePwd(QString account, QString old_pwd, QString new_pwd);
    
    
    void parseAccountRecv(QString topic, QJsonObject root_obj);
    
private:
    QTimer *checkTimer;
    u32 m_secCounts;
    QString m_macStr;
    int m_randNum;
    DrvCommon drv_com;
    u8 m_keyBuff[16];
    AccountWorkStruct m_accountWork;
    QString m_rootPath;
signals:
    void siqUpdateLoginInfo(int login_state, QString account, int auth);    
    void siqShowMsg(QString msg);
    void siqSetMainCurrView(QString view_name);
    void siqSetHomeCurrView(QString view_name);
signals:
    void sigMqttPushMessage(QString topic, QByteArray msg_ba);
    void sigUpdateLoginAccount(QString account, int state=1);
        
public slots:
    void slotCheckTimeout(void);
    
};

#endif // ACCOUNTMAN_H
