#ifndef MODELAP01_H
#define MODELAP01_H

#include <QObject>
#include"BaseModel.h"
class ModelAp01 : public BaseModel
{
    Q_OBJECT
public:
    typedef enum
    {
      AP01_CMD_HEART=0,//心跳包
      AP01_CMD_DATA,//数据包
      
      AP01_CMD_SET_SPEED=100,//设置风扇转速
      AP01_CMD_SET_SWITCH, //设置开关
    }Ap01Cmd;
    
public:
    explicit ModelAp01(QObject *parent = nullptr);
    virtual void showModel(QObject *parent = nullptr);
    virtual void hideModel(void);
    virtual void showSimple(QObject *parent = nullptr);
    virtual void hideSimple(void);  
    virtual  int setRawData(u32 app_id, u32 dev_sn, u8 pack_num, u8 msg_type, u8 *msg_buff, u16 msg_len);
    static QByteArray takeModelPassword(u8 index);
    
    Q_INVOKABLE void setSwitchState(int state);
    Q_INVOKABLE void setFanSpeed(float rate);
    

    
private:

signals:
    void siqUpdateSensorValues(QString dev_sn, QString temp, QString humi, QString pm25);
    void siqUpdateAlarmLevel(int alarm_level);
    void siqUpdateSwitchState(int state);
    void siqUpdateOnlineState(int state);
    
public slots:
    void slotCheckTimeout(void);
};

#endif // MODELAP01_H
