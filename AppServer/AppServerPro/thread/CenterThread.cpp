#include "CenterThread.h"

CenterThread::CenterThread(QObject *parent) : QObject(parent)
{
    m_accountSqlite=nullptr;
    m_timerCounts=m_secCounts=0;
    
    checkTimer = new QTimer();
    checkTimer->setInterval(500);//心跳检测
    checkTimer->start();
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout()));
    
    
    drv_com.createMutiFile(DB_ROOT_FILE);
}


void CenterThread::slotCheckTimeout(void)
{
    if(m_timerCounts%2==0)
    {
        m_secCounts++;
        
        if(m_accountSqlite==nullptr)
        {
            qDebug()<<"open account sqlite!";
            m_accountSqlite = new AccountSqlite();
            m_accountSqlite->openDataBase(DB_ROOT_FILE+QString("/account_list.db"), QString::asprintf("center_%d", drv_com.takeRandNumber()));
            m_accountSqlite->createAccountListTable();//账户列表
            m_accountSqlite->createAppListTable();//应用列表
        }   
        
    }
    
    
}

//生成发布话题
QString CenterThread::makePubTopic(QString account, QString mac_str, int rand_num, QString key_str)
{
    if(account.isEmpty())
    {
        
    }
    QString topic=QString(TOPIC_HEAD) + SERVER_PUB_TOPIC+QString("/center/")+mac_str+QString::asprintf("/%d/", rand_num)+key_str;
    return topic;
}

QString CenterThread::takeAppFilesPath(u32 app_id)
{
    QString path=DB_ROOT_FILE+QString("/AppFiles")+QString::asprintf("/app%u", app_id);
    return path;
}


void CenterThread::slotCenterThreadMessage(QString topic, QJsonObject root_obj)
{
    if(topic.contains("/app"))//应用相关
    {
        parseAppTopic(root_obj);
    }
    else if(topic.contains("/group"))//分组相关
    {
        parseGroupTopic(root_obj);
    }
    else if(topic.contains("/device"))//设备相关
    {
        parseDeviceTopic(root_obj);
    }
}


