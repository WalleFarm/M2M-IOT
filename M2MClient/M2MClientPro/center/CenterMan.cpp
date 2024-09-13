#include "CenterMan.h"

//控制中心

CenterMan::CenterMan(QObject *parent) : QObject(parent)
{
    m_rootPath=CFG_ROOT_FILE;
    #if defined(Q_OS_IOS)
    m_rootPath=QDir::homePath()+QString("/Documents/")+CFG_ROOT_FILE;
    #endif
    
    m_currModelDevSn=0;
    m_modelParent=nullptr;
    m_secCounts=0;
    checkTimer = new QTimer(this);
    checkTimer->setInterval(1000);//心跳检测
    checkTimer->start();
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout()));
    
    readConfig();
    
    initDevTypeList();//初始化支持的设备类型
    
//    for(int i=0; i<5; i++)
//    {
//        QString str;
//        for(int k=0; k<16; k++)
//        {
//            str+=QString::asprintf("0x%02X, ", drv_com.takeRandNumber()&0xFF);
//        }
//        qDebug()<<str;
//    }
    

}

void CenterMan::slotCheckTimeout(void) 
{
    m_secCounts++;
//    if(m_secCounts>10 && m_secCounts<20)
//    {
//        requestAddDevice(0xA1010000+m_secCounts-10, 0);
//    }
    
    if(!m_loginAccount.isEmpty() && m_appWorkList.size()<=0)
    {
        requestAppList();
    }
    
    if(m_secCounts%3==0)
    {
        updateGroupOnlineText();
    }
}

void CenterMan::setMacInfo(QString mac, int rand_num)
{
    m_macStr=mac;
    m_randNum=rand_num;

}

QString CenterMan::makePubTopic(QString key_str)
{
    QString topic=QString(TOPIC_HEAD)+"as/sub/center/"+key_str;
    return  topic;
}

void CenterMan::slotUpdateLoginAccount(QString account, int state)
{
    qDebug()<<"CenterMan account="<<account<<", state="<<state;
    if(state>0)
    {
        m_loginAccount=account;  //登录成功
        requestAppList();//请求应用列表
    }
    else
    {
        m_loginAccount.clear();
    }
}

void CenterMan::slotMqttConnected(void)
{
    
}

void CenterMan::slotSendDownMsg(u32 app_id, u32 dev_sn, u8 pack_num, u8 msg_type, u8 *msg_buff, u16 msg_len)
{
    //    qDebug()<<"slotSendDownMsg****";
        u8 data_buff[1500]={0}, make_buff[1500]={0};
        u16 make_len, data_len;
        data_len=msg_len;
        if(msg_len>sizeof(data_buff))
        {
            qDebug()<<"slotSendDownMsg error, msg_len>sizeof(data_buff)";
            return;
        }
        memcpy(data_buff, msg_buff, data_len);
        u32 currDevSn=dev_sn;
        if(app_id==m_currAppWork.appID)//自身设备
        {
            int deep=0;
            while(currDevSn>0)
            {
                WorkDevStruct *pWorkNode=searchWorkNode(currDevSn);
                if(pWorkNode==nullptr)
                {
                    qDebug()<<"pWorkNode==nullptr";
                    return;
                }
                if(deep>0)
                {
                    msg_type=200;//转发指令为200
                }
                qDebug("down curr_sn=0x%08X, parent_sn=0x%08X", pWorkNode->devSn, pWorkNode->parentSn);
                if(pWorkNode->parentSn==0)//网关设备
                {   
                    make_len=makeGwSendBuff(app_id, currDevSn, pack_num, pWorkNode->encrypt_index,
                                                                msg_type, data_buff, data_len, make_buff, sizeof(make_buff));
                    publishDownMsg(app_id, currDevSn, make_buff, make_len);//发布
                    currDevSn=0;
                }
                else//节点设备
                {
                    make_len=makeNodeSendBuff(currDevSn, pack_num, msg_type, data_buff, data_len, make_buff, sizeof(make_buff));//组合节点数据
                    if(make_len>0 && make_len<sizeof(data_buff))
                    {
                        data_len=make_len;
                        memcpy(data_buff, make_buff, make_len);
                        memset(make_buff, 0, sizeof(make_buff));
                    }
                    else
                    {
                        qDebug("error01, make_len=%u", make_len);
                        return;
                    }
                    currDevSn=pWorkNode->parentSn;//父节点序列号
                }
                deep++;
            }
        }
}

void CenterMan::readConfig(void)
{
    m_currAppWork.appID=0;
    QString path=m_rootPath+"/app.txt";
    QJsonObject root_obj=drv_com.readConfg(path);
    if(root_obj.contains("curr_app"))
    {
        m_currAppWork.appID=root_obj.value("curr_app").toDouble();
    }
    
}

void CenterMan::writeConfig(void)
{
    QString path=m_rootPath+"/app.txt";
    QJsonObject root_obj;
    root_obj.insert("curr_app", (qint64)m_currAppWork.appID);
    drv_com.writeConfig(path, root_obj);
}

u32 CenterMan::takeCurrAppID(void)
{
    return m_currAppWork.appID;
}

void CenterMan::requestNewApp(QString new_name)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("cmd_type", "new_app");
    root_obj.insert("account", m_loginAccount);
    root_obj.insert("new_name", new_name);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("app");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterMan::requestRenameApp(qint64 app_id, QString new_name)
{
    if(app_id==0)
        return;
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("account", m_loginAccount);
    root_obj.insert("cmd_type", "rename");
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr);
    root_obj.insert("app_id", (qint64)app_id);
    root_obj.insert("app_name", new_name);
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("app");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterMan::requestAppList(void)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("cmd_type", "app_list");
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("app");
    emit sigMqttPushMessage(topic, msg_ba);
