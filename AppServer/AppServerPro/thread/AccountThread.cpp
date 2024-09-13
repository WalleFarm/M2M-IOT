#include "AccountThread.h"

AccountThread::AccountThread(QObject *parent) : QObject(parent)
{
    m_accountSqlite=nullptr;
    m_timerCounts=m_secCounts=0;
    
    checkTimer = new QTimer();
    checkTimer->setInterval(500);//心跳检测
    checkTimer->start();
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout()));
    
    
    drv_com.createMutiFile(DB_ROOT_FILE);
}

void AccountThread::slotCheckTimeout(void)
{
    if(m_timerCounts%2==0)
    {
        m_secCounts++;
        checkReqVerCodeList();//验证码超时检测
        
        if(m_accountSqlite==nullptr)
        {
            qDebug()<<"open account sqlite!";
            m_accountSqlite = new AccountSqlite();
            m_accountSqlite->openDataBase(DB_ROOT_FILE+QString("/account_list.db"), QString::asprintf("account_%d", drv_com.takeRandNumber()));
            m_accountSqlite->createAccountListTable();//账户列表
            m_accountSqlite->createAppListTable();//应用列表
        }   
        
    }
    
    
}

//产生4位数随机验证码
QString AccountThread::takeVerCode(void)
{
    u16 ver_code_num=0;
    for(int i=0; i<4; i++)
    {
        ver_code_num=ver_code_num*10+drv_com.takeRandNumber()%10;
    }
    QString ver_code_str=QString::asprintf("%04u", ver_code_num);
    qDebug()<<" ver_code_str="<<ver_code_str;    
    return ver_code_str;
}

//添加验证码到列表中
bool AccountThread::addReqVerCodeNode(QString account, QString phone, QString ver_code)
{
    ReqVerCodeStruct tag_req;

    if(phone.isEmpty())
        return false;
    int i=0;
    for(auto iter : m_reqVerCodeList)
    {
        if(phone==iter.phone)
        {
            qDebug()<<"have same phone="<<phone;
            return false;
        }
        i++;
    }
    tag_req.account=account;
    tag_req.phone=phone;
    tag_req.verCode=ver_code;
    tag_req.reqTime=m_secCounts;
    m_reqVerCodeList.append(tag_req);
    qDebug("add new req node!");
    return true;
}

//检查验证码超时
void AccountThread::checkReqVerCodeList(void)
{
    int i=0;
    for(auto iter : m_reqVerCodeList)
    {
        int det_time=m_secCounts - iter.reqTime;
        if(det_time>60)
        {
            qDebug()<<"time out! del a req ver code node="<<iter.phone;
            m_reqVerCodeList.removeAt(i);
            return;
        }
        i++;
    }
}

//生成发布话题
QString AccountThread::makePubTopic(QString account, QString mac_str, int rand_num, QString key_str)
{
    if(account.isEmpty())
    {
        
    }
    QString topic=QString(TOPIC_HEAD) + SERVER_PUB_TOPIC+QString("/account/")+mac_str+QString::asprintf("/%d/", rand_num)+key_str;
    return topic;
}


void AccountThread::slotAccountThreadMessage(QString topic, QJsonObject root_obj)
{
    if(topic.contains("/reg"))//注册相关
    {
        parseRegTopic(root_obj);
    }
    else if(topic.contains("/login"))//登录相关
    {
        parseLoginTopic(root_obj);
    }
    else if(topic.contains("/code"))//验证码相关
    {
        parseVerCodeTopic(root_obj);
    }
    else if(topic.contains("/passwd"))//密码相关
    {
        parsePasswdTopic(root_obj);
    }
    else if(topic.contains("/child"))//子账户相关
    {
        
    }

}


