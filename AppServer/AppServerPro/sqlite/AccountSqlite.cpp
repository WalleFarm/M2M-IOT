#include "AccountSqlite.h"

AccountSqlite::AccountSqlite(QObject *parent) : BaseSqlite(parent)
{
    drv_com.get_rand_key(32, 5, m_keyBuff);
}

//建立账户列表  数据库表
bool AccountSqlite::createAccountListTable(void)
{
    QString str_query = "CREATE TABLE If Not Exists account_list_tb ("
            "id INTEGER NOT NULL,"
            "account char(30) NOT NULL UNIQUE,"
            "pass_word varchar(50) NOT NULL,"
            "auth bigint DEFAULT 0,"
            "parent_account char(30) DEFAULT NULL,"
            "phone char(30) DEFAULT NULL UNIQUE,"
            "app_list varchar(5000) DEFAULT 0,"
            "create_time timestamp DEFAULT (datetime(\'now\',\'localtime\')),"
            "PRIMARY KEY (id)"
          ") ";


    if(runSqlQuery(str_query)==false)
    {
        qDebug("createAccountsTable error!");
        return false;
    }
    return true;
}

//添加一个账户信息到数据库
bool AccountSqlite::addAccountNode(QString account, QString pass_word, u32 auth, QString parent_account, QString phone)
{
    if(phone.isEmpty())
    {
        phone=account;
    }
    if(pass_word.isEmpty())
    {
        pass_word="12345678";
    }
    u8 out_passwd[50]={0};
    int out_len=drv_com.aes_encrypt_buff((u8*)pass_word.toUtf8().data(), (u16)pass_word.toUtf8().size(), out_passwd, (u16)sizeof (out_passwd), m_keyBuff);//密码加密存储
    if(out_len<16)
        return  false;
    QByteArray pwd_ba((char*)out_passwd, out_len);
    pass_word=pwd_ba.toBase64();
    QString str_query = QString::asprintf("INSERT INTO account_list_tb (account, pass_word, auth, parent_account, phone) VALUES (  \"%s\", \"%s\", %u, \"%s\", \"%s\")",\
                                          account.toUtf8().data(), pass_word.toUtf8().data(), auth, \
                                          parent_account.toUtf8().data(), phone.toUtf8().data());
    if( runSqlQuery(str_query))
    {
        qDebug("addAccountNode ok!");
        return true;
    }

    qDebug("addAccountNode failed!");
    return false;
}

//更新账户密码
bool AccountSqlite::updateAccountPassWord(QString account, QString pass_word)
{
    if(pass_word.isEmpty())
    {
        pass_word="12345678";
    }
    u8 out_passwd[50]={0};
    int out_len=drv_com.aes_encrypt_buff((u8*)pass_word.toUtf8().data(), (u16)pass_word.toUtf8().size(), out_passwd, (u16)sizeof (out_passwd), m_keyBuff);
    if(out_len<16)
        return  false;
    QByteArray pwd_ba((char*)out_passwd, out_len);
    pass_word=pwd_ba.toBase64();

    QString str_query = QString::asprintf("UPDATE account_list_tb SET pass_word=\"%s\"   WHERE account=\"%s\"",
                                          pass_word.toUtf8().data(), account.toUtf8().data());
//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("updateAccountPassWord ok!");
        return true;
    }
    return false;
}

//更新账户关联的手机号
bool AccountSqlite::updateAccountPhone(QString account, QString phone)
{
    QString str_query = QString::asprintf("UPDATE account_list_tb SET phone=\"%s\"   WHERE account=\"%s\"",
                                          phone.toUtf8().data(), account.toUtf8().data());
//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("updateAccountPhone ok!");
        return true;
    }
    return false;
}

//更新账户权限 
bool AccountSqlite::updateAccountAuth(QString account, u32 auth)
{
    QString str_query = QString::asprintf("UPDATE account_list_tb SET auth=%u   WHERE account=\"%s\"",
                                          auth, account.toUtf8().data());
//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("updateAccountAuth ok!");
        return true;
    }
    return false;
}