//    qDebug("requestAppList***");
}

void CenterMan::requestGroupList(void)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("cmd_type", "group_list");
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterMan::requestAddGroup(QString group_name)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("cmd_type", "add_group");
    root_obj.insert("group_name", group_name); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterMan::requestRenameGroup(QString old_name, QString new_name)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("cmd_type", "rename_group");
    root_obj.insert("old_name", old_name); 
    root_obj.insert("new_name", new_name); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterMan::requestDelGroup(QString group_name)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;

    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("cmd_type", "del_group");
    root_obj.insert("group_name", group_name); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterMan::requestOrderGroup(QStringList group_list)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;    

    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("cmd_type", "order_group");
    QJsonArray group_array;
    qDebug()<<"group_list="<<group_list;
    for(auto iter : group_list)
    {
        group_array.append(iter);
    }
    root_obj.insert("group_list", group_array); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("group");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterMan::requestDevList(void)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("cmd_type", "dev_list");
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("device");
    emit sigMqttPushMessage(topic, msg_ba);
}

//添加从机设备
void CenterMan::requestAddDevice(u32 dev_sn, u32 parent_sn)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("cmd_type", "add_dev");
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("dev_sn", (qint64)dev_sn); 
    root_obj.insert("parent_sn", (qint64)parent_sn); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("device");
    emit sigMqttPushMessage(topic, msg_ba);
}

//注册设备,发送到设备端
//设备端需要通过APP在线删除操作或按钮等方式，将APP_ID重置为默认值----APP_ID_MIN(123000)
void CenterMan::requestRegDevice(u32 dev_sn)
{
    u8 make_buff[200]={0}, cmd_buff[50]={0};
    u16 make_len=0, cmd_len=0;
    
    u32 set_app_id=m_currAppWork.appID;
    cmd_buff[cmd_len++]=set_app_id>>24;
    cmd_buff[cmd_len++]=set_app_id>>16;
    cmd_buff[cmd_len++]=set_app_id>>8;
    cmd_buff[cmd_len++]=set_app_id;
    
    make_len=makeGwSendBuff(APP_ID_MIN, dev_sn, drv_com.takeRandNumber()%100+10, 0, BaseModel::REG_CMD_SET_APP_ID, cmd_buff, cmd_len, make_buff, sizeof(make_buff));
    publishDownMsg(APP_ID_MIN, dev_sn, make_buff, make_len);//发布
}

//添加网关设备
QString CenterMan::requestAddDeviceQml(QString dev_sn_str)
{
    QString ack_str;
    bool ok;
    u32 dev_sn=dev_sn_str.toUInt(&ok, 16);
    qDebug()<<"dev_sn_str="<<dev_sn_str;
    if(ok && dev_sn>0)
    {
        InitTypeStruct *pInitType=takeInitType(dev_sn);
        if(pInitType==nullptr)
        {
            ack_str="不支持的设备类型,\n请先升级软件!";
            return ack_str;
        }
        
        requestAddDevice(dev_sn, 0);//添加到服务器
        
        //添加到设备端
        requestRegDevice(dev_sn);//发送到设备端,要保证设备已经重置并且在线,否则此次操作无效
    }
    else
    {
        ack_str="格式有误!";
    }
    
    return ack_str;
}

void CenterMan::requestSortDevice(QString group_name, QList<qint64> dev_list)
{
    QJsonArray dev_array;
    QJsonObject root_obj;
    QJsonDocument json_doc;
    qDebug()<<"group_name="<<group_name;
    qDebug()<<"dev_list="<<dev_list;

    int nSize=dev_list.size();
    for(int i=0; i<nSize; i++)
    {
        u32 dev_sn=dev_list.at(i);
        if(dev_sn>0)
        {
            dev_array.append((qint64)dev_sn);
        }
    }

    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("group_name", group_name); 
    root_obj.insert("dev_list", dev_array); 
    root_obj.insert("cmd_type", "sort_dev");
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("device");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterMan::requestDelDevice(QList<qint64> dev_list)
{
    QJsonArray dev_array;
    QJsonObject root_obj;
    QJsonDocument json_doc;
    qDebug()<<"del dev_list="<<dev_list;

    int nSize=dev_list.size();
    for(int i=0; i<nSize; i++)
    {
        u32 dev_sn=dev_list.at(i);
        if(dev_sn>0)
        {
            dev_array.append((qint64)dev_sn);
            
            if(1)//复位设备的APP_ID
            {
                u8 make_buff[200]={0}, cmd_buff[50]={0};
                u16 make_len=0, cmd_len=0;
                
                u32 set_app_id=APP_ID_MIN;
                cmd_buff[cmd_len++]=set_app_id>>24;
                cmd_buff[cmd_len++]=set_app_id>>16;
                cmd_buff[cmd_len++]=set_app_id>>8;
                cmd_buff[cmd_len++]=set_app_id;
                
                make_len=makeGwSendBuff(m_currAppWork.appID, dev_sn, drv_com.takeRandNumber()%100+10, 0, BaseModel::REG_CMD_SET_APP_ID, cmd_buff, cmd_len, make_buff, sizeof(make_buff));
                publishDownMsg(m_currAppWork.appID, dev_sn, make_buff, make_len);//发布
            }
        }
    }

    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("dev_list", dev_array); 
    root_obj.insert("cmd_type", "del_dev");
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("device");
    emit sigMqttPushMessage(topic, msg_ba);
    
}

void CenterMan::requestRenameDevice(u32 dev_sn, QString new_name)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("cmd_type", "rename_dev");
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("dev_sn", (qint64)dev_sn); 
    root_obj.insert("new_name", new_name); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("device");
    emit sigMqttPushMessage(topic, msg_ba);
}