//解析验证码话题
void AccountThread::parseVerCodeTopic(QJsonObject root_obj)
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
    QString account="";
    if(root_obj.contains("account"))//账户
    {
        QJsonValue value = root_obj.value("account");
        if(value.isString())
        {
            account=value.toString();
        }
    }

    int rand_num=0;
    if(root_obj.contains("rand_num"))//随机数
    {
        QJsonValue value = root_obj.value("rand_num");
        if(value.isDouble())
        {
            rand_num=(int)value.toDouble();
        }
    }
    QString mac_str="";
    if(root_obj.contains("mac"))//mac
    {
        QJsonValue value = root_obj.value("mac");
        if(value.isString())
        {
            mac_str=value.toString();
        }
    }
    QString phone="";
    if(root_obj.contains("phone"))//手机号码
    {
        QJsonValue value = root_obj.value("phone");
        if(value.isString())
        {
            phone=value.toString();
        }
    }
    
    if(cmd_type=="req_vercode")//请求验证码
    {
        if(phone.isEmpty())
        {
            qDebug()<<"phone.isEmpty()";
            return;
        }
        QString ver_code_str=takeVerCode();
        bool ok=addReqVerCodeNode(account, phone, ver_code_str);//添加进列表,进行超时检测
        if(ok)
        {
            //向SMS服务器发送验证码
//            sendSmsCheckCode(phone, ver_code_str);
            ackReqVerCodeState(account, mac_str, rand_num, phone, 0, "验证码已发送!");//返回验证码已发送状态
            qDebug()<<"req_vercode ok";
        }
        else
        {
            ackReqVerCodeState(account, mac_str, rand_num, phone, 1, "重复发送!");//
        }

    }
}

//解析注册话题
void AccountThread::parseRegTopic(QJsonObject root_obj)
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
    QString account="";
    if(root_obj.contains("account"))//账户
    {
        QJsonValue value = root_obj.value("account");
        if(value.isString())
        {
            account=value.toString();
        }
    }

    int rand_num=0;
    if(root_obj.contains("rand_num"))//随机数
    {
        QJsonValue value = root_obj.value("rand_num");
        if(value.isDouble())
        {
            rand_num=(int)value.toDouble();
        }
    }
    QString mac_str="";
    if(root_obj.contains("mac"))//mac
    {
        QJsonValue value = root_obj.value("mac");
        if(value.isString())
        {
            mac_str=value.toString();
        }
    }
    QString pass_word="";
    if(root_obj.contains("pass_word"))//密码
    {
        QJsonValue value = root_obj.value("pass_word");
        if(value.isString())
        {
            pass_word=value.toString();
        }
    }
    QString phone="";
    if(root_obj.contains("phone"))//手机号码
    {
        QJsonValue value = root_obj.value("phone");
        if(value.isString())
        {
            phone=value.toString();
        }
    }
    QString ver_code="";
    if(root_obj.contains("ver_code"))//验证码
    {
        QJsonValue value = root_obj.value("ver_code");
        if(value.isString())
        {
            ver_code=value.toString();
        }
    }
    
    if(cmd_type=="req_reg")//请求注册
    {
        bool ok;
        account.toDouble(&ok);
        if(ok)
        {
            ackReqRegState(account, mac_str, rand_num, phone, 6, "账户不能为纯数字!");
            return;
        }
        
        AccountSqlite::AccountNodeStruct tag_account;
        bool ok1, ok2;
        ok1=m_accountSqlite->searchAccountByName(account, tag_account);
        ok2=m_accountSqlite->searchAccountByPhone(phone, tag_account);
        if(ok1==false && ok2==false)//未找到重复的,可以注册
        {
            for(auto iter : m_reqVerCodeList)
            {
                if(iter.phone == phone)//根据手机号检索
                {
                    if(iter.verCode == ver_code)//验证码相等
                    {
                        bool ok=m_accountSqlite->addAccountNode(account, pass_word, 0x00, "", phone);
                        if(ok)
                        {
                            ackReqRegState(account, mac_str, rand_num, phone, 0, "注册成功!");
                            qDebug()<<"reg ok!";
                            //自动创建一个应用
                            u32 max_app=m_accountSqlite->selectMaxAppID();
                            u32 new_app_id=0;
                            if(max_app>APP_ID_MIN)
                                new_app_id=max_app+1;
                            else  
                                new_app_id=APP_ID_MIN+1;
                            
                            qDebug()<<"new_app_id="<<new_app_id;
                           m_accountSqlite->addAppIDToList(new_app_id, account);
                        }
                        else
                        {
                            ackReqRegState(account, mac_str, rand_num, phone, 1, "数据库存储出错!");
                            qDebug()<<"reg sql error!!";
                        }
                    }
                    else
                    {
                        ackReqRegState(account, mac_str, rand_num, phone, 2, "验证码错误!");
                        qDebug()<<"reg ver_code error!";
                    }
                    return;
                }
            }
            ackReqRegState(account, mac_str, rand_num, phone, 3, "验证码超时!");
            qDebug()<<"no found code!";
        }
        else
        {
            if(ok1==true)
            {
                ackReqRegState(account, mac_str, rand_num, phone, 4, "账户名已存在!");
                qDebug()<<"have same account="<<account;
            }
            else if(ok2==true)
            {
                ackReqRegState(account, mac_str, rand_num, phone, 5, "手机号已存在!");
                qDebug()<<"have same phone="<<phone;
            }
        }
    }
}

