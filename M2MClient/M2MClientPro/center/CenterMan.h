#ifndef CENTERMAN_H
#define CENTERMAN_H

#include <QObject>
#include <QSysInfo>
#include "drivers/DrvCommon.h"
#include "user_opt.h"

#include "modelCpp/ModelAp01.h"
#include "modelCpp/ModelTh01.h"

class CenterMan : public QObject
{
    Q_OBJECT
public:
    enum ModelType 
    {
        MODEL_TYPE_AP01=0xA101,//空气净化器
        MODEL_TYPE_TH01=0xA108,//温湿度计(演示)
        
    };
    typedef enum
    {
        ENCRYPT_MODE_DISABLE=0,
        ENCRYPT_MODE_TEA,
        ENCRYPT_MODE_AES,
    }encryptMode;
    typedef struct
    {
        u8 head[2];
        u8 version;
        u8 encrypt_index;//密码索引
        u8 crc_h;
        u8 crc_l;
        u8 data_len_h;
        u8 data_len_l;
    }ServerHeadStruct;
    
    typedef struct   
    {
        u16 modelValue;//类型值
        QString modelName;//类型名称
        QString modelType;//型号
        u8 level;//0--网关;  其它--节点
    }InitTypeStruct;    
    typedef struct
    {
        QString devName;
        u32 devSn;
        u32 parentSn;
        u32 appID;
        u8 encrypt_index;
        BaseModel *pModel;
    }WorkDevStruct; 
    
    typedef struct
    {
        u32 appID;
        QString appName;
        QString createTime;
    }AppWorkStruct;
    
    typedef struct   
    {
        QString groupName;
        QList<qint64>devList;
    }GroupNodeStruct;
    
public:
    explicit CenterMan(QObject *parent = nullptr);
    void setMacInfo(QString mac, int rand_num);
    QString makePubTopic(QString key_str);
    void readConfig(void);
    void writeConfig(void);
    u32 takeCurrAppID(void);
    
    Q_INVOKABLE void requestNewApp(QString new_name);
    Q_INVOKABLE void requestRenameApp(qint64 app_id, QString new_name);
    Q_INVOKABLE void requestAppList(void);
    Q_INVOKABLE void requestGroupList(void);
    Q_INVOKABLE void requestAddGroup(QString group_name);
    Q_INVOKABLE void requestRenameGroup(QString old_name, QString new_name);
    Q_INVOKABLE void requestDelGroup(QString group_name);
    Q_INVOKABLE void requestOrderGroup(QStringList group_list);
    Q_INVOKABLE void requestDevList(void);
                                void requestAddDevice(u32 dev_sn, u32 parent_sn);
                                void requestRegDevice(u32 dev_sn);
    Q_INVOKABLE QString requestAddDeviceQml(QString dev_sn_str);
    Q_INVOKABLE void requestSortDevice(QString group_name, QList<qint64> dev_list);
    Q_INVOKABLE void requestDelDevice(QList<qint64> dev_list);
    Q_INVOKABLE void requestRenameDevice(u32 dev_sn, QString new_name);
    Q_INVOKABLE void requestUpdateParentSn(u32 dev_sn, u32 parent_sn);
    Q_INVOKABLE void requestMoveDevice(QString src_group, QString dst_group, QList<qint64> dev_list);
    
    Q_INVOKABLE void updateAppListName(void);
    Q_INVOKABLE void selectCurrApp(qint64 app_id);
    
    Q_INVOKABLE void showModelView(qint64 dev_sn);
    Q_INVOKABLE void hideModelView(qint64 dev_sn);
    Q_INVOKABLE void showSimpleView(qint64 dev_sn, QObject *parent);
    Q_INVOKABLE void hideSimpleView(qint64 dev_sn);
    Q_INVOKABLE void setModelParent(QObject *parent);                             
    
    GroupNodeStruct *searchGroupNode(QString group_name);
    void clearGroup(void);
    void updateGroupOnlineText(void);
    void initDevTypeList(void);
    InitTypeStruct * takeInitType(u32 dev_sn);
    WorkDevStruct *addWorkDevice(u32 dev_sn, u32 parent_sn=0, QString dev_name="0");
    void delWorkDevice(u32 dev_sn);
    void clearWorkList(void);
    void updateWorkName(u32 dev_sn, QString new_name);  
    Q_INVOKABLE QString takeWorkDeviceName(qint64 dev_sn);
    WorkDevStruct *searchWorkNode(u32 dev_sn);
    
    u8 takeDevicePasswd(u32 dev_sn, u8 index, u8 *passwd_buff);
    void parseCenterRecv(QString topic, QJsonObject root_obj);
    void parseDeciceRecv(QByteArray msg_ba);
    u16 makeNodeSendBuff(u32 dev_sn, u8 pack_num, u8 msg_type, u8 *in_buff, u16 in_len, u8 *make_buff, u16 make_size);
    u16 makeGwSendBuff(u32 app_id, u32 gw_sn, u8 pack_num, u8 encrypt_index, u8 msg_type, u8 *in_buff, u16 in_len, u8 *make_buff, u16 make_size);
    void publishDownMsg(u32 app_id, u32 gw_sn, u8 *msg_buff, u16 msg_len);
    
private:
    QTimer *checkTimer;
    u32 m_secCounts;
    QString m_macStr;
    int m_randNum;
    DrvCommon drv_com;
    QString m_rootPath;
    QString m_loginAccount;
    
    AppWorkStruct m_currAppWork;
    QList<AppWorkStruct>m_appWorkList;
    QList<GroupNodeStruct>m_groupList;
    QList<WorkDevStruct>m_workDevList;
    QList<InitTypeStruct>m_initTypeList;
    
    QObject *m_modelParent;
    u32 m_currModelDevSn;
signals:
    void siqShowMsg(QString msg);
    void siqSetCenterCurrView(QString view_name);
    void siqUpdateCurrAppName(qint64 app_id, QString app_name);
    void siqUpdateAppListName(int index, qint64 app_id, QString app_name, int select);
    void siqAddGroup(QString group_name,  int total_num=0);//添加分组
    void siqUpdateGroupTotalNum(QString group_name,  int total_num=0);//更新设备数量
    void siqRenameGroup(QString old_name, QString new_name);//重命名分组
    void siqDelGroup(QString group_name);//删除分组
    void siqClearGroup(void);//清除分组
    void siqUpdateOnlineText(QString group_name, QString online_text);
    
    void siqAddDevice2Group(QString group_name, qint64 dev_sn);//新增设备到分组
    void siqDelDevice(qint64 dev_sn, int flag=0);//删除设备
    void siqClearDevice(QString group_name);//清空分组设备
signals:
    void sigMqttPushMessage(QString topic, QByteArray msg_ba);    
    
public slots:
    void slotCheckTimeout(void);
    void slotUpdateLoginAccount(QString account, int state);
    void slotMqttConnected(void);
    void slotSendDownMsg(u32 app_id, u32 dev_sn, u8 pack_num, u8 msg_type, u8 *msg_buff, u16 msg_len);
    
};

#endif // CENTERMAN_H