void CenterMan::requestUpdateParentSn(u32 dev_sn, u32 parent_sn)
{
    QJsonObject root_obj;
    QJsonDocument json_doc;
    root_obj.insert("account", m_loginAccount);
    root_obj.insert("rand_num", m_randNum);
    root_obj.insert("mac", m_macStr); 
    root_obj.insert("cmd_type", "update_parent");
    root_obj.insert("app_id", (qint64)m_currAppWork.appID); 
    root_obj.insert("dev_sn", (qint64)dev_sn); 
    root_obj.insert("parent_sn", (qint64)parent_sn); 
    json_doc.setObject(root_obj);
    QByteArray msg_ba = json_doc.toJson(QJsonDocument::Indented);
    QString topic=makePubTopic("device");
    emit sigMqttPushMessage(topic, msg_ba);
}

//移动设备
void CenterMan::requestMoveDevice(QString src_group, QString dst_group, QList<qint64> dev_list)
{
    GroupNodeStruct *pGroupNodeSrc=searchGroupNode(src_group);
    GroupNodeStruct *pGroupNodeDst=searchGroupNode(dst_group);
    int nSize=dev_list.size();
    if(pGroupNodeSrc==nullptr || pGroupNodeDst==nullptr || nSize<=0)
        return;
//    qDebug()<<"move src_group="<<src_group;
//    qDebug()<<"move dst_group="<<dst_group;
//    qDebug()<<"move dev_list="<<dev_list;
//    qDebug()<<"src dev list="<<pGroupNodeSrc->devList;
//    qDebug()<<"dst dev list="<<pGroupNodeDst->devList;
    if(src_group!="全部")
    {
        for(auto iter : dev_list)//先移除源分组设备
        {
//            qDebug()<<"will remove dev sn="<<iter<<", dev_list="<<pGroupNodeSrc->devList;
            for(int i=0; i<pGroupNodeSrc->devList.size(); i++)
            {
                if(iter==pGroupNodeSrc->devList.at(i))
                {
//                    qDebug()<<"remove dev_sn="<<iter;
                    pGroupNodeSrc->devList.removeAt(i);
                    break;
                }
            }
        }
        requestSortDevice(src_group, pGroupNodeSrc->devList);//重新排序
    }
    if(dst_group!="全部")
    {
        for(auto iter : dev_list)//添加到目标分组
        {
            bool flag=false;
            for(int i=0; i<pGroupNodeDst->devList.size(); i++)
            {
                if(iter==pGroupNodeDst->devList.at(i))//重复,不能添加
                {
                    flag=true;
                    break;
                }
            }
            if(flag==false)
            {
                pGroupNodeDst->devList.append(iter);
            }
        }
        requestSortDevice(dst_group, pGroupNodeDst->devList);//重新排序
    }
}


u8 CenterMan::takeDevicePasswd(u32 dev_sn, u8 index, u8 *passwd_buff)
{
    QByteArray passwd_ba;
    u16 dev_type=dev_sn>>16;
    u8 encrypt_mdoe=ENCRYPT_MODE_DISABLE;
    switch(dev_type)
    {
        case MODEL_TYPE_AP01:
        {
            passwd_ba=ModelAp01::takeModelPassword(index);
            encrypt_mdoe=ENCRYPT_MODE_TEA;
            memcpy(passwd_buff, passwd_ba.data(), 16);
            break;
        }
        case MODEL_TYPE_TH01:
        {
            passwd_ba=ModelTh01::takeModelPassword(index);
            encrypt_mdoe=ENCRYPT_MODE_TEA;
            memcpy(passwd_buff, passwd_ba.data(), 16);
            break;
        }
    }
    return encrypt_mdoe;
}

