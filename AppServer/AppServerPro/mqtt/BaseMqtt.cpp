#include "BaseMqtt.h"


BaseMqtt::BaseMqtt(QObject *parent) : QObject(parent)
{
    isConnected=false;
    checkTimer = new QTimer(this);
    checkTimer->setInterval(1*100);//心跳检测
    checkTimer->start();
    m_heartTime=0;
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout()));

    m_mqttClient=nullptr;
    m_hostAddress="";
}
BaseMqtt::~BaseMqtt(void)
{
    qDebug()<<"~BaseMqtt, hostName="<<m_connectParam.hostName;
}

void BaseMqtt::slotLookUpHost(QHostInfo info)
{
   if(info.error() == QHostInfo::NoError)
   {
       foreach (QHostAddress address, info.addresses())
       {
            m_hostAddress=address.toString();
            qDebug()<<m_connectParam.hostName<<" mqtt found ip= "<<m_hostAddress;
            break;
       }
   }
}

void BaseMqtt::slotCheckTimeout(void)
{
    m_heartTime++;
    if(m_hostAddress.isEmpty())
    {
        if(m_heartTime%10==0)
        {
            qDebug("mqtt start dns!");
            QHostInfo::lookupHost(m_connectParam.hostName, this, SLOT(slotLookUpHost(QHostInfo)));
        }
        return;
    }
    if(m_mqttClient==nullptr)
    {
        if(!m_connectParam.certPath.isEmpty())//使用SSL连接
        {
            QFile file;
            QByteArray client_key_text, client_crt_text, root_crt_text;
            QString certPath=m_connectParam.certPath;
            QSslSocket ssl_socket;
            
            file.setFileName(certPath+"/client.key");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            client_key_text = file.readAll();
            file.close();
            
            file.setFileName(certPath+"/client.crt");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            client_crt_text = file.readAll();
            file.close();

            file.setFileName(certPath+"/rootCA.crt");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            root_crt_text = file.readAll();
            file.close();
            
            QSslKey client_key(client_key_text, QSsl::Rsa);
            QSslCertificate client_crt(client_crt_text);            

            ssl_socket.setPrivateKey(client_key);
            ssl_socket.setLocalCertificate(client_crt);
            
            QSslConfiguration ssl_config=QSslConfiguration::defaultConfiguration();
            ssl_config.setCaCertificates(QSslCertificate::fromData(root_crt_text)); //QSslCertificate::fromPath(certPath+"/rootCA.crt"); 
            ssl_config.setPrivateKey(ssl_socket.privateKey());
            ssl_config.setLocalCertificate(ssl_socket.localCertificate());
            ssl_config.setPeerVerifyMode(QSslSocket::QueryPeer);
            ssl_config.setPeerVerifyDepth(10);
            ssl_config.setProtocol(QSsl::TlsV1_2);

            m_mqttClient = new QMQTT::Client(m_hostAddress, m_connectParam.hostPort, ssl_config, true, this);
//            qDebug()<<"\n###SSL PrivateKey="<<ssl_config.privateKey();
//            qDebug()<<"###SSL Certificate="<<ssl_config.localCertificate();
//            qDebug()<<"###SSL rootCA="<<ssl_config.caCertificates();
//            qDebug()<<"hostName="<<m_hostAddress<<", hostPort="<<m_connectParam.hostPort;
//            qDebug()<<"userName="<<m_connectParam.userName<<", passWord="<<m_connectParam.passWord<<", clientID="<<m_connectParam.clientID;
        }
        else//普通连接
        {
            QHostAddress host(m_hostAddress);
            m_mqttClient = new QMQTT::Client(host, m_connectParam.hostPort, this);
        }
        signalSlotInit();
        m_mqttClient->setUsername(m_connectParam.userName);
        m_mqttClient->setPassword(m_connectParam.passWord);
        m_mqttClient->setClientId(m_connectParam.clientID);
        m_mqttClient->setAutoReconnect(true);
        m_mqttClient->setCleanSession(true);
        m_mqttClient->setKeepAlive(30);
        m_mqttClient->connectToHost();
    }
    else if(this->mqttIsConnected())
    {
        for(auto iter : m_subTopicList)//订阅话题
        {
            if(iter.isSubed==false)
            {
                mqttSubscribeMessage(iter.subTopic, iter.qos);
                break;
            }
        }
        if(m_heartTime%200==0)//保持连接
        {
//            qDebug()<<"mqtt send keep";
            mqttPublishMessage("dev/sub/data1", QByteArray("heart"));
        }
    }
    else
    {
//        qDebug()<<"BaseMqtt no connected!";
    }
}

void BaseMqtt::mqttConnect(QString hostName, u16 hostPort, QString userName, QByteArray passWord, QString clientID, QString certPath)
{
    clientID=clientID+QString("_")+takeHostMac().remove(":");
    m_connectParam.hostName=hostName;
    m_connectParam.hostPort=hostPort;
    m_connectParam.userName=userName;
    m_connectParam.passWord=passWord;
    m_connectParam.clientID=clientID;
    m_connectParam.certPath=certPath;

    u8 *pData=(u8*)hostName.toUtf8().data();
    if(pData[0]>='0' && pData[0]<='9')//判断是否为域名,使用域名时 域名的第一个字符不能是数字
    {
        m_hostAddress=hostName;
    }
}

