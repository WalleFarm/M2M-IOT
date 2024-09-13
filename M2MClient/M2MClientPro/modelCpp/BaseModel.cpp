#include "BaseModel.h"

BaseModel::BaseModel(QObject *parent) : QObject(parent)
{
    m_appID=0;
    m_devSn=0;
    m_parentSn=0;
    m_modelEngine=nullptr;
    m_modelParent=nullptr;
    m_simpleEngine=nullptr;
    m_simpleParent=nullptr;
    m_maxAlarmLevel=0;
    m_onlineTime=0;
    m_devState=0;
    m_downPackNum=drv_com.takeRandNumber()%100+1;
    m_slaveAddr=0;
    m_runTime=0;
    m_rtcTime=0;
    m_onlineState=0;
}

void BaseModel::setDeviceSn(u32 dev_sn)
{
    m_devSn=dev_sn;
}

u32 BaseModel::takeDeviceSn(void)
{
    return m_devSn;
}

void BaseModel::setAppID(u32 app_id)
{
    m_appID=app_id;
}

u32 BaseModel::takeAppID(void)
{
    return m_appID;
}

void BaseModel::setSlaveAddr(u8 slave_addr)
{
    m_slaveAddr=slave_addr;
}

u8 BaseModel::takeSlaveAddr(void)
{
    return m_slaveAddr;
}

void BaseModel::setDeviceName(QString dev_name)
{
    m_devName=dev_name;
    emit siqUpdateDeviceName(dev_name);
}

QString BaseModel::takeDeviceName(void)
{
    return m_devName;
}

void BaseModel::setModelName(QString model_name)
{
    m_modelName=model_name;
}

void BaseModel::setModelType(QString model_type)
{
    m_modelType=model_type;
}

QByteArray BaseModel::utf8ToGbk(QString text)
{
    QTextCodec *codec=QTextCodec::codecForName("GBK");
    QByteArray ba_text=codec->fromUnicode(text);
//    qDebug()<<"ba_text="<<ba_text.toHex();
    return ba_text; 
}

QString BaseModel::gbkToUtf8(QByteArray text)
{
    QTextCodec *gbk=QTextCodec::codecForName("GBK");
    QString str_gbk=gbk->toUnicode(text);
    return str_gbk;
}


QString BaseModel::takeRunTime(void)
{
    return QString::asprintf("%u天%02u时%02u分%02u秒", m_runTime/86400,m_runTime%86400/3600, m_runTime%3600/60, m_runTime%60);
}

u8 BaseModel::takeOnlineState(void)
{
    return m_onlineState;
}


QObject *BaseModel::takeModelParent(void)
{
    return m_modelParent;
}

QObject *BaseModel::takeSimpleParent(void)
{
    return m_simpleParent;
}

QString BaseModel::takeDevSnStr(void)
{
    return QString::asprintf("%08X", m_devSn);
}

QString BaseModel::takeModelName(void)
{
    return m_modelName;
}


QString BaseModel::takeRtcTimeStr(void)
{
    u32 now_time=m_rtcTime+3600*8;
    now_time=now_time%86400;
    return QString::asprintf("%02u:%02u:%02u", now_time/3600, now_time%3600/60, now_time%60);
}






