void CenterMan::parseCenterRecv(QString topic, QJsonObject root_obj)
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
    
     if(topic.contains("/app"))//应用相关
    {
         if(result!=0)
             return;
         if(cmd_type=="new_app")//返回新应用
         {
             u32 app_id=0;
             if(root_obj.contains("app_id"))
             {
                 QJsonValue value = root_obj.value("app_id");
                 if(value.isDouble())
                 {
                     app_id=value.toDouble();
                 }
             }
             if(app_id>0)
             {
                 QString app_name="新应用";
                 if(root_obj.contains("app_name"))
                 {
                     app_name=root_obj.value("app_name").toString();
                 }
                 AppWorkStruct tag_work_app;
                 tag_work_app.appID=app_id;
                 tag_work_app.appName=app_name;
                 m_appWorkList.append(tag_work_app);
             }
         }
         else if(cmd_type=="rename")//返回重命名
         {
             u32 app_id=0;
             if(root_obj.contains("app_id"))
             {
                 QJsonValue value = root_obj.value("app_id");
                 if(value.isDouble())
                 {
                     app_id=value.toDouble();
                 }
             }
             QString app_name="";
             if(root_obj.contains("app_name"))
             {
                 QJsonValue value = root_obj.value("app_name");
                 if(value.isString())
                 {
                     app_name=value.toString();
                 }
             }
             if(app_id>0)
             {
                 int i=0;
                 for(auto iter : m_appWorkList)
                 {
                     if(iter.appID==app_id)
                     {
                         m_appWorkList[i].appName=app_name;
                     }
                     i++;
                 }
                 emit siqUpdateCurrAppName(app_id, app_name);
             }
         }
         else if(cmd_type=="app_list")//返回应用列表
         {
             if(root_obj.contains("app_list"))
             {
                 QJsonValue value=root_obj.value("app_list");
                 
                 if(value.isArray())
                 {
                     m_appWorkList.clear();//清除APP列表
                     bool curr_app_flag=false;
                     QJsonArray app_array=value.toArray();
                     qDebug()<<app_array;
                     int nSize=app_array.size();
                     for(int i=0; i<nSize; i++)
                     {
                         QJsonValue value=app_array.at(i);
                         if(value.isObject())
                         {
                             AppWorkStruct tag_app_work;
                             tag_app_work.appID=0;
                             QJsonObject app_obj=value.toObject();

                             if(app_obj.contains("app_name"))
                             {
                                 QJsonValue value=app_obj.value("app_name");
                                 tag_app_work.appName=value.toString();
                             }
                             if(app_obj.contains("app_id"))
                             {
                                 QJsonValue value=app_obj.value("app_id");
                                 if(value.isDouble())
                                 {
                                     u32 app_id=value.toDouble();
                                     if(app_id>0)
                                     {
                                         tag_app_work.appID=app_id;
                                         if(m_currAppWork.appID==app_id)
                                         {
                                             m_currAppWork.appName=tag_app_work.appName;
                                             curr_app_flag=true;
                                         }
                                     }
                                 }
                             }
                             if(app_obj.contains("create_time"))
                             {
                                 QJsonValue value=app_obj.value("create_time");
                                 tag_app_work.createTime=value.toString();
                             }
                             if(tag_app_work.appID>0)
                             {
                                 m_appWorkList.append(tag_app_work);   
                             }
                         }                    
                     }
                     if(curr_app_flag==false && m_appWorkList.size()>0)//没有默认应用
                     {
                         m_currAppWork=m_appWorkList.first();
                     }
                     
                     emit siqUpdateCurrAppName(m_currAppWork.appID, m_currAppWork.appName);
                     writeConfig();
                     requestDevList();//请求设备列表
                 }
             }     
         }
    }
    else if(topic.contains("/group"))//分组相关
    {
        if(cmd_type=="group_list")
        {
            QJsonArray group_array;
            QJsonValue value=root_obj.value("group_list");
            
            clearGroup();//清除分组
            if(value.isArray())
            {
                group_array=value.toArray();
                int nSize=group_array.size();
                bool group_flag=false;
                for(int i=0; i<nSize; i++)
                {
                    QJsonValue value=group_array.at(i);
                    if(value.isObject())
                    {
                        GroupNodeStruct group_node;
                        QJsonObject group_obj=value.toObject();
                        if(group_obj.contains("group_name"))
                        {
                            QJsonValue value=group_obj.value("group_name");
                            if(value.isString())
                            {
                                group_node.groupName=value.toString();
                                if(group_node.groupName=="全部")
                                {
                                    group_flag=true;
                                }
                            }
                        }
                        if(!group_node.groupName.isEmpty())
                        {
                            emit siqAddGroup(group_node.groupName);
                        }
                        if(group_obj.contains("dev_list"))//分组内的设备
                        {
                            QJsonValue value=group_obj.value("dev_list");
                            if(value.isArray())
                            {
                                QJsonArray dev_array=value.toArray();
                                int nSize=dev_array.size();
                                for(int i=0; i<nSize; i++)
                                {
                                    QJsonValue value=dev_array.at(i);
                                    if(value.isDouble())
                                    {
                                        u32 dev_sn=value.toDouble();
                                        if(dev_sn>0)
                                        {
                                            group_node.devList.append(dev_sn);
//                                            qDebug()<<group_node.groupName<<", "<<dev_sn;
                                            emit siqAddDevice2Group(group_node.groupName, dev_sn);//添加到该分组
                                        }
                                    }
                                }
                            }
                        }
                        if(!group_node.groupName.isEmpty())
                        {
                            emit siqUpdateGroupTotalNum(group_node.groupName, group_node.devList.size());
                            m_groupList.append(group_node);
                            qDebug()<<group_node.groupName<<" dev list="<<group_node.devList;
                        }
                    }
                }
                if(group_flag==false)
                {
                    requestAddGroup("全部");
                }

            }
        }
        else if(cmd_type=="add_group")
        {
            QString group_name="";
            if(root_obj.contains("group_name"))
            {
                QJsonValue value = root_obj.value("group_name");
                group_name=value.toString();
            }
            if(group_name.isEmpty())
                return;
            if(result==0)
            {
                emit siqAddGroup(group_name, 0);           
                GroupNodeStruct group_node;
                group_node.groupName=group_name;
                m_groupList.append(group_node);
            }
        }
        else if(cmd_type=="rename_group")
        {
            if(result==0)
            {
                QString old_name, new_name;
                if(root_obj.contains("old_name"))
                {
                    QJsonValue value = root_obj.value("old_name");
                    old_name=value.toString();
                }
                if(root_obj.contains("new_name"))
                {
                    QJsonValue value = root_obj.value("new_name");
                    new_name=value.toString();
                }
                emit siqRenameGroup(old_name, new_name);
                GroupNodeStruct *pGroupNode=searchGroupNode(old_name);
                if(pGroupNode)
                    pGroupNode->groupName=new_name;
            }
        }
        else if(cmd_type=="del_group")
        {
            QString group_name="";
            if(root_obj.contains("group_name"))
            {
                QJsonValue value = root_obj.value("group_name");
                group_name=value.toString();
            }

            if(result==0)
            {
                emit siqDelGroup(group_name);
                int nSize=m_groupList.size();
                for(int i=0; i<nSize; i++)
                {
                    if(m_groupList.at(i).groupName==group_name)
                    {
                        m_groupList.removeAt(i);
                        break;
                    }
                }
            }
        }
        else if(cmd_type=="order_group")
        {
            
        }
    }
    else if(topic.contains("/device"))//设备相关
    {
        if(cmd_type=="dev_list")//设备列表
        {
            clearWorkList();//清除原来的设备
            QJsonArray dev_array;
            if(root_obj.contains("dev_list"))
            {
                QJsonValue value = root_obj.value("dev_list");
                dev_array=value.toArray();
            }
            int nSize=dev_array.size();
            for(int i=0; i<nSize; i++)
            {
                QJsonObject dev_node=dev_array.at(i).toObject();
                u32 dev_sn=dev_node.value("dev_sn").toDouble();
                u32 parent_sn=dev_node.value("parent_sn").toDouble();
                QString dev_name=dev_node.value("dev_name").toString();
                addWorkDevice(dev_sn, parent_sn, dev_name);//添加到工作设备列表
            }
            requestGroupList();//请求分组信息
        }
        else if(cmd_type=="add_dev")//新增设备
        {
            u32 dev_sn=root_obj.value("dev_sn").toDouble();
            u32 parent_sn=root_obj.value("parent_sn").toDouble();
            if(result==0)
            {
                addWorkDevice(dev_sn, parent_sn, "");//添加到工作列表
                emit siqAddDevice2Group("全部", dev_sn);//添加到全部分组
                
                GroupNodeStruct *pGroupNode=searchGroupNode("全部");
                if(pGroupNode)
                {
                    pGroupNode->devList.append(dev_sn);
                    requestSortDevice("全部", pGroupNode->devList);//重新排序
                }
                
            }

        }
        else if(cmd_type=="del_dev")//移除设备
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
                delWorkDevice(dev_sn);//删除工作设备
                emit siqDelDevice(dev_sn, i==nSize-1);//删除显示设备
            }
        }
        else if(cmd_type=="rename_dev")//重命名设备
        {
            u32 dev_sn=root_obj.value("dev_sn").toDouble();
            QString new_name=root_obj.value("new_name").toString();
            updateWorkName(dev_sn, new_name);//更新设备名称
        }
        else if(cmd_type=="sort_dev")//重新排序分组内的设备
        {
            QString group_name="";
            if(root_obj.contains("group_name"))
            {
                QJsonValue value = root_obj.value("group_name");
                group_name=value.toString();
            }
            GroupNodeStruct *pGroupNode=searchGroupNode(group_name);
            if(pGroupNode)//更新分组内的设备
            {
                QJsonArray dev_array;
                if(root_obj.contains("dev_list"))
                {
                    QJsonValue value = root_obj.value("dev_list");
                    dev_array=value.toArray();
                }
               emit siqClearDevice(group_name);
                pGroupNode->devList.clear();
                int nSize=dev_array.size();
                for(int i=0; i<nSize; i++)
                {
                    u32 dev_sn=dev_array.at(i).toDouble();
                    pGroupNode->devList.append(dev_sn);
                    emit siqAddDevice2Group(group_name, dev_sn);//添加设备到分组
                }  
                emit siqUpdateGroupTotalNum(group_name, pGroupNode->devList.size());//更新设备数量
            }
        }        
    }
}