bool BaseMqtt::mqttIsConnected(void)
{
//    if(m_mqttClient!=nullptr)
//        return m_mqttClient->isConnectedToHost();
    return isConnected;
}

void BaseMqtt::mqttPublishMessage(QString topicFilter, QByteArray msgBa)
{
    if(m_mqttClient==nullptr || m_mqttClient->isConnectedToHost()==false)
       return;
    QMQTT::Message message;
    message.setTopic(topicFilter);
    message.setPayload(msgBa);
    m_mqttClient->publish(message);
}

void BaseMqtt::mqttPingresp(void)
{
//    m_mqttClient->pingresp();
}

void BaseMqtt::mqttSubscribeMessage(QString topicFilter, quint8 qos)
{
    if(m_mqttClient==nullptr)
       return;
    m_mqttClient->subscribe(topicFilter, qos);
}

void BaseMqtt::mqttUnsubscribeMessage(QString topicFilter)
{
    if(m_mqttClient==nullptr)
       return;
    m_mqttClient->unsubscribe(topicFilter);
}

void BaseMqtt::signalSlotInit(void)
{
    connect(m_mqttClient, SIGNAL(connected()), this, SLOT(slotMqttConnected()));
    connect(m_mqttClient, SIGNAL(disconnected()), this, SLOT(slotMqttDisconnected()));
    connect(m_mqttClient, SIGNAL(error(QMQTT::ClientError)), this, SLOT(slotMqttError(QMQTT::ClientError)));
    connect(m_mqttClient, SIGNAL(pingresp()), this, SLOT(slotMqttPingresp()));
    connect(m_mqttClient, SIGNAL(published(QMQTT::Message,quint16)), this, SLOT(slotMqttPuslished(QMQTT::Message,quint16)));
    connect(m_mqttClient, SIGNAL(received(QMQTT::Message)), this, SLOT(slotMqttReceived(QMQTT::Message)));
    connect(m_mqttClient, SIGNAL(subscribed(QString,quint8)), this, SLOT(slotMqttSubscribed(QString,quint8)));
    connect(m_mqttClient, SIGNAL(unsubscribed(QString)), this, SLOT(slotMqttUnsubscribed(QString)));
}

void BaseMqtt::mqttAddTopic(QString topic, u8 qos)
{
    for(auto iter : m_subTopicList)
    {
        if(iter.subTopic==topic)
        {
            qDebug()<<"have the same topic="<<topic;
            return;
        }
    }
    SubTopicStruct tag_subTopic;
    tag_subTopic.subTopic=topic;
    tag_subTopic.isSubed=false;
    tag_subTopic.qos=qos;
    m_subTopicList.append(tag_subTopic);
    qDebug()<<"mqttAddTopic="<<topic;
}

void BaseMqtt::mqttDelTopic(QString topic)
{
    int i=0;
    for(auto iter : m_subTopicList)
    {
        if(topic==iter.subTopic)
        {
            if(iter.isSubed==true)
            {
                this->mqttUnsubscribeMessage(topic);
            }
            m_subTopicList.removeAt(i);
            qDebug()<<"remove topic="<<topic;
            return;
        }
        i++;
    }
}

void BaseMqtt::slotMqttConnected(void)
{
    isConnected=true;
    emit sigMqttConnected();
    qDebug()<<"clientId="<<m_mqttClient->clientId()<<"connected";
}

void BaseMqtt::slotMqttDisconnected(void)
{
    isConnected=false;
    int nSize=m_subTopicList.size();
    for(int i=0; i<nSize; i++)
    {
        m_subTopicList[i].isSubed=false;
    }
    qDebug()<<"clientId="<<m_mqttClient->clientId()<<"disconnected";
    emit sigMqttDisconnected();
}

void BaseMqtt::slotMqttError(const QMQTT::ClientError error)
{
    qDebug()<<"clientId="<<m_mqttClient->clientId()<<"slotMqttError="<<error;
}

void BaseMqtt::slotMqttPingresp(void)
{
//    qDebug("BaseMqtt::slotMqttPingresp");
}

void BaseMqtt::slotMqttPuslished(const QMQTT::Message &message, quint16 msgid)
{
    msgid=message.id();
    msgid=msgid;
//    qDebug("BaseMqtt::slotMqttPuslished, msgid=%d ", msgid);
//    qDebug()<<"msg="<<message.payload().toHex();
}

void BaseMqtt::slotMqttReceived(const QMQTT::Message &message)
{
    emit sigtMqttReceived(message);

}

void BaseMqtt::slotMqttSubscribed(const QString &topic, const quint8 qos)
{
    int i=0;
//    qDebug()<<"slotMqttSubscribed, topic="<<topic<<", qos="<<qos;
    for(auto iter : m_subTopicList)
    {
        if(iter.subTopic==topic)
        {
            m_subTopicList[i].isSubed=true;
            break;
        }
        i++;
    }
    emit sigMqttSubscribed(topic, qos);
}

void BaseMqtt::slotMqttUnsubscribed(const QString &topic)
{
    int i=0;
    for(auto iter : m_subTopicList)
    {
        if(iter.subTopic==topic)
        {
            m_subTopicList[i].isSubed=false;
            break;
        }
        i++;
    }
    emit sigMqttUnsubscribed(topic);
}

QString BaseMqtt::takeHostMac(void)
{
    DrvCommon drv_com;    
    return drv_com.takeRandMac();
}