//更新账户的应用列表
bool AccountSqlite::updateAccountAppList(QString account, QList<u32> app_list)
{
    QJsonDocument json_doc;
    QJsonObject root_obj;
    QJsonArray app_array;
    int nSize=app_list.size();
    for(int i=0; i<nSize; i++)
    {
        u32 app_id=app_list.at(i);
        if(app_id>0)
        {
            app_array.append((qint64)app_id);
        }
    }
    root_obj.insert("app_list", app_array);
    json_doc.setObject(root_obj);
    QByteArray json_ba=json_doc.toJson(QJsonDocument::Compact);//转为数据保存
    qDebug()<<"json_ba="<<json_ba.data();
    QString str_query = QString::asprintf("UPDATE account_list_tb SET  app_list=\"%s\"   WHERE account=\"%s\"",
                                          json_ba.toBase64().data(), account.toUtf8().data());

//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("updateAccountAppList ok!");
        return true;
    }
    return false;
}

//根据名称 获取帐号信息
bool AccountSqlite::selectAccountByName(QString account, AccountNodeStruct &account_node)
{
    QString str_query = QString::asprintf("SELECT account, pass_word, auth, parent_account, phone, app_list, create_time  FROM account_list_tb WHERE account=\"%s\"" , account.toUtf8().data());

//    qDebug()<<str_query;
    if(runSqlQuery(str_query)==false)
    {
        qDebug("selectAccountNode error_01!");
         return false;
    }
    while(m_sqlQuery.next())
    {
        int ptr=0;
        account_node.account=m_sqlQuery.value(ptr++).toString();
        QString pwd_str=m_sqlQuery.value(ptr++).toString();
        QByteArray pwd_ba=QByteArray::fromBase64(pwd_str.toUtf8());
        u8 out_passwd[50]={0};
        int out_len=drv_com.aes_decrypt_buff((u8*)pwd_ba.data(), (u16)pwd_ba.size(), out_passwd, sizeof (out_passwd), m_keyBuff);//存储密码解密
        if(out_len<=0)
        {
            m_sqlQuery.finish();
            return  false;
        }
        account_node.passWord=QString((char*)out_passwd);
        account_node.auth=m_sqlQuery.value(ptr++).toUInt();
        account_node.parentAccount=m_sqlQuery.value(ptr++).toString();
        account_node.phone=m_sqlQuery.value(ptr++).toString();

        QString json_str=m_sqlQuery.value(ptr++).toString();//应用列表JSON
        QByteArray json_ba=QByteArray::fromBase64(json_str.toUtf8());
        QJsonParseError json_error;
        QJsonDocument json_doc;
        json_doc = QJsonDocument(QJsonDocument::fromJson(json_ba, &json_error));//转为JSON格式
        if(json_error.error != QJsonParseError::NoError)
        {
            qDebug()<<"json error= "<<json_error.error;
        }
        else
        {
            QJsonObject root_obj = json_doc.object();
            if(root_obj.contains("app_list"))//应用列表
            {
                QJsonValue value=root_obj.value("app_list");
                if(value.isArray())
                {
                    QJsonArray app_array=value.toArray();
                    int nSize=app_array.size();
                    for(int i=0; i<nSize; i++)
                    {
                        QJsonValue value=app_array.at(i);
                        if(value.isDouble())
                        {
                            u32 app_id=(u32)value.toDouble();
                            if(app_id>0)
                            {
                                account_node.appList.append(app_id);//添加app_id
                            }
                        }
                    }
                }
            }
        }
        account_node.createTime=m_sqlQuery.value(ptr++).toString();
        m_sqlQuery.finish();
        return true;
    }
    qDebug("selectAccountNode error_02!");
    return false;
}

