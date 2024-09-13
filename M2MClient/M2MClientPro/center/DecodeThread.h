#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include <QObject>
#include "QZXing"
#include<QPixmap>
#include "drivers/DrvCommon.h"
class DecodeThread : public QObject
{
    Q_OBJECT
public:
    explicit DecodeThread(QObject *parent = nullptr);
    
private:
    QZXing *m_scanQrDecode;
    QTimer *checkTimer;
signals:
    void sigDecodeResult(QString result_str);
public slots:
    void slotCheckTimeout(void);
    void slotDecodeImage(QImage img);
};

#endif // DECODETHREAD_H
