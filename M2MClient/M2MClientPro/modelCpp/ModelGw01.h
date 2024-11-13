#ifndef MODELGW01_H
#define MODELGW01_H

#include <QObject>
 #include"BaseModel.h"

#define     NODE01_DATA_LIST_SIZE       1000

class ModelGw01 : public BaseModel
{
    Q_OBJECT
    
    typedef enum
    {
        GW01_CMD_DATA=0,//数据包
        
        GW01_CMD_NODE_DATA=10,//节点数据,类似于透传
        GW01_CMD_NODE_LIST,//该网关下的节点列表
        
        GW01_CMD_NODE_DOWN=128,//节点下行数据
        
    }Gw01Cmd;
    
    typedef enum
    {
      NwkMasterDownResultAddOK=0,//添加成功
      NwkMasterDownResultFull, //缓冲区已满
      NwkMasterDownResultErrSn, //设备不存在
      NwkMasterDownResultTimeOut,//超时,失败
      NwkMasterDownResultSuccess,//下发成功
      
      NwkMasterDownResultErrUnknow=100,//未知错误
    }NwkMasterDownResult;
    
    typedef struct
    {
        int16_t rssi;
        int8_t snr;
        u8 up_pack_num;
        u16 temp;
        u16 pack_id;
        u32 time_stamp;//接收时间戳
        u32 freq;
        u8 sf;
        u8 bw;
    }Node01DataUnionStruct;
    
    typedef struct
    {
        u32 node_sn;
        QList<Node01DataUnionStruct> data_list;
    }Node01DataListStruct;
    
public:
    explicit ModelGw01(QObject *parent = nullptr);
    virtual void showModel(QObject *parent = nullptr);//显示完整模型
    virtual void hideModel(void);//隐藏完整模型
    virtual void showSimple(QObject *parent = nullptr);//显示简略模型
    virtual void hideSimple(void);  //隐藏简略模型
    virtual  int setRawData(u32 app_id, u32 dev_sn, u8 pack_num, u8 msg_type, u8 *msg_buff, u16 msg_len);//设置上行数据
    static QByteArray takeModelPassword(u8 index);//获取通讯密码
    
    Node01DataListStruct *addNode01(u32 node_sn);
    Node01DataListStruct *findNode01(u32 node_sn);
    void addNode01DataUnion(u32 node_sn, Node01DataUnionStruct data_union);
    QString makeNode01HistoryDataStr(int index, Node01DataUnionStruct data_union);
    Q_INVOKABLE void updateNode01History(QString node_str);
    Q_INVOKABLE QString exportNode01History(QString node_str);
    Q_INVOKABLE void setNode01DownMsg(QString node_str);
    void setGwTime(u32 time);
private:
    QList<Node01DataListStruct> m_node01DataList;
    
signals:
    void siqUpdateOnlineState(int state);
    void siqUpdateNode01Data(QString sn_str, QString rssi_str, QString snr_str, QString data_str);
    void siqAddNode01History(int id,  QString data_str);
    void siqShowMsg(QString msg);
public slots:
    void slotCheckTimeout(void);//定时函数
    
};

#endif // MODELGW01_H
