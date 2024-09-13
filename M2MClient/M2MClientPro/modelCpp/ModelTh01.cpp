#include "ModelTh01.h"

ModelTh01::ModelTh01(QObject *parent) : BaseModel(parent)
{
    m_secTickets=0;
    checkTimer = new QTimer(this);
    checkTimer->setInterval(1*1000);//心跳检测
    checkTimer->start(); 
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout()));
}


void ModelTh01::slotCheckTimeout(void)
{
        m_secTickets++;
        if(m_onlineState>0)//离线检测
        {
            if(m_secTickets - m_onlineTime>40)
            {
                m_onlineState=DEV_STATE_OFF_LINE;       
            }
            else
            {
                m_onlineState=DEV_STATE_ON_LINE;
            }
            emit siqUpdateOnlineState(m_onlineState);   
        }

}

QByteArray ModelTh01::takeModelPassword(u8 index)
{
    static u8 passwd_table[5][16]={
        0x9D, 0x53, 0x09, 0xBF, 0x75, 0x28, 0xDE, 0x94, 0x4A, 0xFD, 0xB3, 0x69, 0x1F, 0xD2, 0x88, 0x3E, 
        0xF4, 0xAA, 0x5D, 0x13, 0xC9, 0x7F, 0x31, 0xE7, 0x9D, 0x53, 0x06, 0xBC, 0x72, 0x28, 0xDB, 0x91,
        0x47, 0xFD, 0xB3, 0x66, 0x1C, 0xD2, 0x88, 0x3B, 0xF1, 0xB5, 0x75, 0x39, 0xFA, 0xBE, 0x7E, 0x42,
        0x03, 0xC7, 0x88, 0x4B, 0x0C, 0xD0, 0x91, 0x54, 0x15, 0xD9, 0x9A, 0x5E, 0x21, 0xE2, 0xA6, 0x67, 
        0x2A, 0xEB, 0xAF, 0x70, 0x34, 0xF4, 0xB8, 0x79, 0x3D, 0xFD, 0xC1, 0x82, 0x46, 0x06, 0xCA, 0x8B, 
    };
    if(index>=5)index=0;
    
    QByteArray ba;
    ba.setRawData((char*)&passwd_table[index][0], 16);
    return ba;
    
}


void ModelTh01::showModel(QObject *parent)
{
    hideModel();
    m_modelParent=parent; 
    if(m_modelEngine==nullptr)
    {
        m_modelEngine=new QQmlApplicationEngine(this);
        m_modelEngine->rootContext()->setContextProperty("theModelTh01", this);
        m_modelEngine->rootContext()->setContextProperty("theCenterMan", this->parent());
    }
    m_modelEngine->load("qrc:/qmlRC/modelQml/TH01/ModelTh01.qml");
}

void ModelTh01::hideModel(void)
{
    if(m_modelEngine)
        delete m_modelEngine;
    m_modelEngine=nullptr;
    m_modelParent=nullptr;
}


void ModelTh01::showSimple(QObject *parent)
{
    m_simpleParent=parent; 
    if(m_simpleEngine==nullptr)
    {
        m_simpleEngine=new QQmlApplicationEngine(this);
        m_simpleEngine->rootContext()->setContextProperty("theModelTh01", this);
        m_simpleEngine->rootContext()->setContextProperty("theCenterMan", this->parent());
    }
    m_simpleEngine->load("qrc:/qmlRC/modelQml/TH01/SimpleTh01.qml");
}

void ModelTh01::hideSimple(void)
{
    if(m_simpleEngine)
        delete m_simpleEngine;
    m_simpleEngine=nullptr;
    m_simpleParent=nullptr;
}

int ModelTh01::setRawData(u32 app_id, u32 dev_sn, u8 pack_num, u8 msg_type, u8 *msg_buff, u16 msg_len)
{
    if(dev_sn!=m_devSn)
        return 0;
    
    u8 *pData=msg_buff;
    msg_len=msg_len;
    if(m_upPackNum==pack_num)
        return 0;
    m_upPackNum=pack_num;
    m_appID=app_id;
//    qDebug()<<"msg_type="<<msg_type;
    switch(msg_type)
    {
        case TH01_CMD_DATA://状态数据
        {
            int temp=pData[0]<<8|pData[1];//温度 原始数据
            float temp_f=(temp-1000)/10.f;//温度浮点数据
            pData+=2;
            int humi=pData[0]<<8|pData[1];
            float humi_f=humi/10.f;
            pData+=2;
            u8 alarm_type=pData[0];
            pData+=1;
            
            QString temp_str=QString::asprintf("%.1f", temp_f);
            QString humi_str=QString::asprintf("%.1f", humi_f);
            emit siqUpdateSensorValues(temp_str, humi_str, alarm_type>>4, alarm_type&0x0F);
            break;
        }
        case TH01_CMD_THRESH://阈值数据
        {
            int temp=pData[0]<<8|pData[1];//温度 原始数据
            float temp_f=(temp-1000)/10.f;//温度浮点数据
            pData+=2;
            int humi=pData[0]<<8|pData[1];
            float humi_f=humi/10.f;
            pData+=2;
            
            QString temp_str=QString::asprintf("%.1f", temp_f);
            QString humi_str=QString::asprintf("%.1f", humi_f);
            emit siqUpdateThresh(temp_str, humi_str);
            break;
        }
    }
    QDateTime current_date_time = QDateTime::currentDateTime();
    m_updateTime=current_date_time.toString("hh:mm:ss");

    m_onlineTime=m_secTickets;
    m_onlineState=DEV_STATE_ON_LINE;
    return 0;
}

void ModelTh01::requestThreshValue(void)
{
    u8 make_buff[100]={0};
    u16 make_len=0;

    emit sigSendDownMsg(m_appID, m_devSn, m_downPackNum++, TH01_CMD_THRESH, make_buff, make_len);
}

QString ModelTh01::setTempThresh(QString temp_str)
{
    float temp_f=temp_str.toFloat();
    if(temp_f<0.f || temp_f>120.f)
    {
        return QString("输入范围有误!");
    }
    
    u16 temp_u16=temp_f*10+1000;
    u8 make_buff[100]={0};
    u16 make_len=0;
    make_buff[make_len++]=temp_u16>>8;
    make_buff[make_len++]=temp_u16;
    emit sigSendDownMsg(m_appID, m_devSn, m_downPackNum++, TH01_CMD_SET_TEMP, make_buff, make_len);
    return "";
}

QString ModelTh01::setHumiThresh(QString humi_str)
{
    float humi_f=humi_str.toFloat();
    if(humi_f<20.f || humi_f>100.f)
    {
        return QString("输入范围有误!");
    }
    
    u16 humi_u16=humi_f*10;
    u8 make_buff[100]={0};
    u16 make_len=0;
    make_buff[make_len++]=humi_u16>>8;
    make_buff[make_len++]=humi_u16;
    emit sigSendDownMsg(m_appID, m_devSn, m_downPackNum++, TH01_CMD_SET_HUMI, make_buff, make_len);
    return "";
}