void CenterMan::parseDeciceRecv(QByteArray msg_ba)
{
#define     OUT_BUFF_SIZE   1500
#define     IN_BUFF_SIZE       1500
    u8 head[]={0xAA, 0x55}, *pBuff=NULL, *pData=NULL;
    u8 pack_num=0, cmd_type=0;
    u32 app_id, curr_dev_sn, next_dev_sn, parent_sn;
    u16 in_len, out_len, crcValue, data_len=0;
    u8 out_buff[OUT_BUFF_SIZE]={0}, in_buff[IN_BUFF_SIZE]={0};


    if( (pBuff=drv_com.memstr((u8*)msg_ba.data(), msg_ba.size(), head, 2)) != NULL )
    {
        ServerHeadStruct *pSeverHead=(ServerHeadStruct*)pBuff;
        crcValue=pSeverHead->crc_h<<8|pSeverHead->crc_l;
        data_len=pSeverHead->data_len_h<<8|pSeverHead->data_len_l;

        pData=pBuff+8;//指到app_id地址
        if(data_len<8 ||  crcValue != drv_com.crc16(pData, data_len))//CRC校验
        {
            qDebug("deviceRecvProcess: crc error!\n");
            return;
        }
        app_id=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
        pData+=4;
        curr_dev_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
        parent_sn=0;
        pData+=4;//指向数据区
//        qDebug("deviceRecvProcess: app_id=%u, gw_sn=%08X", app_id, curr_dev_sn);
        if(app_id==0 || curr_dev_sn==0)
        {
            qDebug("deviceRecvProcess error: app_id==0 || curr_dev_sn==0");
            return;
        }

        in_len=data_len-8;//加密数据长度
        if(in_len>IN_BUFF_SIZE)//数据包过长
        {
            qDebug("deviceRecvProcess error: in_len>DATA_BUFF_SIZE, in_len=%d", in_len);
            return;
        }
        memcpy(in_buff, pData, in_len);//复制数据
        u8 deep_counts=1;


        
        while(curr_dev_sn>0)
        {
            u8 encrypt_mode=ENCRYPT_MODE_DISABLE;
            u8 encrypt_index=0;
            u8 passwd_buff[20]={0};
            if(deep_counts==1)//网关设备
            {
                encrypt_index=pSeverHead->encrypt_index;//密码索引
                encrypt_mode=takeDevicePasswd(curr_dev_sn, encrypt_index, passwd_buff);//获取该设备的通讯密码
            } 

            switch(encrypt_mode)
            {
                case ENCRYPT_MODE_DISABLE: //不加密
                {
                    out_len=in_len;
                    if(out_len>OUT_BUFF_SIZE)
                        out_len=0;
                    memcpy(out_buff, in_buff, in_len);
                    break;
                }
                case ENCRYPT_MODE_TEA: //TEA加密
                {
                    out_len=drv_com.TEA_DecryptBuffer(in_buff, in_len, (u32*)passwd_buff);
                    if(out_len>OUT_BUFF_SIZE)
                        out_len=0;
                    memcpy(out_buff, in_buff, out_len);
                    break;
                }
                case ENCRYPT_MODE_AES: //AES加密
                {
                    out_len=drv_com.aes_decrypt_buff(in_buff, in_len, out_buff, OUT_BUFF_SIZE, passwd_buff);
                    break;
                }
                default: return;//出错
            }
            if(out_len>0)
            {
                pBuff=pData=out_buff;
                data_len=pData[0]<<8|pData[1];
                pData+=2;
                pack_num=pData[0];
                pData++;
                cmd_type=pData[0];
                pData++;//指向数据区或者下一个数据单元
//                qDebug("data_len=%u, pack_num=%u, cmd_type=%u", data_len, pack_num, cmd_type);
                if(data_len<4 || (data_len>IN_BUFF_SIZE))
                {
                    qDebug("deviceRecvProcess error: data_len<4 || data_len>IN_BUFF_SIZE, data_len=%d, pack_num=%d", data_len, pack_num);
                    return;
                }
                crcValue=pBuff[data_len]<<8|pBuff[data_len+1];
                if(crcValue != drv_com.crc16(pBuff, data_len))//单元内校验
                {
                    qDebug("deviceRecvProcess error: crcValue failed!");
                    return;
                }

                if(cmd_type==100)//是否为数据包命令
                {
                    next_dev_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
//                    qDebug("next_dev_sn=0x%08X", next_dev_sn);
                    pData+=4;
                    parent_sn=curr_dev_sn;
                    curr_dev_sn=next_dev_sn;
                    in_len=data_len-8;//下一个数据区的长度
                    memset(in_buff, 0, IN_BUFF_SIZE);
                    memcpy(in_buff, pData, in_len);//复制下一个数据单元
                }
                else//本机数据,进行处理
                {
//                    QByteArray msg_ba((char*)pData, data_len-4);
//                    qDebug()<<"msg_ba="<<msg_ba.toHex(':');
                    if(app_id==m_currAppWork.appID)//本项目设备
                    {
                        WorkDevStruct *pWorkNode=searchWorkNode(curr_dev_sn);
                        if(pWorkNode)
                        {
                            if(parent_sn>0 && pWorkNode->parentSn != parent_sn)//节点父序列号改变
                            {
                                pWorkNode->parentSn=parent_sn;
//                                requestUpdateWorkName(curr_dev_sn, pWorkNode->devName);//更新父序列号
                            }
                            if(parent_sn==0)
                            {
                                pWorkNode->encrypt_index=pSeverHead->encrypt_index;//记录网关的加密模式,方便数据下发
                            }
                            if(pWorkNode->pModel)
                            {
                                pWorkNode->pModel->setRawData(app_id, curr_dev_sn, pack_num, cmd_type, pData, data_len-4);
                            }
                            return;
                        }
                        else//设备未存在
                        {
                            if(m_secCounts>10)//可能是还未从服务器请求下来
                            {
                                InitTypeStruct *pInittype=takeInitType(curr_dev_sn);
                                if(pInittype)
                                {
                                    requestAddDevice(curr_dev_sn, parent_sn);//请求添加设备
                                }
                            }

                        }
                    }
                    curr_dev_sn=0;
                    return;
                }
                deep_counts++;
                if(deep_counts>=5)//嵌套层数
                {
                    qDebug("deviceRecvProcess error: deep_counts>DEEP_MAX! parent_sn=%u", parent_sn);
                    return;
                }
            }
            else
            {
                qDebug("deviceRecvProcess error: out_len<=0!");
                return;
            }
        }

    }
    else
    {
        qDebug("no found head AA 55");
    }
}

