#include "MainInterface.h"

MainInterface::MainInterface(QObject *parent) : QObject(parent)
{
    qDebug("Client Start!");
    
    m_timerCounts=m_secCounts=0;
    m_mqttClient=nullptr;
    m_currWorkAppID=0;
    m_bisGrab=false;
    
    m_randNum=drv_com.takeRandNumber();
    m_macStr=drv_com.takeHostMac();
    m_accountMan.setMacInfo(m_macStr, m_randNum);
    m_centerMan.setMacInfo(m_macStr, m_randNum);
    
    checkTimer = new QTimer();
    checkTimer->setInterval(500);
    checkTimer->start();
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout()));
    
    m_mqttClient=new BaseMqtt();
    connect(m_mqttClient, SIGNAL(sigtMqttReceived(QMQTT::Message)), this, SLOT(slotMqttReceived(QMQTT::Message)));
    m_mqttClient->mqttConnect("broker.emqx.io", 1883, "usr_client", "123465789", QString::asprintf("Client_%d", drv_com.takeRandNumber()%1000));
    
    QString topic=QString(TOPIC_HEAD) +QString("as/pub/account/")+m_macStr+QString::asprintf("/%d/#", m_randNum);
    qDebug()<<topic;
    m_mqttClient->mqttAddTopic(topic, 0);//账户类话题
    topic=QString(TOPIC_HEAD) +QString("as/pub/center/")+m_macStr+QString::asprintf("/%d/#", m_randNum);
    qDebug()<<topic;
    m_mqttClient->mqttAddTopic(topic, 0);//控制中心话题
    
    const QUrl url(QStringLiteral("qrc:/qmlRC/mainQml/main.qml"));
    m_qmlEngine.rootContext()->setContextProperty("theMainInterface", this);
    m_qmlEngine.rootContext()->setContextProperty("theAccountMan", &m_accountMan);
    m_qmlEngine.rootContext()->setContextProperty("theCenterMan", &m_centerMan);
    m_qmlEngine.load(url);
    
    
    connect(&m_accountMan, SIGNAL(sigMqttPushMessage(QString, QByteArray)), this, SLOT(slotMqttPushMessage(QString, QByteArray)));
    connect(&m_centerMan, SIGNAL(sigMqttPushMessage(QString, QByteArray)), this, SLOT(slotMqttPushMessage(QString, QByteArray)));
    connect(&m_accountMan, SIGNAL(sigUpdateLoginAccount(QString, int)), &m_centerMan, SLOT(slotUpdateLoginAccount(QString, int)));
    connect(m_mqttClient, SIGNAL(sigMqttConnected()), &m_centerMan, SLOT(slotMqttConnected()));
    
    //二维码识别
    m_decodeThread = new DecodeThread();
    connect(this, SIGNAL(sigDecodeImage(QImage)), m_decodeThread, SLOT(slotDecodeImage(QImage)));
    connect(m_decodeThread, SIGNAL(sigDecodeResult(QString)), this, SLOT(slotDecodeResult(QString)));
    QThread *tmp_thread = new QThread();
    m_decodeThread->moveToThread(tmp_thread);
    tmp_thread->start();

    
}


void MainInterface::slotCheckTimeout(void)
{
    if(m_timerCounts%2==0)
    {
        m_secCounts++;
        bool net_state=m_mqttClient->mqttIsConnected();
        emit siqUpdateNetState(net_state);
        
        u32 curr_app_id=m_centerMan.takeCurrAppID();
        if(curr_app_id!=m_currWorkAppID)//APP ID切换,更改设备消息订阅话题
        {
            QString topic=QString(TOPIC_HEAD) +QString::asprintf("dev/pub/data/%d", m_currWorkAppID);
            m_mqttClient->mqttDelTopic(topic);//删除旧话题
            m_currWorkAppID=curr_app_id;
            topic=QString(TOPIC_HEAD) +QString::asprintf("dev/pub/data/%d", m_currWorkAppID);
            qDebug()<<topic;
            m_mqttClient->mqttAddTopic(topic, 0);//新应用话题   
        }
    }
    
}


void MainInterface::slotMqttReceived(const QMQTT::Message &message)
{
    QJsonParseError json_error;
    QJsonDocument json_doc;
    QString recv_topic=message.topic();
//     qDebug()<<"msg topic= "<<message.topic();
//     qDebug()<<message.payload().data();
     if(recv_topic.contains("dev/pub/data"))
     {
         m_centerMan.parseDeciceRecv(message.payload());
         return;
     }
    json_doc = QJsonDocument::fromJson(message.payload(), &json_error);//转为JSON格式
    if(json_error.error != QJsonParseError::NoError)
    {
//        qDebug()<<"json error= "<<json_error.error;
        return;
    }
    QJsonObject root_obj = json_doc.object();
    if(recv_topic.contains("/account"))
    {
        
        m_accountMan.parseAccountRecv(recv_topic, root_obj);
    }
    else if(recv_topic.contains("/center"))
    {
        m_centerMan.parseCenterRecv(recv_topic, root_obj);
    }
    
}


void MainInterface::slotMqttPushMessage(QString topic, QByteArray msg_ba)
{
    if(m_mqttClient && m_mqttClient->mqttIsConnected())
    {
        m_mqttClient->mqttPublishMessage(topic, msg_ba);
    }
}



void MainInterface::slotGrabReady(void)
{
    m_qrImage=m_grabResult.data()->image();
    emit sigDecodeImage(m_qrImage); 
    
//    QPixmap mainmap=QPixmap::fromImage(m_qrImage);
//    bool ok=mainmap.save("qr_img.png", "PNG");
//    qDebug()<<"write state="<<ok;
}

void MainInterface::slotDecodeResult(QString result_str)
{
    qDebug()<<"result_str="<<result_str;
    if(!result_str.isEmpty())
        emit siqDecodeSuccess(result_str); 
    m_bisGrab=false;
}
 
void MainInterface::setQrImage(QObject *object)
{
    if(!m_bisGrab && object)
    {
        QQuickItem *item = qobject_cast<QQuickItem*>(object);
        if(item==nullptr)
            return;
        m_grabResult=item->grabToImage();
        QQuickItemGrabResult * grabResult = m_grabResult.data();       
        if (grabResult) 
        {
             connect(grabResult, SIGNAL(ready()),this,SLOT(slotGrabReady()));
             m_bisGrab = true;
        }  
    }
}