//根据手机号 获取帐号信息
bool AccountSqlite::selectAccountByPhone(QString phone, AccountNodeStruct &account_node)
{
    QString str_query = QString::asprintf("SELECT account, pass_word, auth, parent_account, phone, app_list, create_time  FROM account_list_tb WHERE phone=\"%s\"" , phone.toUtf8().data());

//    qDebug()<<str_query;
    if(runSqlQuery(str_query)==false)
    {
        qDebug("selectAccountNode error_01!");
         return false;
    }
    while(m_sqlQuery.next())
    {
        int ptr=0;
        account_node.account=m_sqlQuery.value(ptr++).toString();
        QString pwd_str=m_sqlQuery.value(ptr++).toString();
        QByteArray pwd_ba=QByteArray::fromBase64(pwd_str.toUtf8());
        u8 out_passwd[50]={0};
        int out_len=drv_com.aes_decrypt_buff((u8*)pwd_ba.data(), (u16)pwd_ba.size(), out_passwd, sizeof (out_passwd), m_keyBuff);//存储密码解密
        if(out_len<=0)
        {
            m_sqlQuery.finish();
            return  false;
        }
        account_node.passWord=QString((char*)out_passwd);
        account_node.auth=m_sqlQuery.value(ptr++).toUInt();
        account_node.parentAccount=m_sqlQuery.value(ptr++).toString();
        account_node.phone=m_sqlQuery.value(ptr++).toString();

        QString json_str=m_sqlQuery.value(ptr++).toString();//应用列表JSON
        QByteArray json_ba=QByteArray::fromBase64(json_str.toUtf8());
        QJsonParseError json_error;
        QJsonDocument json_doc;
        json_doc = QJsonDocument(QJsonDocument::fromJson(json_ba, &json_error));//转为JSON格式
        if(json_error.error != QJsonParseError::NoError)
        {
            qDebug()<<"json error= "<<json_error.error;
        }
        else
        {
            QJsonObject root_obj = json_doc.object();
            if(root_obj.contains("app_list"))//应用列表
            {
                QJsonValue value=root_obj.value("app_list");
                if(value.isArray())
                {
                    QJsonArray app_array=value.toArray();
                    int nSize=app_array.size();
                    for(int i=0; i<nSize; i++)
                    {
                        QJsonValue value=app_array.at(i);
                        if(value.isDouble())
                        {
                            u32 app_id=(u32)value.toDouble();
                            if(app_id>0)
                            {
                                account_node.appList.append(app_id);//添加app_id
                            }
                        }
                    }
                }
            }
        }
        account_node.createTime=m_sqlQuery.value(ptr++).toString();
        m_sqlQuery.finish();
        return true;
    }
    qDebug("selectAccountNode error_02!");
    return false;
}

//获取子账户列表
bool AccountSqlite::selectChildAccountList(QString parent_account, QList<AccountNodeStruct> &child_list)
{
    QString str_query = QString::asprintf("SELECT account, pass_word, auth, parent_account, phone, app_list, create_time  FROM account_list_tb WHERE parent_account=\"%s\"" , parent_account.toUtf8().data());

//    qDebug()<<str_query;
    if(runSqlQuery(str_query)==false)
    {
        qDebug("selectChildAccountList error_01!");
         return false;
    }
    while(m_sqlQuery.next())
    {
        int ptr=0;
        AccountNodeStruct tag_account;
        tag_account.account=m_sqlQuery.value(ptr++).toString();
        QString pwd_str=m_sqlQuery.value(ptr++).toString();
        QByteArray pwd_ba=QByteArray::fromBase64(pwd_str.toUtf8());
        u8 out_passwd[50]={0};
        int out_len=drv_com.aes_decrypt_buff((u8*)pwd_ba.data(), (u16)pwd_ba.size(), out_passwd, sizeof (out_passwd), m_keyBuff);
        if(out_len<=0)
        {
            m_sqlQuery.finish();
            return  false;
        }
        tag_account.passWord=QString((char*)out_passwd);
        tag_account.auth=m_sqlQuery.value(ptr++).toUInt();
        tag_account.parentAccount=m_sqlQuery.value(ptr++).toString();
        tag_account.phone=m_sqlQuery.value(ptr++).toString();

        QString json_str=m_sqlQuery.value(ptr++).toString();
        QByteArray json_ba=QByteArray::fromBase64(json_str.toUtf8());
        QJsonParseError json_error;
        QJsonDocument json_doc;
        json_doc = QJsonDocument(QJsonDocument::fromJson(json_ba, &json_error));//转为JSON格式
        if(json_error.error != QJsonParseError::NoError)
        {
            qDebug()<<"json error= "<<json_error.error;
        }
        else
        {
            QJsonObject root_obj = json_doc.object();
            if(root_obj.contains("app_list"))//应用列表
            {
                QJsonValue value=root_obj.value("app_list");
                if(value.isArray())
                {
                    QJsonArray app_array=value.toArray();
                    int nSize=app_array.size();
                    for(int i=0; i<nSize; i++)
                    {
                        QJsonValue value=app_array.at(i);
                        if(value.isDouble())
                        {
                            u32 app_id=(u32)value.toDouble();
                            if(app_id>0)
                            {
                                tag_account.appList.append(app_id);//添加app_id
                            }
                        }
                    }
                }
            }
        }
        tag_account.createTime=m_sqlQuery.value(ptr++).toString();
        child_list.append(tag_account);
    }
    m_sqlQuery.finish();
    return true;
}