u16 CenterMan::makeGwSendBuff(u32 app_id, u32 gw_sn, u8 pack_num, u8 encrypt_index, u8 msg_type, u8 *in_buff, u16 in_len, u8 *make_buff, u16 make_size)
{
    u8 passwd_buff[20]={0};
    u8 tmp_buff[1500]={0};
    u16 make_len=0,tmp_len=0, data_len, crcValue;
    if(in_len+32>make_size || in_len+32>(int)sizeof(tmp_buff))
    {
        qDebug("makeGwSendBuff error: in_len+10>make_size");
        return 0;
    }
     u8 encrypt_mode=takeDevicePasswd(gw_sn, encrypt_index, passwd_buff);

    //数据区
    data_len=in_len+4;
    tmp_len=0;
    tmp_buff[tmp_len++]=data_len>>8;//加密区
    tmp_buff[tmp_len++]=data_len;
    tmp_buff[tmp_len++]=pack_num;
    tmp_buff[tmp_len++]=msg_type;
    memcpy(&tmp_buff[tmp_len], in_buff, in_len);
    tmp_len+=in_len;
    crcValue=drv_com.crc16(tmp_buff, tmp_len);
    tmp_buff[tmp_len++]=crcValue>>8;
    tmp_buff[tmp_len++]=crcValue;

    u8 *pData=&make_buff[16];
    //加密
    switch(encrypt_mode)
    {
        case ENCRYPT_MODE_DISABLE:
        {
            memcpy(pData, tmp_buff, tmp_len);
            break;        
        }
        case ENCRYPT_MODE_AES:
        {
            tmp_len=drv_com.aes_encrypt_buff(tmp_buff, tmp_len, pData, make_size-16, passwd_buff);
            break;    
        }
        case ENCRYPT_MODE_TEA:
        {
            u8 remain_len=tmp_len%8;
            if(remain_len>0)
            {
                tmp_len=tmp_len+(8-remain_len);
            }
            tmp_len=drv_com.TEA_EncryptBuffer(tmp_buff, tmp_len, (u32*)passwd_buff);
            memcpy(pData, tmp_buff, tmp_len);
            break;
        }
        default:
        {
            qDebug("encrypt_mode gw  error!");
            return 0;
        }
 
    }

    if(tmp_len>0)
    {
        data_len=tmp_len+8; 
        make_len=0;
        make_buff[make_len++]=0xAA; 
        make_buff[make_len++]=0x55;
        make_buff[make_len++]=SERVER_PROTOCOL_VER;
        make_buff[make_len++]=encrypt_index;
        make_buff[make_len++]=crcValue>>8;
        make_buff[make_len++]=crcValue;
        make_buff[make_len++]=data_len>>8;
        make_buff[make_len++]=data_len;
        make_buff[make_len++]=app_id>>24;
        make_buff[make_len++]=app_id>>16;
        make_buff[make_len++]=app_id>>8;
        make_buff[make_len++]=app_id;
        make_buff[make_len++]=gw_sn>>24;
        make_buff[make_len++]=gw_sn>>16;
        make_buff[make_len++]=gw_sn>>8;
        make_buff[make_len++]=gw_sn;

        crcValue=drv_com.crc16(&make_buff[8], data_len);
        make_buff[4]=crcValue>>8;
        make_buff[5]=crcValue;
        make_len=tmp_len+make_len;
    }

    return make_len;
}

