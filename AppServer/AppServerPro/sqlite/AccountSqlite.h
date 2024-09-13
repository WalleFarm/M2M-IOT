#ifndef ACCOUNTSQLITE_H
#define ACCOUNTSQLITE_H

#include <QObject>
#include "BaseSqlite.h"

class AccountSqlite : public BaseSqlite
{
    Q_OBJECT
public:
    typedef struct
    {
        QString account;  //账户名
        QString passWord;  //密码
        u32 auth;  //权限值
        QString parentAccount; //父账号
        QString phone; //关联手机号
        QList<u32>appList;  //名下的应用列表
        QString createTime; //创建时间
    }AccountNodeStruct;  //账户节点

    typedef struct
    {
        u32 appID;   //应用ID
        QString appName; //应用名称
        QString creator;  //建立者的账户名
        QString createTime; //创建时间
    }AppNodeStruct;  //应用节点
    
public:
    explicit AccountSqlite(QObject *parent = nullptr);
    
    bool createAccountListTable(void);
    bool addAccountNode(QString account, QString pass_word, u32 auth, QString parent_account, QString phone);
    bool updateAccountPassWord(QString account, QString pass_word);
    bool updateAccountPhone(QString account, QString phone);
    bool updateAccountAuth(QString account, u32 auth);
    bool updateAccountAppList(QString account, QList<u32>app_list);
    bool selectAccountByName(QString account, AccountNodeStruct &account_node);
    bool selectAccountByPhone(QString phone, AccountNodeStruct &account_node);
    bool selectChildAccountList(QString parent_account, QList<AccountNodeStruct>&child_list);
    bool searchAccountByName(QString account, AccountNodeStruct &account_node);
    bool searchAccountByPhone(QString phone, AccountNodeStruct &account_node);
    bool selectAccountAppList(QString account, QList<u32> &app_list);
    
    bool createAppListTable(void);
    bool addAppIDToList(u32 app_id, QString creator);
    bool updateAppName(u32 app_id, QString app_name);
    QString selectAppName(u32 app_id);
    bool selectAppInfo(u32 app_id, AppNodeStruct &app_node);
    u32 selectMaxAppID(void);
    u32 getAppCountFromAccount(QString account);

private:
    u8 m_keyBuff[16];
    DrvCommon drv_com;
signals:
    
};

#endif // ACCOUNTSQLITE_H
