#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include<QQuickItemGrabResult>
#include<QQuickItem>
#include<QPixmap>

#include "drivers/DrvCommon.h"
#include "mqtt/BaseMqtt.h"
#include "center/AccountMan.h"
#include "center/CenterMan.h"
#include "center/DecodeThread.h"
#include "user_opt.h"

class MainInterface : public QObject
{
    Q_OBJECT
public:
    explicit MainInterface(QObject *parent = nullptr);
    Q_INVOKABLE void setQrImage(QObject *object);
    
private:
    DrvCommon drv_com;
    QTimer *checkTimer;
    u32 m_timerCounts, m_secCounts;
    BaseMqtt *m_mqttClient;
    int m_randNum;
    QString m_macStr;
    QQmlApplicationEngine m_qmlEngine;
    
    AccountMan m_accountMan;
    CenterMan m_centerMan;
    DecodeThread *m_decodeThread;
    bool m_bisGrab;
    QSharedPointer<QQuickItemGrabResult> m_grabResult;
    QImage m_qrImage;
    
    u32 m_currWorkAppID;
signals:
    void sigDecodeImage(QImage img);
signals:
    void siqDecodeSuccess(QString result);
    void siqUpdateNetState(int state);
public slots:
    void slotCheckTimeout(void);
    void slotMqttReceived(const QMQTT::Message& message);
    void slotMqttPushMessage(QString topic, QByteArray msg_ba);
    void slotGrabReady(void);
    void slotDecodeResult(QString result_str);
    
};

#endif // MAININTERFACE_H
