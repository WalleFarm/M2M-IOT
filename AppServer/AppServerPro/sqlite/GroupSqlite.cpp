#include "GroupSqlite.h"

GroupSqlite::GroupSqlite(QObject *parent) : BaseSqlite(parent)
{
    
}



bool GroupSqlite::createGroupTable(void)
{
    QString str_query = QString::asprintf("CREATE TABLE If Not Exists group_tb ("
                                          "groupName varchar(1000)  NOT NULL,"
                                          "groupOrder INTEGER  DEFAULT 1000,"
                                          "devList varchar(10000) DEFAULT 0,"
                                          "createTime timestamp DEFAULT (datetime(\'now\',\'localtime\')),"
                                          "PRIMARY KEY (groupName)"
                                          ")"
                                          );
//    qDebug()<<"str_query= "<<str_query;
    if(runSqlQuery(str_query)==false)
    {
        qDebug()<<"createGroupTable error";
        return false;
    }
    else
    {
        qDebug()<<"createGroupTable ok:  ";

    }
    return true;
}

bool GroupSqlite::insertGroup(QString groupName)
{
    QString str_query = QString::asprintf("INSERT INTO group_tb (groupName) VALUES ( \"%s\")",
                                          groupName.toUtf8().data());
//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("insertGroup ok 001!");
        return true;
    }
    else   
    {
        createGroupTable();
        if( runSqlQuery(str_query))
        {
            qDebug("insertGroup ok 002!");
            return true;
        }
    }
    return false;
}

bool GroupSqlite::updateGroupName(QString oldName, QString newName)
{
    QString str_query = QString::asprintf("UPDATE group_tb SET  groupName=\"%s\"   WHERE groupName=\"%s\"",
                                          newName.toUtf8().data(), oldName.toUtf8().data());

//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("updateGroupName ok!");
        return true;
    }
    return false;
}

bool GroupSqlite::updateGroupOrder(QString groupName, int groupOrder)
{
    QString str_query = QString::asprintf("UPDATE group_tb SET  groupOrder=%d   WHERE groupName=\"%s\"",
                                          groupOrder, groupName.toUtf8().data());

//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
//        qDebug("updateGroupOrder ok!");
        return true;
    }
    return false;
}

bool GroupSqlite::updateDevList(QString groupName, QList<u32> devList)
{
    QJsonArray dev_array;
    int nSize=devList.size();
    for(int i=0; i<nSize; i++)
    {
        u32 dev_sn=devList.at(i);
        dev_array.append((qint64)dev_sn);
    }
    QJsonDocument json_doc;
    QJsonObject root_obj;
    root_obj.insert("dev_list", dev_array);
    json_doc.setObject(root_obj);
    QByteArray json_ba=json_doc.toJson().toBase64();
    QString json_str(json_ba.data());
    
    QString str_query = QString::asprintf("UPDATE group_tb SET  devList=\"%s\"   WHERE groupName=\"%s\"",
                                          json_str.toUtf8().data(), groupName.toUtf8().data());

//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("updateDevList ok!");
        return true;
    }
    return false;
}

bool GroupSqlite::selectGroupNode(QString groupName, GroupNodeStruct &groupNode)
{
    QString str_query = QString::asprintf("SELECT groupName, groupOrder, devList, createTime  FROM  group_tb WHERE groupName=\"%s\"" , 
                                          groupName.toUtf8().data());
    
    //    qDebug()<<str_query;
        if(runSqlQuery(str_query)==false)
        {
            qDebug("selectGroupNode error_01!");
             return false;
        }
        while(m_sqlQuery.next())
        {
            int ptr=0;
            groupNode.groupName=m_sqlQuery.value(ptr++).toString();
            groupNode.groupOrder=m_sqlQuery.value(ptr++).toUInt();
            groupNode.devList.clear();
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
                if(root_obj.contains("dev_list"))//应用列表
                {
                    QJsonValue value=root_obj.value("dev_list");
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
                                    groupNode.devList.append(dev_sn);//添加dev_sn
                                }
                            }
                        }
                    }
                }
            }
            groupNode.createTime=m_sqlQuery.value(ptr++).toString();
            break;
        }
        m_sqlQuery.finish();    
        return true;
}

bool GroupSqlite::selectGroupList(QList<GroupNodeStruct> &groupList)
{
    QString str_query = QString::asprintf("SELECT groupName, groupOrder, devList, createTime  FROM  group_tb  ORDER BY groupOrder ASC");                                            
    //    qDebug()<<str_query;
        if(runSqlQuery(str_query)==false)
        {
            qDebug("selectGroupList error_01!");
             return false;
        }
        while(m_sqlQuery.next())
        {
            int ptr=0;
            GroupNodeStruct tag_group_node;
            tag_group_node.groupName=m_sqlQuery.value(ptr++).toString();
            tag_group_node.groupOrder=m_sqlQuery.value(ptr++).toUInt();
            tag_group_node.devList.clear();
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
                if(root_obj.contains("dev_list"))//设备列表
                {
                    QJsonValue value=root_obj.value("dev_list");
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
                                    tag_group_node.devList.append(dev_sn);//添加dev_sn
                                }
                            }
                        }
                    }
                }
            }
            tag_group_node.createTime=m_sqlQuery.value(ptr++).toString();
            groupList.append(tag_group_node);
        }
        m_sqlQuery.finish();    
        return true;
}