bool AccountSqlite::searchAccountByName(QString account, AccountNodeStruct &account_node)
{
    QString str_query = QString::asprintf("SELECT account, phone, parent_account  FROM  account_list_tb WHERE account=\"%s\"" , account.toUtf8().data());

//    qDebug()<<str_query;
    if(runSqlQuery(str_query)==false)
    {
        qDebug("searchAccountByName error_01!");
         return false;
    }
    while(m_sqlQuery.next())
    {
        int ptr=0;
        account_node.account=m_sqlQuery.value(ptr++).toString();
        account_node.phone=m_sqlQuery.value(ptr++).toString();
        account_node.parentAccount=m_sqlQuery.value(ptr++).toString();
        m_sqlQuery.finish();
        return true;
    }
    qDebug("searchAccountByName error_02!");
    return false;
}

bool AccountSqlite::searchAccountByPhone(QString phone, AccountNodeStruct &account_node)
{
    QString str_query = QString::asprintf("SELECT account, phone, parent_account  FROM  account_list_tb WHERE phone=\"%s\"" , phone.toUtf8().data());

//    qDebug()<<str_query;
    if(runSqlQuery(str_query)==false)
    {
        qDebug("searchAccountByPhone error_01!");
         return false;
    }
    while(m_sqlQuery.next())
    {
        int ptr=0;
        account_node.account=m_sqlQuery.value(ptr++).toString();;
        account_node.phone=m_sqlQuery.value(ptr++).toString();
        account_node.parentAccount=m_sqlQuery.value(ptr++).toString();
        m_sqlQuery.finish();
//        qDebug("searchAccountByPhone ok!");
        return true;
    }
    qDebug("searchAccountByPhone error_02!");
    return false;
}


bool AccountSqlite::selectAccountAppList(QString account, QList<u32> &app_list)
{
    QString str_query = QString::asprintf("SELECT app_id  FROM  app_id_list WHERE creator=\"%s\" " , account.toUtf8().data());

//    qDebug()<<str_query;
    if(runSqlQuery(str_query)==false)
    {
        qDebug("selectAccountAppList error_01!");
         return false;
    }
    while(m_sqlQuery.next())
    {
        u32 app_id=m_sqlQuery.value(0).toUInt();
        if(app_id>0)
        {
            app_list.append(app_id);
        }
    }
    m_sqlQuery.finish();
    return true;
}