u16 CenterMan::makeNodeSendBuff(u32 dev_sn, u8 pack_num, u8 msg_type, u8 *in_buff, u16 in_len, u8 *make_buff, u16 make_size)
{
    u16 make_len=0, data_len=in_len+4;
    u16  crcValue;
    u8 *pUnion=nullptr;


    if(make_size<in_len+10)
        return 0;
    make_buff[make_len++]=dev_sn>>24;
    make_buff[make_len++]=dev_sn>>16;
    make_buff[make_len++]=dev_sn>>8;
    make_buff[make_len++]=dev_sn;
    pUnion=&make_buff[make_len];//校验码开始阶段
    make_buff[make_len++]=data_len>>8;
    make_buff[make_len++]=data_len;
    make_buff[make_len++]=pack_num;
    make_buff[make_len++]=msg_type;
    memcpy(&make_buff[make_len], in_buff, in_len);
    make_len+=in_len;
    crcValue=drv_com.crc16(pUnion, make_len-4);
    make_buff[make_len++]=crcValue>>8;
    make_buff[make_len++]=crcValue;
    return make_len;
}

//发送设备下行数据 
void CenterMan::publishDownMsg(u32 app_id, u32 gw_sn, u8 *msg_buff, u16 msg_len)
{
    QString topic=QString(TOPIC_HEAD)+QString::asprintf("dev/sub/data/%u/%08X", app_id, gw_sn);
    QByteArray msg_ba((char*)msg_buff, msg_len);
//    qDebug()<<"000 pub topic="<<topic<<"\nmsg_ba="<<msg_ba.toHex(':');
     emit sigMqttPushMessage(topic, msg_ba);
}


//更新弹框内的应用列表
void CenterMan::updateAppListName(void)
{
    int nSize=m_appWorkList.size();
    for(int i=0; i<nSize; i++)
    {
        u32 app_id=m_appWorkList[i].appID;
        QString app_name=m_appWorkList[i].appName;
        emit siqUpdateAppListName(i, app_id, app_name, app_id==m_currAppWork.appID);
        
    }
}

//选择切换应用
void CenterMan::selectCurrApp(qint64 app_id)
{
    if(app_id==m_currAppWork.appID)
        return;
    
    int nSize=m_appWorkList.size();
    for(int i=0; i<nSize; i++)
    {
        if(app_id==m_appWorkList[i].appID)
        {
            m_currAppWork=m_appWorkList[i];
            break;
        }
    }
    emit siqUpdateCurrAppName(m_currAppWork.appID, m_currAppWork.appName);
    writeConfig();
    requestDevList();//请求设备列表
}

CenterMan::GroupNodeStruct *CenterMan::searchGroupNode(QString group_name)
{
    GroupNodeStruct *pGroupNode=nullptr;
    for(int i=0; i<m_groupList.size(); i++)
    {
        if(m_groupList[i].groupName==group_name)
        {
            pGroupNode=&m_groupList[i];
            break;
        }
    }
    return pGroupNode;
}

//清除分组,在切换应用时使用
void CenterMan::clearGroup(void)
{
    for(auto iter : m_workDevList)//隐藏所有设备
    {
        if(iter.pModel)
        {
            iter.pModel->hideModel();
            iter.pModel->hideSimple();
        }
    }
    m_groupList.clear();
    emit siqClearGroup();//清空分组
}

void CenterMan::updateGroupOnlineText(void)
{
    for(auto group : m_groupList)
    {
        int total_num=group.devList.size(), online_num=0;
        for(auto device : group.devList)
        {
            WorkDevStruct *pWork=searchWorkNode(device);
            if(pWork && pWork->pModel)
            {
                if(pWork->pModel->takeOnlineState()>0)
                {
                    online_num++;
                }
            }
            emit siqUpdateOnlineText(group.groupName, QString::asprintf("%d/%d", online_num, total_num));
        }
    }
}

void CenterMan::initDevTypeList(void)
{
    InitTypeStruct tag_initNode;
    tag_initNode.modelName="空气净化器";
    tag_initNode.modelType="AP-WF-01";
    tag_initNode.modelValue=MODEL_TYPE_AP01;
    tag_initNode.level=0;
    m_initTypeList.append(tag_initNode);
    
    tag_initNode.modelName="温湿度计";
    tag_initNode.modelType="TH-WF-01";
    tag_initNode.modelValue=MODEL_TYPE_TH01;
    tag_initNode.level=0;
    m_initTypeList.append(tag_initNode);
    
}

