#include "AccountMan.h"

//账户管理


AccountMan::AccountMan(QObject *parent) : QObject(parent)
{
    m_accountWork.account.clear();
    m_accountWork.login_time=0;
    m_accountWork.auth=0;
    m_rootPath=CFG_ROOT_FILE;
    #if defined(Q_OS_IOS)
    m_rootPath=QDir::homePath()+QString("/Documents/")+CFG_ROOT_FILE;
    #endif
    drv_com.createMutiFile(m_rootPath);    
    
    m_secCounts=0;
    checkTimer = new QTimer(this);
    checkTimer->setInterval(1000);//心跳检测
    checkTimer->start();
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout()));
    
}

void AccountMan::slotCheckTimeout(void)
{
    m_secCounts++;
    if(m_secCounts==2)
    {
        readConfig();
    }

}


void AccountMan::setMacInfo(QString mac, int rand_num)
{
    m_macStr=mac;
    m_randNum=rand_num;
    u16 crcValue=drv_com.crc16((u8*)mac.toLatin1().data(), mac.size());
    
    drv_com.get_rand_key(crcValue%30, crcValue%10, m_keyBuff);//敏感数据加密密码
    QByteArray ba((char*)m_keyBuff, 16);
//    qDebug()<<"m_keyBuff"<<ba.toHex(' ');
    
}

QString AccountMan::makePubTopic(QString key_str)
{
    QString topic=QString(TOPIC_HEAD)+"as/sub/account/"+key_str;
    return  topic;
}

void AccountMan::readConfig(void)
{
    QString path=m_rootPath+"/account.txt";
    QJsonObject root_obj=drv_com.readConfg(path, m_keyBuff);
    
    if(root_obj.contains("account"))
    {
        QString account_str=root_obj.value("account").toString(); 
        qDebug()<<"account_str="<<account_str;
        m_accountWork.account=account_str;
    }
    if(root_obj.contains("login_time"))
    {
        qint64 login_time=root_obj.value("login_time").toDouble();
        qDebug()<<"login_time="<<login_time;
        m_accountWork.login_time=login_time;
    }
    qint64 det_time=QDateTime::currentDateTime().toTime_t()-m_accountWork.login_time;
    qDebug()<<"det_time="<<det_time;
    if(checkAccount(m_accountWork.account)==0 && det_time<86400*3)
    {
        setViewByLoginState(1);//免登录,发给前端
        writeConfig();//更新登录时间
        m_accountWork.login_state=1;
        emit sigUpdateLoginAccount(m_accountWork.account, 1);//发给控制中心
        emit siqUpdateLoginInfo(m_accountWork.login_state, m_accountWork.account, m_accountWork.auth);//更新"我的"账户信息
    }
    else
    {
        setViewByLoginState(0);//引导登录
    }
}

void AccountMan::writeConfig(void)
{
    QString path=m_rootPath+"/account.txt";
    QJsonObject root_obj;
    QString account_str=m_accountWork.account;
    root_obj.insert("account", account_str);
    root_obj.insert("login_time", (qint64)QDateTime::currentDateTime().toTime_t());
    
    drv_com.writeConfig(path, root_obj, m_keyBuff);

}

//根据登录状态切换页面
void AccountMan::setViewByLoginState(int state)
{
    qDebug("setViewByLoginState=%d", state);
    m_accountWork.login_state=state;
    if(state>0)//已登录
    {
        emit siqSetMainCurrView("main-center");
        emit siqSetHomeCurrView("home-logined");
    }
    else//未登录
    {
        m_accountWork.account.clear();
        m_accountWork.auth=0;
        m_accountWork.parent_account.clear();
        writeConfig();
        emit siqSetMainCurrView("main-login");
        emit siqSetHomeCurrView("home-quit");
    }
}

int AccountMan::isPhoneNumber(QString phone_str)
{
    if(phone_str.size()==11)
    {
        bool ok;
        phone_str.toULongLong(&ok);
        return ok;
    }
    return  false;
}

int AccountMan::isNumber(QString num_str)
{
    bool ok;
    num_str.toULongLong(&ok);
    return num_str.size()*ok;
}

int AccountMan::checkAccount(QString str)
{
    bool ok;
    str.toDouble(&ok);
    int result=0;
    if(str.size()<5)
    {
        result=1;
    }
    else if(str=="admin" || str=="Admin")
    {
        result=2;
    }
    else if(ok)
    {
         result=3;
    }
    
    
    return result;
}