/********************************************************/
//建立应用表
bool AccountSqlite::createAppListTable(void)
{
    QString str_query = QString::asprintf("CREATE TABLE If Not Exists app_id_list ("
                                          "id INTEGER NOT NULL,"
                                          "app_id bigint NOT NULL UNIQUE,"
                                          "app_name char(30) DEFAULT NULL,"
                                          "creator char(30) DEFAULT NULL,"
                                          "create_time timestamp DEFAULT (datetime(\'now\',\'localtime\')),"
                                          "PRIMARY KEY (id)"
                                          ")"
                                         );


    if(runSqlQuery(str_query)==false)
    {
        qDebug("createAppLisTable error!");
        return false;
    }
    return true;
}

//添加应用ID
bool AccountSqlite::addAppIDToList(u32 app_id, QString creator)
{
    QString str_query = QString::asprintf("INSERT INTO app_id_list (app_id, app_name, creator) VALUES ( %u, \"%s\" , \"%s\" )", app_id, "新应用",creator.toUtf8().data());

    if( runSqlQuery(str_query))
    {
        qDebug("addAppIDToList ok!");

        AccountNodeStruct tag_account;
        tag_account.account.clear();
        selectAccountByName(creator, tag_account);
        if(tag_account.account.isEmpty()==false)
        {
            int nSize=tag_account.appList.size();
            for(int i=0; i<nSize; i++)
            {
                if(tag_account.appList.at(i)==app_id)
                {
                    return true;
                }
            }
            tag_account.appList.append(app_id);
            updateAccountAppList(creator, tag_account.appList);//更新应用列表
        }
        return true;
    }

    qDebug("addAppIDToList failed!");
    return false;
}

//更新应用名称
bool AccountSqlite::updateAppName(u32 app_id, QString app_name)
{
    QString str_query = QString::asprintf("UPDATE app_id_list SET app_name=\"%s\"   WHERE app_id=%u",
                                          app_name.toUtf8().data(), app_id);
//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("updateAppName ok!");
        return true;
    }
    return false;
}

//获取应用名称
QString AccountSqlite::selectAppName(u32 app_id)
{
    QString app_name="";
    QString str_query = QString::asprintf("select app_name from app_id_list where app_id=%u", app_id);
    if( runSqlQuery(str_query))
    {
        while(m_sqlQuery.next())
        {
            app_name=m_sqlQuery.value(0).toString();
            break;
        }
        m_sqlQuery.finish();
    }
    return app_name;
}

//查询应用节点信息
bool AccountSqlite::selectAppInfo(u32 app_id, AppNodeStruct &app_node)
{
    QString str_query = QString::asprintf("select app_id, app_name, creator, create_time from app_id_list where app_id=%u", app_id);
//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        while(m_sqlQuery.next())
        {
            int ptr=0;
            app_node.appID=m_sqlQuery.value(ptr++).toUInt();
            app_node.appName=m_sqlQuery.value(ptr++).toString();
            app_node.creator=m_sqlQuery.value(ptr++).toString();
            app_node.createTime=m_sqlQuery.value(ptr++).toString();
            m_sqlQuery.finish();
            return true;
        }
        m_sqlQuery.finish();
    }
    return false;
}


//获取最大的应用ID
u32 AccountSqlite::selectMaxAppID(void)
{
    u32 max_app=0;
    QString str_query = QString::asprintf("SELECT app_id  FROM  app_id_list ORDER BY app_id DESC limit 10");
    if(runSqlQuery(str_query)==false)
    {
        qDebug("selectMaxAppID error_01!");
         return max_app;
    }
    while(m_sqlQuery.next())
    {
        max_app=m_sqlQuery.value(0).toUInt();
        break;
    }
    m_sqlQuery.finish();
//    qDebug()<<"selectAppList="<<app_list;
    return max_app;
}

//获取该账户下的应用数量
u32 AccountSqlite::getAppCountFromAccount(QString account)
{
    u32 app_cnts=0;
    QString str_query = QString::asprintf("select app_id from app_id_list where  creator=\"%s\"", account.toUtf8().data());
//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        while(m_sqlQuery.next())
        {
          app_cnts++;
        }
    }
    return app_cnts;
}