bool GroupSqlite::delGroupNode(QString groupName)
{
    QString str_query = QString::asprintf("DELETE FROM group_tb WHERE groupName=\"%s\"", groupName.toUtf8().data());

//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("delGroupNode ok!");
        return true;
    }
    return false;
}

bool GroupSqlite::clearGroupList(void)
{
    QString str_query = QString::asprintf("delete from group_tb");
    if( runSqlQuery(str_query))
    {
        qDebug("clearGroupList ok!");
        return true;
    }
    return false;
}


/******************************************
*******工作设备数据库
******************************************/
bool GroupSqlite::createDeviceTable(void)
{
    QString str_query = QString::asprintf("CREATE TABLE If Not Exists device_tb ("
                                          "devSn bigint  NOT NULL,"
                                          "devStr varchar(20)  NOT NULL,"
                                          "parentSn bigint  DEFAULT 0,"
                                          "parentStr varchar(20)  DEFAULT 0,"
                                          "devName varchar(10000)  DEFAULT 0,"
                                          "createTime timestamp DEFAULT (datetime(\'now\',\'localtime\')),"
                                          "PRIMARY KEY (devSn)"
                                          ")"
                                          );
//    qDebug()<<"str_query= "<<str_query;
    if(runSqlQuery(str_query)==false)
    {
        qDebug()<<"createDeviceTable error";
        return false;
    }
    else
    {
        qDebug()<<"createDeviceTable ok:  ";

    }
    return true;
}

bool GroupSqlite::insertDeviceNode(u32 devSn, u32 parentSn)
{
    QString dev_str=QString::asprintf("%08X", devSn);
    QString parent_str=QString::asprintf("%08X", parentSn);
    QString str_query = QString::asprintf("INSERT INTO device_tb (devSn, parentSn, devStr, parentStr) VALUES ( %u, %u, \"%s\", \"%s\")",
                                                             devSn, parentSn, dev_str.toLatin1().data(), parent_str.toLatin1().data());
                                          
//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("insertDeviceNode ok 001!");
        return true;
    }
    else   
    {
        createDeviceTable();
        if( runSqlQuery(str_query))
        {
            qDebug("insertDeviceNode ok 002!");
            return true;
        }
    }
    return false;
}

bool GroupSqlite::updateDeviceName(u32 devSn, QString devName)
{
    QString str_query = QString::asprintf("UPDATE device_tb SET  devName=\"%s\"   WHERE devSn=%u",
                                          devName.toUtf8().data(), devSn);

//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("updateDeviceName ok!");
        return true;
    }
    return false;
}

bool GroupSqlite::updateDeviceParentSn(u32 devSn, u32 parentSn)
{
    QString parent_str=QString::asprintf("%08X", parentSn);
    QString str_query = QString::asprintf("UPDATE device_tb SET  parentSn=%u, parentStr=\"%s\"   WHERE devSn=%u",
                                          parentSn, parent_str.toLatin1().data(),devSn);

//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("updateDeviceParentSn ok!");
        return true;
    }
    return false;
}

bool GroupSqlite::selectDeviceNode(u32 devSn, DeviceNodeStruct &workNode)
{
    QString str_query = QString::asprintf("SELECT devSn, parentSn, devName, createTime  FROM  device_tb WHERE devSn=%u" , 
                                          devSn);
    
    //    qDebug()<<str_query;
        if(runSqlQuery(str_query)==false)
        {
            qDebug("selectDeviceNode error_01!");
             return false;
        }
        while(m_sqlQuery.next())
        {
            int ptr=0;
            workNode.devSn=m_sqlQuery.value(ptr++).toUInt();
            workNode.parentSn=m_sqlQuery.value(ptr++).toUInt();
            workNode.devName=m_sqlQuery.value(ptr++).toString();   
            workNode.createTime=m_sqlQuery.value(ptr++).toString();
            break;
        }
        m_sqlQuery.finish();    
        return true;  
}

bool GroupSqlite::selectDeviceList(QList<DeviceNodeStruct> &workList)
{
    QString str_query = QString::asprintf("SELECT devSn, parentSn,  devName, createTime  FROM  device_tb");
                                          
    
    //    qDebug()<<str_query;
        if(runSqlQuery(str_query)==false)
        {
            qDebug("selectDeviceList error_01!");
             return false;
        }
        while(m_sqlQuery.next())
        {
            int ptr=0;
            DeviceNodeStruct workNode;
            workNode.devSn=m_sqlQuery.value(ptr++).toUInt();
            workNode.parentSn=m_sqlQuery.value(ptr++).toUInt();
            workNode.devName=m_sqlQuery.value(ptr++).toString();   
            workNode.createTime=m_sqlQuery.value(ptr++).toString();
            workList.append(workNode);
        }
        m_sqlQuery.finish();    
        return true;  
}

bool GroupSqlite::delDeviceNode(u32 devSn)
{
    QString str_query = QString::asprintf("DELETE FROM device_tb WHERE devSn=%u", devSn);

//    qDebug()<<str_query;
    if( runSqlQuery(str_query))
    {
        qDebug("delDeviceNode ok!");
        return true;
    }
    return false;
}















