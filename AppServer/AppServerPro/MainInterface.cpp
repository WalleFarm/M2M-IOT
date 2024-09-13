#include "MainInterface.h"

MainInterface::MainInterface(QObject *parent) : QObject(parent)
{
    qDebug("App Server Start!");
    
    m_timerCounts=m_secCounts=0;
    m_mqttClient=nullptr;
    
    
    checkTimer = new QTimer();
    checkTimer->setInterval(500);
    checkTimer->start();
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout()));
    
    m_mqttClient=new BaseMqtt();
    connect(m_mqttClient, SIGNAL(sigtMqttReceived(QMQTT::Message)), this, SLOT(slotMqttReceived(QMQTT::Message)));
    m_mqttClient->mqttConnect("broker.emqx.io", 1883, "usr_as", "123465789", QString::asprintf("AppServer_%d", drv_com.takeRandNumber()));
    m_mqttClient->mqttAddTopic(QString(TOPIC_HEAD)+"as/sub/#", 0);//服务类话题
    
    //账户相关线程
    if(1)
    {
        QThread *tmp_thread = new QThread();
        AccountThread *account_thread = new AccountThread();
        account_thread->moveToThread(tmp_thread);
        tmp_thread->start(QThread::NormalPriority);
        connect(this, SIGNAL(sigAccountThreadMessage(QString, QJsonObject)), account_thread, SLOT(slotAccountThreadMessage(QString, QJsonObject)));
        connect(account_thread, SIGNAL(sigMqttPushMessage(QString, QByteArray)), this, SLOT(slotMqttPushMessage(QString, QByteArray)));      
    }

    if(1)//控制中心线程
    {
        QThread *tmp_thread = new QThread();
        CenterThread *center_thread = new CenterThread();
        center_thread->moveToThread(tmp_thread);
        tmp_thread->start(QThread::NormalPriority);
        connect(this, SIGNAL(sigCenterThreadMessage(QString,QJsonObject)), center_thread, SLOT(slotCenterThreadMessage(QString,QJsonObject)));
        connect(center_thread, SIGNAL(sigMqttPushMessage(QString, QByteArray)), this, SLOT(slotMqttPushMessage(QString, QByteArray)));      
    }
     
    
}


void MainInterface::slotCheckTimeout(void)
{
    if(m_timerCounts%2==0)
    {
        m_secCounts++;
        
    }
    
}


void MainInterface::slotMqttReceived(const QMQTT::Message &message)
{
    QJsonParseError json_error;
    QJsonDocument json_doc;
    QString recv_topic=message.topic();
     qDebug()<<"msg topic= "<<message.topic();
     qDebug()<<message.payload().data();
    json_doc = QJsonDocument::fromJson(message.payload(), &json_error);//转为JSON格式
    if(json_error.error != QJsonParseError::NoError)
    {
//        qDebug()<<"json error= "<<json_error.error;
        return;
    }
    QJsonObject rootObj = json_doc.object();
    
    if(recv_topic.contains("account/"))//账户类相关的话题
    {
        emit sigAccountThreadMessage(recv_topic, rootObj);
    }
    else if(recv_topic.contains("center/"))//控制中心话题
    {
        emit sigCenterThreadMessage(recv_topic, rootObj);
    }
    
}

void MainInterface::slotMqttPushMessage(QString topic, QByteArray msg_ba)
{
    if(m_mqttClient && m_mqttClient->mqttIsConnected())
    {
        m_mqttClient->mqttPublishMessage(topic, msg_ba);
    }
}







