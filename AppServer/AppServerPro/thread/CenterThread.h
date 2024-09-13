#ifndef CENTERTHREAD_H
#define CENTERTHREAD_H

#include <QObject>
#include "sqlite/AccountSqlite.h"
#include "sqlite/GroupSqlite.h"
#include "user_opt.h"

class CenterThread : public QObject
{
    Q_OBJECT
public:
    explicit CenterThread(QObject *parent = nullptr);
    QString makePubTopic(QString account, QString mac_str, int rand_num, QString key_str);
    QString takeAppFilesPath(u32 app_id);
    
    void parseAppTopic(QJsonObject root_obj);
    void parseGroupTopic(QJsonObject root_obj);
    void parseDeviceTopic(QJsonObject root_obj);
    
    void ackNewAppState(QString account, QString mac_str, int rand_num, u32 app_id, QString app_name, int result, QString ack_str);
    void ackRenameAppState(QString account, QString mac_str, int rand_num, u32 app_id, QString app_name, int result, QString ack_str);
    void ackReqAppListState(QString account, QString mac_str, int rand_num, QList<u32> app_list, int result, QString ack_str);
    void ackTakeGroupList(QString account, QString mac_str, int rand_num, u32 app_id, QJsonArray group_array, int result, QString ack_str);
    void ackAddGroup(QString account, QString mac_str, int rand_num, u32 app_id, QString group_name, int result, QString ack_str);
    void ackUpdateGroupName(QString account, QString mac_str, int rand_num, u32 app_id, QString old_name, QString new_name, int result, QString ack_str);
    void ackDelGroup(QString account, QString mac_str, int rand_num, u32 app_id, QString group_name, int result, QString ack_str);
    void ackOrderGroup(QString account, QString mac_str, int rand_num, u32 app_id, QJsonArray group_array, int result, QString ack_str);
    void ackDevList(QString account, QString mac_str, int rand_num, u32 app_id, QList<GroupSqlite::DeviceNodeStruct> work_list, int result, QString ack_str);
    void ackSortDevice(QString account, QString mac_str, int rand_num, u32 app_id, QString group_name, QJsonArray dev_array, int result, QString ack_str);
    void ackAddDevice(QString account, QString mac_str, int rand_num, u32 app_id, u32 dev_sn, u32 parent_sn, int result, QString ack_str);
    void ackDelDevice(QString account, QString mac_str, int rand_num, u32 app_id, QJsonArray dev_array, int result, QString ack_str);
    void ackRenameDevice(QString account, QString mac_str, int rand_num, u32 app_id, u32 dev_sn, QString new_name, int result, QString ack_str);
    void ackUpdateParentSn(QString account, QString mac_str, int rand_num, u32 app_id, u32 dev_sn, u32 parent_sn, int result, QString ack_str);
    
private:
    QTimer *checkTimer;
    u32 m_timerCounts, m_secCounts;
    DrvCommon drv_com;
    AccountSqlite *m_accountSqlite;
    
signals:   
    void sigMqttPushMessage(QString topic, QByteArray msg_ba);
    
public slots:
    void slotCheckTimeout(void);
    void slotCenterThreadMessage(QString topic, QJsonObject root_obj);
    
};

#endif // CENTERTHREAD_H
