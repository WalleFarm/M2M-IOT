#ifndef BASEMQTT_H
#define BASEMQTT_H

#include <QObject>
#include "drivers/DrvCommon.h"
#include "mqtt/qmqtt.h"
#include "mqtt/qmqtt_client.h"
#include<QNetworkInterface>
#include<QSslSocket>
#include<QSslConfiguration>
#include<QSslKey>
#include<QTimer>
#include<QHostInfo>
class BaseMqtt : public QObject
{
    Q_OBJECT
public:
    typedef struct
    {
        QString subTopic;
        bool isSubed;
        u8 qos;
    }SubTopicStruct;
    typedef struct
    {
       QString hostName;
       u16 hostPort;
       QString userName;
       QByteArray passWord;
       QString clientID;
       QString certPath;
    }ConnectParamStruct;
public:
    explicit BaseMqtt(QObject *parent = nullptr);
    ~BaseMqtt(void);
    void signalSlotInit(void);
    void mqttConnect(QString hostName, u16 hostPort, QString userName, QByteArray passWord, QString clientID, QString certPath="");
    void mqttPublishMessage(QString topicFilter, QByteArray msgBa);
    void mqttSubscribeMessage(QString topicFilter, quint8 qos);
    void mqttUnsubscribeMessage(QString topicFilter);
    bool mqttIsConnected(void);
    void mqttPingresp(void);
    void mqttAddTopic(QString topic, u8 qos);
    void mqttDelTopic(QString topic);

public:
    QString takeHostMac(void);

private:
    QString m_hostAddress;
    QMQTT::Client  *m_mqttClient;
    QList<SubTopicStruct> m_subTopicList;
    QTimer *checkTimer;
    bool isConnected;
    u32 m_heartTime;
    ConnectParamStruct m_connectParam;
signals:
    void sigMqttConnected(void);
    void sigMqttDisconnected(void);
    void sigMqttSubscribed(const QString& topic, const quint8 qos);
    void sigMqttUnsubscribed(const QString& topic);
    void sigtMqttReceived(const QMQTT::Message& message);
    void sigMqttError(const QMQTT::ClientError error);

public slots:
    void slotCheckTimeout(void);

public slots:
    void slotLookUpHost(QHostInfo info);
    void slotMqttConnected(void);
    void slotMqttDisconnected(void);
    void slotMqttPuslished(const QMQTT::Message& message, quint16 msgid);
    void slotMqttSubscribed(const QString& topic, const quint8 qos);
    void slotMqttUnsubscribed(const QString& topic);
    void slotMqttError(const QMQTT::ClientError error);
    void slotMqttReceived(const QMQTT::Message& message);
    void slotMqttPingresp(void);
};

#endif // BASEMQTT_H