//解析登录话题
void AccountThread::parseLoginTopic(QJsonObject root_obj)
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
    QString account="";
    if(root_obj.contains("account"))//账户
    {
        QJsonValue value = root_obj.value("account");
        if(value.isString())
        {
            account=value.toString();
        }
    }

    int rand_num=0;
    if(root_obj.contains("rand_num"))//随机数
    {
        QJsonValue value = root_obj.value("rand_num");
        if(value.isDouble())
        {
            rand_num=(int)value.toDouble();
        }
    }
    QString mac_str="";
    if(root_obj.contains("mac"))//mac
    {
        QJsonValue value = root_obj.value("mac");
        if(value.isString())
        {
            mac_str=value.toString();
        }
    }
    QString pass_word="";
    if(root_obj.contains("pass_word"))//密码
    {
        QJsonValue value = root_obj.value("pass_word");
        if(value.isString())
        {
            pass_word=value.toString();
        }
    }
    QString phone="";
    if(root_obj.contains("phone"))//手机号码
    {
        QJsonValue value = root_obj.value("phone");
        if(value.isString())
        {
            phone=value.toString();
        }
    }
    QString ver_code="";
    if(root_obj.contains("ver_code"))//验证码
    {
        QJsonValue value = root_obj.value("ver_code");
        if(value.isString())
        {
            ver_code=value.toString();
        }
    }
    
    AccountSqlite::AccountNodeStruct tag_account;
    if(cmd_type=="login_pwd")//密码登录
    {
        if(!account.isEmpty())
        {                        
            bool ok=account.toDouble();
            if(ok)//手机号登录
            {
                QString phone=account;
                if( m_accountSqlite->selectAccountByPhone(phone, tag_account)==false)
                {
                    ackLoginState("", phone, "", mac_str, rand_num, 0, "", 1, "手机号未注册!");
                    return;
                }
            }
            else
            {
                if( m_accountSqlite->selectAccountByName(account, tag_account)==false)
                {
                    ackLoginState(account, "", "", mac_str, rand_num, 0, "", 2, "帐号未注册!");
                    return;
                }
            }
            if((account==tag_account.account || account==tag_account.phone) && pass_word==tag_account.passWord)//再次校验用户名跟密码
            {
               ackLoginState(tag_account.account,tag_account.phone, tag_account.parentAccount, mac_str, rand_num, tag_account.auth, tag_account.createTime, 0, "登录成功!");
            }
            else
            {
                ackLoginState(tag_account.account,tag_account.phone, tag_account.parentAccount, mac_str, rand_num, tag_account.auth, tag_account.createTime, 3, "密码错误!");
            }
        }
        else
        {
            ackLoginState(account, "", "", mac_str, rand_num, 0, "", 4, "账户不存在!");
        }
    }
    else if(cmd_type=="login_code")//验证码登录
    {
        bool ok; 
        phone.toDouble(&ok);
        if(phone.size()!=11 || !ok)
        {
            qDebug()<<"login phone="<<phone<<" error!";
            ackReqRegState(phone, mac_str, rand_num, phone, 1, "手机号有误!");
            return;
        }
        bool flag=false;
        for(auto iter : m_reqVerCodeList)
        {
            if(iter.phone == phone)
            {
                if(iter.verCode == ver_code)
                {
                    flag=true;
                }
                break;
            }
        }
        if(flag==false)
        {
            ackReqRegState(phone, mac_str, rand_num, phone, 1, "验证码错误!");
            return;
        }
        
        tag_account.phone.clear();
        m_accountSqlite->searchAccountByPhone(phone, tag_account);
        account=tag_account.account;
        if(phone==tag_account.phone)
        {
//            ackReqRegState(phone, mac_str, rand_num, phone, 0, "登录成功!");
            ackLoginState(account, phone, "", mac_str, rand_num, 0x00, "", 0, "登录成功!");
        }
        else//新手机号,直接注册
        {
            account="Y"+phone;
            pass_word.clear();
            for(int i=0; i<8; i++)
            {
                pass_word+=QString::asprintf("%d", drv_com.takeRandNumber()%10);//随机密码
            }
            qDebug()<<account<<" pass_word="<<pass_word;
            bool ok=m_accountSqlite->addAccountNode(account, pass_word, 0x00, "", phone);
            if(ok)
            {
                ackLoginState(account, phone, "", mac_str, rand_num, 0x00, "", 0, "登录(注册)成功!");
                qDebug()<<"reg ok!";
                //自动创建一个应用
                u32 max_app=m_accountSqlite->selectMaxAppID();
                u32 new_app_id=0;
                if(max_app>APP_ID_MIN)
                    new_app_id=max_app+1;
                else  
                    new_app_id=APP_ID_MIN+1;
                
                qDebug()<<"phone new_app_id="<<new_app_id;
               m_accountSqlite->addAppIDToList(new_app_id, account);
            }
            else
            {
                ackReqRegState(phone, mac_str, rand_num, phone, 1, "数据库存储错误!");
                qDebug()<<"reg sql error!!";
            }
        }
    }
    
}

