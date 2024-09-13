#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <QObject>
#include "drivers/DrvCommon.h"  
#include <QQmlApplicationEngine>
#include<QQmlContext>
#include<QTextCodec>

class BaseModel : public QObject
{
    Q_OBJECT
public:
    
    typedef enum
    {
        SERVER_CMD_UP_DATA=100,
        SERVER_CMD_DOWN_DATA=200,
        
        REG_CMD_REPORT_APP_ID=210,
        REG_CMD_SET_APP_ID,
        
        UPDATE_CMD_IAP=220,//远程升级总命令
        UPDATE_CMD_INTO_BOOT,//使设备进入升级状态
        UPDATE_CMD_KEEP,//保持连接  
        
        SERVER_CMD_LORA=230,//LORA总命令
    }serverCmd;
    
    enum alarmLevel
    {
        ALARM_LEVEL0=0,
        ALARM_LEVEL1,
        ALARM_LEVEL2,

    };//报警等级
    
    enum alarmType
    {
        ALARM_TYPE_OK=0,
        ALARM_TYPE_UP_0,
        ALARM_TYPE_UP_1,
        ALARM_TYPE_DOWN_0,
        ALARM_TYPE_DOWN_1,
      
        ALARM_TYPE_RESET=20,//恢复正常
      
    };//报警类型
    enum devState
    {
        DEV_STATE_OFF_LINE=0,
        DEV_STATE_ON_LINE,
        DEV_STATE_WARN,
        DEV_STATE_ALARM,
    };//设备状态
    
    typedef struct
    {
       u16 value,upWarn,upAlarm,downWarn,downAlarm;
       u8 alarmType, alarmEnable, alarmLevel;
       u8 linePtr;       
    }AnalogStruct;
    
public:
    explicit BaseModel(QObject *parent = nullptr);
    void setDeviceSn(u32 dev_sn);
    Q_INVOKABLE u32 takeDeviceSn(void);
    void setAppID(u32 app_id);
    u32 takeAppID(void);
    void setSlaveAddr(u8 slave_addr);
    u8 takeSlaveAddr(void);
    void setDeviceName(QString dev_name);
    Q_INVOKABLE QString takeDeviceName(void);
    void setModelName(QString model_name);
    void setModelType(QString model_type);
    QString takeRunTime(void);
    u8 takeOnlineState(void);
    
    Q_INVOKABLE QObject *takeModelParent(void);
    Q_INVOKABLE QObject *takeSimpleParent(void);
    Q_INVOKABLE QString takeDevSnStr(void);
    Q_INVOKABLE QString takeModelName(void);
    Q_INVOKABLE QString takeRtcTimeStr(void);
    
    virtual int setRawData(u32 app_id, u32 dev_sn, u8 pack_num, u8 msg_type, u8 *msg_buff, u16 msg_len)=0;
    virtual void showModel(QObject *parent = nullptr)=0;
    virtual void hideModel(void)=0;    
    virtual void showSimple(QObject *parent = nullptr)=0;
    virtual void hideSimple(void)=0;   
    
public:
    QByteArray utf8ToGbk(QString text);
    QString gbkToUtf8(QByteArray text);

public://基本数据
    QString m_devName, m_modelName, m_modelType;
    u32 m_devSn, m_parentSn, m_appID;
    u8 m_upPackNum, m_downPackNum;
    u8 m_devState, m_softVersion, m_maxAlarmLevel;
    QString m_updateTime, m_location;
    u32 m_rtcTime, m_runTime, m_onlineTime;
    u8 m_onlineState;
    QQmlApplicationEngine *m_modelEngine, *m_simpleEngine;
    QObject *m_modelParent, *m_simpleParent;
    u8 m_slaveAddr;

public:
    QTimer *checkTimer;
    u32 m_secTickets;
    DrvCommon drv_com;

signals:
    void siqUpdateDeviceName(QString dev_name);    
signals:
    void sigSendDownMsg(u32 app_id, u32 dev_sn, u8 pack_num, u8 msg_type, u8 *msg_buff, u16 msg_len);
};

#endif // BASEMODEL_H
