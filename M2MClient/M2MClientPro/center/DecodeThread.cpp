#include "DecodeThread.h"

DecodeThread::DecodeThread(QObject *parent) : QObject(parent)
{
    m_scanQrDecode=nullptr;            
    checkTimer = new QTimer(this);
    checkTimer->setInterval(1000);
    checkTimer->start();
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout()));
}

void DecodeThread::slotCheckTimeout(void)
{
    if(m_scanQrDecode==nullptr)
    {
        m_scanQrDecode = new QZXing();
        m_scanQrDecode->setDecoder(QZXing::DecoderFormat_QR_CODE | QZXing::DecoderFormat_CODE_128);
    }
}

void DecodeThread::slotDecodeImage(QImage img)
{
    QSize img_size=img.size();
    int width=img_size.width()*0.8;
    if(width>200)
        width=200;
    int height=width;
    int x=(img_size.width()-width)/2;
    int y=(img_size.height()-height)/2;

    QImage resize_img=img.copy(x, y, width, height);
    
//    QPixmap mainmap=QPixmap::fromImage(resize_img);
//    bool ok=mainmap.save("qr_img.png", "PNG");
//    qDebug()<<"write state="<<ok; 
    QString result_str=m_scanQrDecode->decodeImage(resize_img);
    emit sigDecodeResult(result_str);
}