void AccountThread::parsePasswdTopic(QJsonObject root_obj)
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
    QString account="";
    if(root_obj.contains("account"))//账户
    {
        QJsonValue value = root_obj.value("account");
        if(value.isString())
        {
            account=value.toString();
        }
    }
    QString mac_str="";
    if(root_obj.contains("mac"))//mac
    {
        QJsonValue value = root_obj.value("mac");
        if(value.isString())
        {
            mac_str=value.toString();
        }
    }

    int rand_num=0;
    if(root_obj.contains("rand_num"))//rand_num
    {
        QJsonValue value = root_obj.value("rand_num");
        if(value.isDouble())
        {
            rand_num=(int)value.toDouble();
        }
    }
    QString pass_word="";
    if(root_obj.contains("pass_word"))//密码
    {
        QJsonValue value = root_obj.value("pass_word");
        if(value.isString())
        {
            pass_word=value.toString();
        }
    }
    QString phone="";
    if(root_obj.contains("phone"))//手机号码
    {
        QJsonValue value = root_obj.value("phone");
        if(value.isString())
        {
            phone=value.toString();
        }
    }
    QString ver_code="";
    if(root_obj.contains("ver_code"))//验证码
    {
        QJsonValue value = root_obj.value("ver_code");
        if(value.isString())
        {
            ver_code=value.toString();
        }
    }
    
    if(cmd_type=="reset_pwd")
    {
        if(account.isEmpty() || phone.isEmpty())
        {
            qDebug()<<"account.isEmpty() || phone.isEmpty()";
            return;
        }
        AccountSqlite::AccountNodeStruct tag_account;
        m_accountSqlite->selectAccountByPhone(phone, tag_account);
        if(tag_account.account!=account)
        {
            ackResetPasswdState(account, mac_str, rand_num, 4, "账户与手机不匹配!");
            return;
        }
                
        for(auto iter : m_reqVerCodeList)
        {
            if(iter.phone == phone)
            {
                if(iter.verCode == ver_code)
                {
                    tag_account.passWord.clear();
                    m_accountSqlite->updateAccountPassWord(account, pass_word);//更新密码
                    m_accountSqlite->selectAccountByName(account, tag_account);//重新获取密码
                    if(tag_account.passWord==pass_word)//新密码校验
                    {
                        ackResetPasswdState(account, mac_str, rand_num, 0, "密码修改成功!");
                        qDebug("set new pwd ok!");
                    }
                    else
                    {
                        ackResetPasswdState(account, mac_str, rand_num, 1, "密码修改(校验)失败!");
                        qDebug("set new pwd failed!");
                    }
                }
                else
                {
                    ackResetPasswdState(account, mac_str, rand_num, 2, "验证码错误!");
                    qDebug()<<"reg ver_code error!";
                }
                return;
            }
        }
        ackResetPasswdState(account, mac_str, rand_num, 3, "验证码已过期!");
        
    }
    else if(cmd_type=="change_pwd")
    {
        QString new_pwd="";
        if(root_obj.contains("new_pwd"))//
        {
            QJsonValue value = root_obj.value("new_pwd");
            if(value.isString())
            {
                new_pwd=value.toString();
            }
        }
        QString old_pwd="";
        if(root_obj.contains("old_pwd"))//
        {
            QJsonValue value = root_obj.value("old_pwd");
            if(value.isString())
            {
                old_pwd=value.toString();
            }
        }
        if(old_pwd.size()<8 || new_pwd.size()<8)
        {
            ackChangePwdState(account, mac_str, rand_num, 1, "密码长度不足!");
            return;
        }
        AccountSqlite::AccountNodeStruct tag_account;
        tag_account.passWord="";
        m_accountSqlite->selectAccountByName(account, tag_account);
        if(!tag_account.passWord.isEmpty())
        {
            if(tag_account.passWord==old_pwd)//旧密码匹配
            {
                m_accountSqlite->updateAccountPassWord(account, new_pwd);//更新密码
                m_accountSqlite->selectAccountByName(account, tag_account);//重新获取密码
                if(tag_account.passWord==new_pwd)//新密码校验
                {
                    ackChangePwdState(account, mac_str, rand_num, 0, "密码修改成功!");
                    qDebug("set new pwd ok!");
                }
                else
                {
                    ackChangePwdState(account, mac_str, rand_num, 2, "数据库更新失败!");
                    qDebug("set new pwd failed!");
                }
            }
            else
            {
                ackChangePwdState(account, mac_str, rand_num, 3, "旧密码不匹配!");
                qDebug("tag_account.pass_word!=old_pwd");
            }
        }
        else
        {
            ackChangePwdState(account, mac_str, rand_num, 4, "账户不存在!");
            qDebug("tag_account.pass_word.isEmpty()");
        }
    }
    
    
}