CenterMan::InitTypeStruct *CenterMan::takeInitType(u32 dev_sn)  
{
    u16 dev_type=dev_sn>>16;
    int i=0;
    for(auto iter : m_initTypeList)
    {
        if(iter.modelValue==dev_type)
        {
            return &m_initTypeList[i];
        }
        i++;
    }
    return nullptr;
}


CenterMan::WorkDevStruct * CenterMan::addWorkDevice(u32 dev_sn, u32 parent_sn, QString dev_name)
{
    WorkDevStruct *pWorkNode=searchWorkNode(dev_sn); //检查是否添加过
    if(pWorkNode)
        return pWorkNode;
    if(dev_name=="0" || dev_name.isEmpty())
        dev_name=QString::asprintf("%08X", dev_sn);
    WorkDevStruct workNode;
    workNode.appID=m_currAppWork.appID;
    workNode.devSn=dev_sn;
    workNode.devName=dev_name;
    workNode.parentSn=parent_sn;
    workNode.pModel=nullptr; 
    switch(dev_sn>>16)
    {
        case MODEL_TYPE_AP01:
        {
            workNode.pModel = new ModelAp01(this);
            break;
        }
        case MODEL_TYPE_TH01:
        {
            workNode.pModel = new ModelTh01(this);
            break;
        }
      
        default:return nullptr;
    }
    if(workNode.pModel)
    {
        InitTypeStruct *pInit=takeInitType(dev_sn);
        if(pInit)
        {
            workNode.pModel->setModelName(pInit->modelName);
            workNode.pModel->setModelType(pInit->modelType);
        }
        workNode.pModel->setDeviceSn(dev_sn);
        workNode.pModel->setDeviceName(dev_name);
        workNode.pModel->setAppID(m_currAppWork.appID);
        connect(workNode.pModel, SIGNAL(sigSendDownMsg(u32,u32,u8,u8,u8*,u16)), this ,SLOT(slotSendDownMsg(u32,u32,u8,u8,u8*,u16)));     
    }
    m_workDevList.append(workNode);
    return &m_workDevList.last();
}


void CenterMan::delWorkDevice(u32 dev_sn)
{
    int i=0;
    for(auto iter : m_workDevList)
    {
        if(iter.devSn==dev_sn)
        {
            if(iter.pModel)
            {
                iter.pModel->hideModel();
                iter.pModel->hideSimple();
                delete iter.pModel;
            }
            m_workDevList.removeAt(i);
            return;
        }
        i++;
    }
}

void CenterMan::clearWorkList(void)
{

    for(auto iter : m_workDevList)
    {
        if(iter.pModel)
        {
            iter.pModel->hideModel();
            iter.pModel->hideSimple();
            delete iter.pModel;
        }
    }
    m_workDevList.clear();
}

void CenterMan::updateWorkName(u32 dev_sn, QString new_name)
{
    WorkDevStruct *pWorkNode=searchWorkNode(dev_sn);
    if(pWorkNode)
    {
        pWorkNode->devName=new_name;
        if(pWorkNode->pModel)
        {
            pWorkNode->pModel->setDeviceName(new_name);
        }
    }
}

QString CenterMan::takeWorkDeviceName(qint64 dev_sn)
{
    QString dev_name;
    WorkDevStruct *pWorkNode=searchWorkNode(dev_sn);
    if(pWorkNode)
    {
        dev_name=pWorkNode->devName;
    }
    return dev_name;
}

CenterMan::WorkDevStruct *CenterMan::searchWorkNode(u32 dev_sn)
{
    WorkDevStruct *pWorkNode=nullptr;
    int i=0;
    for(auto iter : m_workDevList)
    {
        if(iter.devSn==dev_sn)
        {
            pWorkNode=&m_workDevList[i];
            break;
        }
        i++;
    }
    return pWorkNode;
}


void CenterMan::showModelView(qint64 dev_sn)
{
    if(m_currModelDevSn>0)//隐藏原来的显示设备
    {
        WorkDevStruct *pWorkNode=searchWorkNode(m_currModelDevSn);
         if(pWorkNode && pWorkNode->pModel)
         {
             pWorkNode->pModel->hideModel();
         }
    }
    WorkDevStruct *pWorkNode=searchWorkNode(dev_sn);
    if(pWorkNode && pWorkNode->pModel)
    {
        pWorkNode->pModel->showModel(m_modelParent);
        m_currModelDevSn=dev_sn;
        emit siqSetCenterCurrView("center-dev");
    }
}

void CenterMan::hideModelView(qint64 dev_sn)
{
    WorkDevStruct *pWorkNode=searchWorkNode(dev_sn);
    if(pWorkNode && pWorkNode->pModel)
    {
        pWorkNode->pModel->hideModel();
    }
}


void CenterMan::showSimpleView(qint64 dev_sn, QObject *parent)
{
    
    WorkDevStruct *pWorkNode=searchWorkNode(dev_sn);
    
    if(pWorkNode && pWorkNode->pModel)
    {
        pWorkNode->pModel->showSimple(parent);
    }
}

void CenterMan::hideSimpleView(qint64 dev_sn)
 {
    WorkDevStruct *pWorkNode=searchWorkNode(dev_sn);
    if(pWorkNode && pWorkNode->pModel)
    {
        pWorkNode->pModel->hideSimple();
    }
}

void CenterMan::setModelParent(QObject *parent)
{
    m_modelParent=parent;
}





