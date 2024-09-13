#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <QObject>
#include "drivers/DrvCommon.h"
#include "mqtt/BaseMqtt.h"
#include "thread/AccountThread.h"
#include "thread/CenterThread.h"


class MainInterface : public QObject
{
    Q_OBJECT
public:
    explicit MainInterface(QObject *parent = nullptr);
    
    
private:
    DrvCommon drv_com;
    QTimer *checkTimer;
    u32 m_timerCounts, m_secCounts;
    BaseMqtt *m_mqttClient;

signals:
    void sigAccountThreadMessage(QString topic, QJsonObject root_obj);
    void sigCenterThreadMessage(QString topic, QJsonObject root_obj);
    
public slots:
    void slotCheckTimeout(void);
    void slotMqttReceived(const QMQTT::Message& message);
    
    void slotMqttPushMessage(QString topic, QByteArray msg_ba);
};

#endif // MAININTERFACE_H