void AccountThread::ackReqVerCodeState(QString account, QString mac_str, int rand_num, QString phone, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("cmd_type", "req_vercode");
    root_obj.insert("result", result);
    root_obj.insert("phone", phone);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "code");
    emit sigMqttPushMessage(topic, msg_ba);
}

void AccountThread::ackReqRegState(QString account, QString mac_str, int rand_num, QString phone, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("cmd_type", "req_reg");
    root_obj.insert("result", result);
    root_obj.insert("phone", phone);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "reg");
    emit sigMqttPushMessage(topic, msg_ba);
}

void AccountThread::ackLoginState(QString account, QString phone, QString parent_account, QString mac_str, int rand_num, u32 auth, QString create_time, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    QStringList ver_list;
    ver_list<<"1.2.4"<<"1.3.5";
//    qDebug("ack login state=%d", state);
    root_obj.insert("cmd_type", "login");
    root_obj.insert("ack_str", ack_str);
    root_obj.insert("result", result);
    root_obj.insert("account", account);
    root_obj.insert("phone", phone);
    root_obj.insert("parent_account", parent_account);
    root_obj.insert("auth", (qint64)auth);
    if(ver_list.size()>=2)
    {
        root_obj.insert("android", ver_list[0]);
        root_obj.insert("ios", ver_list[1]);
    }
    root_obj.insert("auth", (qint64)auth);
    root_obj.insert("create_time", create_time);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "login");
    emit sigMqttPushMessage(topic, msg_ba);
}

void AccountThread::ackResetPasswdState(QString account, QString mac_str, int rand_num, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("cmd_type", "reset_pwd");
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "passwd");
    emit sigMqttPushMessage(topic, msg_ba);
}

void AccountThread::ackChangePwdState(QString account, QString mac_str, int rand_num, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("cmd_type", "change_pwd");
    root_obj.insert("account", account);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "passwd");
    emit sigMqttPushMessage(topic, msg_ba);
}