int AccountMan::checkPasswd(QString str)
{
    int result=0;
    if(str.size()<8)
    {
        result=1;
    }
     return result;
}

void AccountMan::setMainCurrView(QString view_name)
{
    emit siqSetMainCurrView(view_name);
}

void AccountMan::setHomeCurrView(QString view_name)
{
    emit siqSetHomeCurrView(view_name);
}


void AccountMan::requestVerCode(QString account, QString phone)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "req_vercode");
    root_obj.insert("account", account);
    root_obj.insert("phone", phone);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("code");
    emit sigMqttPushMessage(topic, msg_ba);
}

void AccountMan::requestReg(QString account, QString passwd, QString phone, QString ver_code)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "req_reg");
    root_obj.insert("account", account);
    root_obj.insert("pass_word", passwd);
    root_obj.insert("phone", phone);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr);
    root_obj.insert("ver_code", ver_code);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("reg");
    emit sigMqttPushMessage(topic, msg_ba);
}


void AccountMan::requestLogin(QString account, QString pass_word, int remember)
{
    if(remember){}
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("cmd_type", "login_pwd");
    root_obj.insert("account", account);
    root_obj.insert("pass_word", pass_word);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("login");
    emit sigMqttPushMessage(topic, msg_ba);
}


void AccountMan::requestLoginByVerCode(QString phone, QString ver_code)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("cmd_type", "login_code");
    root_obj.insert("phone", phone);
    root_obj.insert("ver_code", ver_code);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("login");
    emit sigMqttPushMessage(topic, msg_ba);
}

void AccountMan::requestResetPasswd(QString account, QString pass_word, QString phone, QString ver_code)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("cmd_type", "reset_pwd");
    root_obj.insert("account", account);
    root_obj.insert("pass_word", pass_word);
    root_obj.insert("phone", phone);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr);
    root_obj.insert("ver_code", ver_code);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("passwd");
    emit sigMqttPushMessage(topic, msg_ba);
}

void AccountMan::requestChangePwd(QString account, QString old_pwd, QString new_pwd)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "change_pwd");
    root_obj.insert("account", account);
    root_obj.insert("old_pwd", old_pwd);
    root_obj.insert("new_pwd", new_pwd);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("passwd");
    emit sigMqttPushMessage(topic, msg_ba);
}


void AccountMan::parseAccountRecv(QString topic, QJsonObject root_obj)
{
    QString cmd_type="";
    if(root_obj.contains("cmd_type"))//命令类型
    {
        QJsonValue value = root_obj.value("cmd_type");
        if(value.isString())
        {
            cmd_type=value.toString();
        }
    }
    QString ack_str;
    if(root_obj.contains("ack_str"))
    {
        QJsonValue value = root_obj.value("ack_str");
        if(value.isString())
        {
            ack_str=value.toString();
        }
    }
    if(!ack_str.isEmpty())
    {
        emit siqShowMsg(ack_str);
    }
    
    int result=-1;
    if(root_obj.contains("result"))
    {
        QJsonValue value = root_obj.value("result");
        if(value.isDouble())
        {
            result=value.toInt();
        }
    }
    
    if(topic.contains("/reg"))//注册相关
    {
        
    }
    else if(topic.contains("/login"))//登录相关
    {
        if(result==0)//登录成功
        {
            setViewByLoginState(1);//页面切换
            if(root_obj.contains("account"))
            {
                QJsonValue value = root_obj.value("account");
                m_accountWork.account=value.toString();
            }
            if(root_obj.contains("phone"))
            {
                QJsonValue value = root_obj.value("phone");
                m_accountWork.phone=value.toString();
            }
            if(root_obj.contains("auth"))
            {
                QJsonValue value = root_obj.value("auth");
                m_accountWork.auth=value.toDouble();
            }
            if(root_obj.contains("parent_account"))
            {
                QJsonValue value = root_obj.value("parent_account");
                m_accountWork.parent_account=value.toString();
            }
            writeConfig();//记住登录账户,下次快速登录
            emit sigUpdateLoginAccount(m_accountWork.account, 1);//通知控制中心
            emit siqUpdateLoginInfo(m_accountWork.login_state, m_accountWork.account, m_accountWork.auth);//更新"我的"账户信息
        }
    }

}
















