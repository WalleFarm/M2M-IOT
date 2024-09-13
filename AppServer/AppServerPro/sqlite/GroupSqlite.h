#ifndef GROUPSQLITE_H
#define GROUPSQLITE_H

#include <QObject>
#include "BaseSqlite.h"
class GroupSqlite : public BaseSqlite
{
    Q_OBJECT
public:
    typedef struct   
    {
        u32 groupOrder;
        QString groupName;
        QList<u32>devList;
        QString createTime;
    }GroupNodeStruct;
    
    typedef struct   
    {
        u32 devSn;
        u32 parentSn;
        QString devName;
        QString createTime;
    }DeviceNodeStruct;   
    
public:
    explicit GroupSqlite(QObject *parent = nullptr);
    
    bool createGroupTable(void);
    bool insertGroup(QString groupName);
    bool updateGroupName(QString oldName, QString newName);
    bool updateGroupOrder(QString groupName, int groupOrder);
    bool updateDevList(QString groupName, QList<u32>devList);
    bool selectGroupNode(QString groupName, GroupNodeStruct &groupNode);
    bool selectGroupList(QList<GroupNodeStruct> &groupList);
    bool delGroupNode(QString groupName);
    bool clearGroupList(void);
    
    
    bool createDeviceTable(void);
    bool insertDeviceNode(u32 devSn, u32 parentSn);
    bool updateDeviceName(u32 devSn, QString devName);
    bool updateDeviceParentSn(u32 devSn, u32 parentSn);
    bool selectDeviceNode(u32 devSn, DeviceNodeStruct &workNode);
    bool selectDeviceList(QList<DeviceNodeStruct> &workList);
    bool delDeviceNode(u32 devSn);
    
signals:
    
};

#endif // GROUPSQLITE_H
