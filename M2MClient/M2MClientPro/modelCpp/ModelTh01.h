#ifndef MODELTH01_H
#define MODELTH01_H

#include <QObject>
#include"BaseModel.h"

class ModelTh01 : public BaseModel
{
    Q_OBJECT
public:
    typedef enum
    {
        TH01_CMD_DATA=0,//数据包
        TH01_CMD_THRESH,//阈值
        
        
        TH01_CMD_SET_TEMP=128,//设置温度阈值
        TH01_CMD_SET_HUMI,//设置湿度阈值
    }Th01Cmd;
    
public:
    explicit ModelTh01(QObject *parent = nullptr);
    
    virtual void showModel(QObject *parent = nullptr);//显示完整模型
    virtual void hideModel(void);//隐藏完整模型
    virtual void showSimple(QObject *parent = nullptr);//显示简略模型
    virtual void hideSimple(void);  //隐藏简略模型
    virtual  int setRawData(u32 app_id, u32 dev_sn, u8 pack_num, u8 msg_type, u8 *msg_buff, u16 msg_len);//设置上行数据
    static QByteArray takeModelPassword(u8 index);//获取通讯密码
    
    Q_INVOKABLE void requestThreshValue(void);
    Q_INVOKABLE QString setTempThresh(QString temp_str);
    Q_INVOKABLE QString setHumiThresh(QString humi_str);
    
signals:
    void siqUpdateSensorValues(QString temp, QString humi, int temp_alarm, int humi_alarm);//状态数据
    void siqUpdateThresh(QString temp, QString humi);//阈值数据
    void siqUpdateOnlineState(int state);
public slots:
    void slotCheckTimeout(void);//定时函数
    
};

#endif // MODELTH01_H