void CenterThread::parseAppTopic(QJsonObject root_obj)
{
    QString account="";
    if(root_obj.contains("account"))//账户
    {
        QJsonValue value = root_obj.value("account");
        if(value.isString())
        {
            account=value.toString();
        }
    }
    if(account.isEmpty())
    {
        qDebug()<<"account isEmpty";
        return;
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
    QString cmd_type="";
    if(root_obj.contains("cmd_type"))//命令
    {
        QJsonValue value = root_obj.value("cmd_type");
        if(value.isString())
        {
            cmd_type=value.toString();
        }
    }
    
    if(cmd_type=="new_app")//新建应用
    {
        qDebug()<<"account= "<<account<<" req new app!";
        AccountSqlite::AccountNodeStruct tag_account;
        AccountSqlite::AppNodeStruct tag_app;
        u32 new_app_id=0;
        tag_app.appID=0;
        tag_account.parentAccount="";
        m_accountSqlite->selectAccountByName(account, tag_account);
        qDebug("@@tag_account.auth=0x%08X", tag_account.auth);
        if(tag_account.account.isEmpty()==true)
        {
            qDebug()<<"account"<<account<<" is not found!";
            ackNewAppState(account, mac_str, rand_num, 0, "",1, account+" 未找到账号!");
            return;
        }
        if(tag_account.parentAccount.isEmpty()==false)//检测是否为根账号
        {
           qDebug()<<"account="<<account<<" is not a root count!";
           ackNewAppState(account, mac_str, rand_num, 0, "",1, account+" 该账号不是主账号!");
           return;
        }
        int app_cnts=m_accountSqlite->getAppCountFromAccount(account);//当前的应用数量
        if(app_cnts>=8)//限制每个账号创建应用的数量
        {
            ackNewAppState(account, mac_str, rand_num, 0, "",1, account+" 应用数量已达上限!");
            return;
        }
        u32 max_app=m_accountSqlite->selectMaxAppID();
        if(max_app>APP_ID_MIN)
            new_app_id=max_app+1;
        else  
            new_app_id=APP_ID_MIN+1;
        
        qDebug()<<"new_app_id="<<new_app_id;
        drv_com.createMutiFile(takeAppFilesPath(new_app_id));//创建应用数据库文件夹
       bool ok=m_accountSqlite->addAppIDToList(new_app_id, account);
       if(ok)
       {    
           QString new_name="新应用";        
           if(root_obj.contains("new_name"))//命令
           {
               QJsonValue value = root_obj.value("new_name");
                new_name=value.toString();
           }
           m_accountSqlite->updateAppName(new_app_id, new_name);   //更新数据库内应用名称
           ackNewAppState(account, mac_str, rand_num, new_app_id, new_name, 0, "创建成功!");
           qDebug()<<"111 addAppIDToList ok, new_app_id="<<new_app_id<<", account="<<account;
       }
       else
       {
           ackNewAppState(account, mac_str, rand_num, 0, "",1, "创建失败!");
       }
    }
    else if(cmd_type=="rename")//重命名应用
    {
        u32 app_id=0;
        if(root_obj.contains("app_id"))
        {
            QJsonValue value=root_obj.value("app_id");
            if(value.isDouble())
            {
                app_id=value.toDouble();
            }
        }
        QString app_name="新名称";
        if(root_obj.contains("app_name"))
        {
            QJsonValue value=root_obj.value("app_name");
            if(value.isString())
            {
                app_name=value.toString();
            }
        }
        if(app_id>0)
        {
            m_accountSqlite->updateAppName(app_id, app_name);   
            ackRenameAppState(account, mac_str, rand_num, app_id, app_name, 0, "更新成功!");
        }
        else
        {
            ackRenameAppState(account, mac_str, rand_num, app_id, app_name, 1, "更新失败!");
        }
       
    }
    else if(cmd_type=="app_list")//获取应用列表
    {
        AccountSqlite::AccountNodeStruct tag_account;
        tag_account.account.clear();
        tag_account.appList.clear();
        m_accountSqlite->selectAccountByName(account, tag_account);
        if(tag_account.account.isEmpty()==false)
        {
            ackReqAppListState(account, mac_str, rand_num, tag_account.appList, 0, "应用获取成功!");
        }
    }
}

void CenterThread::parseGroupTopic(QJsonObject root_obj)
{
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
    u32 app_id=0;
    if(root_obj.contains("app_id"))
    {
        QJsonValue value=root_obj.value("app_id");
        if(value.isDouble())
        {
            app_id=value.toDouble();
        }
    }
    QString cmd_type="";
    if(root_obj.contains("cmd_type"))//命令
    {
        QJsonValue value = root_obj.value("cmd_type");
        if(value.isString())
        {
            cmd_type=value.toString();
        }
    }
    
    QString db_path=takeAppFilesPath(app_id);
    drv_com.createMutiFile(db_path);
    QString db_name=db_path+"/"+APP_DB_NAME;
    QJsonArray group_array;
    GroupSqlite tag_groupSqlite;
    bool ok=tag_groupSqlite.openDataBase(db_name, "rand_001");
    int result=0;
    QString ack_str="";
    if(!ok)
        return;
    
    if(cmd_type=="group_list")
    {
        QList<GroupSqlite::GroupNodeStruct>group_list;
        tag_groupSqlite.selectGroupList(group_list);
        for(auto iter : group_list)
        {
            QJsonObject group_obj;
            QJsonArray dev_list;
            for(auto iter2 : iter.devList)
            {
                u32 dev_sn=iter2;
                dev_list.append((qint64)dev_sn);
            }
            group_obj.insert("group_name", iter.groupName);
            group_obj.insert("dev_list", dev_list);
            group_array.append(group_obj);
        }
        ackTakeGroupList(account, mac_str, rand_num, app_id, group_array, result, ack_str);
    }
    else if(cmd_type=="add_group")
    {
        QString group_name;
        if(root_obj.contains("group_name"))
        {
            group_name=root_obj.value("group_name").toString();
        }
        if(group_name.isEmpty())
            return;
        GroupSqlite::GroupNodeStruct tag_group;
        tag_groupSqlite.selectGroupNode(group_name, tag_group);
        if(group_name==tag_group.groupName)
        {
            result=3;//已存在相同名称
            ack_str="名称重复!";
        }
        else  
        {
            ok=tag_groupSqlite.insertGroup(group_name);
            if(ok)
            {
                result=0;//成功
                ack_str="添加成功!";
            }
            else   
            {
                result=4;//写入失败
                ack_str="写入失败!";
            }
        }
        ackAddGroup(account, mac_str, rand_num, app_id, group_name, result, ack_str);
    }
    else if(cmd_type=="del_group")
    {
        QString group_name;
        if(root_obj.contains("group_name"))
        {
            group_name=root_obj.value("group_name").toString();
        }
        GroupSqlite::GroupNodeStruct tag_group;
        tag_group.groupName="";
        tag_groupSqlite.selectGroupNode(group_name, tag_group);
        if(group_name!=tag_group.groupName)
        {
            result=3;//分组不存在
            ack_str="分组不存在!";
        }
        else  
        {
            tag_groupSqlite.delGroupNode(group_name);
            tag_group.groupName="";
            tag_groupSqlite.selectGroupNode(group_name, tag_group);
            if(group_name!=tag_group.groupName)
            {
                result=0;//成功
                ack_str="删除成功!";
            }
            else   
            {
                result=4;//删除失败
                ack_str="删除失败!";
            }
        }
        ackDelGroup(account, mac_str, rand_num, app_id, group_name, result, ack_str);
    }
    else if(cmd_type=="order_group")
    {
        QJsonArray group_array;
        if(root_obj.contains("group_list"))
        {
            group_array=root_obj.value("group_list").toArray();
        }
        int nSize=group_array.size();
        for(int i=0; i<nSize; i++)
        {
            QString group_name=group_array.at(i).toString();
            tag_groupSqlite.updateGroupOrder(group_name, i);
        }
        result=0;//成功
        ack_str="排序成功!";
//        ackOrderGroup(account, mac_str, rand_num, app_id, group_array, result, ack_str);
        if(1)
        {
            QList<GroupSqlite::GroupNodeStruct>group_list;
            QJsonArray group_array;
            tag_groupSqlite.selectGroupList(group_list);
            for(auto iter : group_list)
            {
                QJsonObject group_obj;
                QJsonArray dev_list;
                for(auto iter2 : iter.devList)
                {
                    u32 dev_sn=iter2;
                    dev_list.append((qint64)dev_sn);
                }
                group_obj.insert("group_name", iter.groupName);
                group_obj.insert("dev_list", dev_list);
                group_array.append(group_obj);
            }
            ackTakeGroupList(account, mac_str, rand_num, app_id, group_array, result, ack_str);
        }
    }
    else if(cmd_type=="rename_group")
    {
        QString old_name;
        if(root_obj.contains("old_name"))
        {
            old_name=root_obj.value("old_name").toString();
        }
        QString new_name;
        if(root_obj.contains("new_name"))
        {
            new_name=root_obj.value("new_name").toString();
        }
        
        GroupSqlite::GroupNodeStruct tag_group;
        tag_group.groupName="";
        tag_groupSqlite.selectGroupNode(old_name, tag_group);
        if(old_name==tag_group.groupName)
        {
            tag_group.groupName="";
            tag_groupSqlite.selectGroupNode(new_name, tag_group);
            if(new_name==tag_group.groupName)
            {
                result=4;//新名称已存在
                ack_str="新名称已存在!";
            }
            else  
            {
                tag_groupSqlite.updateGroupName(old_name, new_name);
                tag_group.groupName="";
                tag_groupSqlite.selectGroupNode(new_name, tag_group);
                if(new_name==tag_group.groupName)
                {
                    result=0;
                    ack_str="更新成功!";
                }
                else  
                {
                    result=5;//写入失败
                    ack_str="写入失败!";
                }
            }
        }
        else  
        {
            result=3;//旧名称不存在
            ack_str="旧名称不存在!";
        }
         ackUpdateGroupName(account, mac_str, rand_num, app_id, old_name, new_name, result,ack_str);
    }
    tag_groupSqlite.closeDataBase();
}

void CenterThread::parseDeviceTopic(QJsonObject root_obj)
{
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
    u32 app_id=0;
    if(root_obj.contains("app_id"))
    {
        QJsonValue value=root_obj.value("app_id");
        if(value.isDouble())
        {
            app_id=value.toDouble();
        }
    }
    QString cmd_type="";
    if(root_obj.contains("cmd_type"))//命令
    {
        QJsonValue value = root_obj.value("cmd_type");
        if(value.isString())
        {
            cmd_type=value.toString();
        }
    }
    
    QString db_path=takeAppFilesPath(app_id);
    drv_com.createMutiFile(db_path);
    QString db_name=db_path+"/"+APP_DB_NAME;
    GroupSqlite tag_groupSqlite;
    bool ok=tag_groupSqlite.openDataBase(db_name, "rand_001");
    int result=0;
    QString ack_str="";
    if(!ok)
        return;
    
    if(cmd_type=="sort_dev")
    {
        QString group_name="";
        if(root_obj.contains("group_name"))
        {
            QJsonValue value = root_obj.value("group_name");
            group_name=value.toString();
        }
        QJsonArray dev_array;
        QList<u32>dev_list;
        if(root_obj.contains("dev_list"))
        {
            QJsonValue value = root_obj.value("dev_list");
            dev_array=value.toArray();
        }
        int nSize=dev_array.size();
        for(int i=0; i<nSize; i++)
        {
            u32 dev_sn=dev_array.at(i).toDouble();
            dev_list.append((qint64)dev_sn);
        }
        
        tag_groupSqlite.updateDevList(group_name, dev_list);//更新数据库
        ack_str="排序成功!";
        ackSortDevice(account, mac_str, rand_num, app_id, group_name, dev_array, result, ack_str);
    }
    else if(cmd_type=="dev_list")//获取某个应用下的所有设备
    {
        QList<GroupSqlite::DeviceNodeStruct>work_list;
        tag_groupSqlite.selectDeviceList(work_list);
        ackDevList(account, mac_str, rand_num, app_id, work_list, result, ack_str);
    }
    else if(cmd_type=="add_dev")
    {
        u32 dev_sn=0, parent_sn=0;
        if(root_obj.contains("dev_sn"))
        {
            QJsonValue value = root_obj.value("dev_sn");
            dev_sn=value.toDouble();
        }
        if(root_obj.contains("parent_sn"))
        {
            QJsonValue value = root_obj.value("parent_sn");
            parent_sn=value.toDouble();
        }
        GroupSqlite::DeviceNodeStruct dev_node;
        tag_groupSqlite.selectDeviceNode(dev_sn, dev_node);
        if(dev_node.devSn==dev_sn)
        {
            ack_str=QString::asprintf("设备%08X重复 添加!", dev_sn);
            result=1;
        }
        else
        {
            tag_groupSqlite.insertDeviceNode(dev_sn, parent_sn);//添加设备到数据库
            ack_str=QString::asprintf("设备%08X 添加成功!", dev_sn);        
        }
        ackAddDevice(account, mac_str, rand_num, app_id, dev_sn, parent_sn, result, ack_str);
    }
    else if(cmd_type=="del_dev")
    {
        QJsonArray dev_array;
        if(root_obj.contains("dev_list"))
        {
            QJsonValue value = root_obj.value("dev_list");
            dev_array=value.toArray();
        }
        int nSize=dev_array.size();
        for(int i=0; i<nSize; i++)
        {
            u32 dev_sn=dev_array.at(i).toDouble();
            tag_groupSqlite.delDeviceNode(dev_sn);//删除设备
        }
        ack_str="删除成功!";
        ackDelDevice(account, mac_str, rand_num, app_id, dev_array, result, ack_str);
    }
    else if(cmd_type=="rename_dev")
    {
        u32 dev_sn=0;
        if(root_obj.contains("dev_sn"))
        {
            QJsonValue value = root_obj.value("dev_sn");
            dev_sn=value.toDouble();
        }
        QString new_name;
        if(root_obj.contains("new_name"))
        {
            QJsonValue value = root_obj.value("new_name");
            new_name=value.toString();
        }
        if(dev_sn>0 &&!new_name.isEmpty())
        {
            tag_groupSqlite.updateDeviceName(dev_sn, new_name);
            ack_str="重命名成功!";
        }
        else
        {
            ack_str="重命名失败!";
            result=1;
        }
        ackRenameDevice(account, mac_str, rand_num, app_id, dev_sn, new_name, result, ack_str);
    }
    else if(cmd_type=="update_parent")
    {
        u32 dev_sn=0, parent_sn=0;
        if(root_obj.contains("dev_sn"))
        {
            QJsonValue value = root_obj.value("dev_sn");
            dev_sn=value.toDouble();
        }
        if(root_obj.contains("parent_sn"))
        {
            QJsonValue value = root_obj.value("parent_sn");
            parent_sn=value.toDouble();
        }
        tag_groupSqlite.updateDeviceParentSn(dev_sn, parent_sn);
        ackUpdateParentSn(account, mac_str, rand_num, app_id, dev_sn, parent_sn, result, ack_str);
    }
    tag_groupSqlite.closeDataBase();//关闭数据库
}




void CenterThread::ackNewAppState(QString account, QString mac_str, int rand_num, u32 app_id, QString app_name, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("cmd_type", "new_app");
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("app_name", app_name);
    root_obj.insert("create_time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "app");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackRenameAppState(QString account, QString mac_str, int rand_num, u32 app_id, QString app_name, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("cmd_type", "rename");
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("app_name", app_name);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "app");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackReqAppListState(QString account, QString mac_str, int rand_num, QList<u32> app_list, int result, QString ack_str)
{
    QJsonArray app_array;
    QJsonObject root_obj;
    QJsonDocument json_doc;

    for(int i=0; i<app_list.size(); i++)
    {
        u32 app_id=app_list.at(i);
        if(app_id>0)
        {
            AccountSqlite::AppNodeStruct tag_app_node;
            m_accountSqlite->selectAppInfo(app_id, tag_app_node);
            QJsonObject app_obj;
            app_obj.insert("app_id", (qint64)app_id);
            app_obj.insert("app_name", tag_app_node.appName);
            app_obj.insert("create_time", tag_app_node.createTime);
            app_array.append(app_obj);
        }
    }
    root_obj.insert("cmd_type", "app_list");
    root_obj.insert("account", account);
    root_obj.insert("app_list", app_array);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "app");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackTakeGroupList(QString account, QString mac_str, int rand_num, u32 app_id, QJsonArray group_array, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "group_list");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("group_list", group_array);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackAddGroup(QString account, QString mac_str, int rand_num, u32 app_id, QString group_name, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "add_group");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("group_name", group_name);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackUpdateGroupName(QString account, QString mac_str, int rand_num, u32 app_id, QString old_name, QString new_name, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "rename_group");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("old_name", old_name);
    root_obj.insert("new_name", new_name);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackDelGroup(QString account, QString mac_str, int rand_num, u32 app_id, QString group_name, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "del_group");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("group_name", group_name);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackOrderGroup(QString account, QString mac_str, int rand_num, u32 app_id, QJsonArray group_array, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "order_group");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("group_list", group_array);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackDevList(QString account, QString mac_str, int rand_num, u32 app_id, QList<GroupSqlite::DeviceNodeStruct> work_list, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "dev_list");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    QJsonArray dev_array;
    for(auto iter : work_list)
    {
        QJsonObject dev_node;
        dev_node.insert("dev_sn", (qint64)iter.devSn);
        dev_node.insert("parent_sn", (qint64)iter.parentSn);
        dev_node.insert("dev_name", iter.devName);
        dev_node.insert("create_time", iter.createTime);
        dev_array.append(dev_node);
    }
    root_obj.insert("dev_list", dev_array);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "device");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackSortDevice(QString account, QString mac_str, int rand_num, u32 app_id, QString group_name, QJsonArray dev_array, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "sort_dev");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("group_name", group_name);
    root_obj.insert("dev_list", dev_array);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "device");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackAddDevice(QString account, QString mac_str, int rand_num, u32 app_id, u32 dev_sn, u32 parent_sn, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "add_dev");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("dev_sn", (qint64)dev_sn);
    root_obj.insert("parent_sn", (qint64)parent_sn);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "device");
    emit sigMqttPushMessage(topic, msg_ba);
}


void CenterThread::ackDelDevice(QString account, QString mac_str, int rand_num, u32 app_id, QJsonArray dev_array, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "del_dev");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("dev_list", dev_array);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "device");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterThread::ackRenameDevice(QString account, QString mac_str, int rand_num, u32 app_id, u32 dev_sn, QString new_name, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "rename_dev");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("dev_sn", (qint64)dev_sn);
    root_obj.insert("new_name", new_name);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "device");
    emit sigMqttPushMessage(topic, msg_ba);
}


void CenterThread::ackUpdateParentSn(QString account, QString mac_str, int rand_num, u32 app_id, u32 dev_sn, u32 parent_sn, int result, QString ack_str)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "update_parent");
    root_obj.insert("account", account);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("dev_sn", (qint64)dev_sn);
    root_obj.insert("parent_sn", (qint64)parent_sn);
    root_obj.insert("result", result);
    root_obj.insert("ack_str", ack_str);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic(account, mac_str, rand_num, "device");
    emit sigMqttPushMessage(topic, msg_ba);
}



