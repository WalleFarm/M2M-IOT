#ifndef DRVCOMMON_H
#define DRVCOMMON_H
#include <QObject>
#include<QDebug>
#include<QFile>
#include<QFileInfo>
#include<QDataStream>
#include <QUuid>
#include <QThread>
#include<QDir>
#include<QDateTime>
#include<QTimer>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>

#include <QMetaType>
#include<QNetworkInterface>

#include <QHostAddress>
#include <QUdpSocket>
#include "encrypt/mbedtls/aes.h"

#define		RAND_TAB_LEN	20

typedef unsigned char u8;      /* 8 bit unsigned */
typedef unsigned short u16;    /* 16 bit unsigned */
typedef unsigned int u32;      /* 32 bit unsigned */

class DrvCommon
{
private:
    u16 m_currRandCrc;
    QUdpSocket m_debugUdp;
public:
    DrvCommon();
    quint16 crc16(quint8 *puchMsg,quint16 usDataLen) ;
    quint8 *memstr(quint8 *full_str, quint16 full_len, quint8 *sub_str, quint16 sub_len);
    void EncryptTEA(u32 *firstChunk, u32 *secondChunk, u32* key);
    void DecryptTEA(u32 *firstChunk, u32 *secondChunk, u32* key);
    u16 TEA_EncryptBuffer(u8 *buff, u16 len, u32* key);
    u16 TEA_DecryptBuffer(u8 *buff, u16 len, u32* key);

    u16 rand_Encrypt(u8 rand_num, u8 *inBuff, u16 inLen, u8 *outBuff);
    u16 rand_Decrypt(u8 *inBuff, u16 inLen, u8 *outBuff);
    void tea_test(void);
    void rand_test(void);

    void tea_key_copy(u8 *tea_key);
    int aes_encrypt_buff(u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size, u8 *passwd);
    int aes_decrypt_buff(u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size, u8 *passwd);
//    void creat_rand_buff(void);
    void get_rand_key(u8 start, u8 step, u8 *key_buff);
    u16 rand_encrypt_buff(u8 *inBuff, u16 inLen, u8 *outBuff);
    u16 rand_decrypt_buff(u8 *inBuff, u16 inLen, u8 *outBuff);

    void passwdEncrypt(QString pwd_str, u8 start, u8 step);
    QByteArray passwdDecrypt(u8 *in_buff, u8 start, u8 step);
    
    int takeRandNumber(void);
    QString takeHostMac(void);
    QString takeRandMac(void);
    void createRootFile(QString file_name);
    void createMutiFile(QString path);
    void udpDebug(QString msg_str);
    QJsonObject readConfg(QString path, u8 *key_buff=nullptr);
    int writeConfig(QString path, QJsonObject root_obj, u8 *key_buff=nullptr);
    QString takeUUID(void);
};

#endif // DRVCOMMON_H
