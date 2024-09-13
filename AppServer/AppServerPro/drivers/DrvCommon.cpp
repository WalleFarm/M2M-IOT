#include "DrvCommon.h"


static const quint8 auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

//////////////低位表///////////////////
static const quint8 auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;

//////////////随机数表///////////////////
static const quint8 rand_table[] = {
0x2E, 0x91, 0x7C, 0xFA, 0xFD, 0xC7, 0x6E, 0xF4, 0x2E, 0x91,
0x5D, 0x1F, 0x40, 0x8A, 0xC4, 0x8E, 0x67, 0x6F, 0xB5, 0xA3,
0x76, 0xB3, 0xD2, 0x4D, 0x19, 0x0D, 0x0D, 0x63, 0xBE, 0xEB,
0x3B, 0x17, 0x54, 0x83, 0x5D, 0x6D, 0x75, 0x22, 0xBB, 0x8F,
0xD4, 0x5A, 0x63, 0x40, 0xA0, 0x99, 0xBA, 0x7F, 0x8C, 0x8F,
0xE0, 0x4C, 0x36, 0xB9, 0x92, 0xD5, 0xA8, 0x3A, 0xFD, 0x30,
0x56, 0x84, 0x72, 0x82, 0x7B, 0xCF, 0xD1, 0x57, 0x18, 0xB8,
0xB1, 0xC3, 0x21, 0xE8, 0xEC, 0xEC, 0xD4, 0xE7, 0xAE, 0x26,
0x8B, 0xED, 0x15, 0x30, 0x52, 0xAA, 0x3A, 0x3F, 0xA6, 0xB6,
0xEF, 0x38, 0xDE, 0x76, 0x8B, 0x32, 0xC0, 0x8E, 0x3D, 0xE7,
0x6B, 0x49, 0xCE, 0x24, 0x6E, 0x07, 0xC2, 0xDB, 0x62, 0x10,
0xB9, 0xF1, 0x3E, 0xEB, 0x0B, 0xEC, 0x44, 0x1B, 0xF5, 0x80,
0x47, 0x40, 0x6F, 0x48, 0x1D, 0x2A, 0x52, 0x14, 0x02, 0xCB,
0xFE, 0x90, 0xC2, 0x02, 0x48, 0x41, 0x6F, 0x41, 0xAB, 0x9D,
0x38, 0xE9, 0xCF, 0x38, 0x9F, 0xE2, 0x9B, 0xE1, 0xEB, 0xE6,
0x29, 0x3E, 0xD6, 0xBF, 0xD4, 0x62, 0x5A, 0x5C, 0x37, 0xAA,
0x53, 0x5A, 0xAD, 0x69, 0x95, 0xB7, 0x44, 0xDC, 0xA3, 0xC1,
0xB9, 0xA6, 0xCC, 0xC6, 0x63, 0xE7, 0x80, 0xC7, 0xC6, 0x92,
0x50, 0xFE, 0xC9, 0x6E, 0x19, 0xBB, 0x4F, 0xF8, 0x57, 0x69,
0xA8, 0x01, 0x9C, 0xC8, 0x9F, 0xBF, 0x1C, 0x27, 0x9A, 0xC4,
};


DrvCommon::DrvCommon()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString str_data=current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    m_currRandCrc=crc16((quint8*)str_data.toLatin1().data(), str_data.toLatin1().size());
}

quint16 DrvCommon::crc16(quint8 *puchMsg, quint16 usDataLen)
{
    quint8 uchCRCHi = 0xFF ;
    quint8 uchCRCLo = 0xFF ;
    quint16 uIndex ;
    while (usDataLen--)
    {
        uIndex = uchCRCHi ^ *puchMsg++ ;
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex] ;
    }
    return (((quint16)(uchCRCHi) << 8) | uchCRCLo) ;
}

quint8 *DrvCommon::memstr(quint8 *full_str, quint16 full_len, quint8 *sub_str, quint16 sub_len)
{
    int last_pos=full_len-sub_len+1;
    quint16 i;

    if(last_pos<0)
    {
        return NULL;
    }

    for(i=0;i<last_pos;i++)
    {
        if(full_str[i]==sub_str[0])//先对比第一个字节
        {
            if( memcmp(&full_str[i], sub_str, sub_len)==0 )
            {
                return &full_str[i];
            }
        }
    }
    return NULL;
}

void DrvCommon::EncryptTEA(u32 *firstChunk, u32 *secondChunk, u32 *key)
{
    u32 y = *firstChunk;
    u32 z = *secondChunk;
    u32 sum = 0;
    u32 delta = 0x9e3779b9;

    for (int i = 0; i < 32; i++)//8轮运算(需要对应下面的解密核心函数的轮数一样)
    {
        sum += delta;
        y += ((z << 4) + key[0]) ^ (z + sum) ^ ((z >> 5) + key[1]);
        z += ((y << 4) + key[2]) ^ (y + sum) ^ ((y >> 5) + key[3]);
    }

    *firstChunk = y;
    *secondChunk = z;
}

void DrvCommon::DecryptTEA(u32 *firstChunk, u32 *secondChunk, u32 *key)
{
    u32  sum = 0;
    u32  y = *firstChunk;
    u32  z = *secondChunk;
    u32  delta = 0x9e3779b9;

    sum = delta << 5; //32轮运算，所以是2的5次方；16轮运算，所以是2的4次方；8轮运算，所以是2的3次方

    for (int i = 0; i < 32; i++) //8轮运算
    {
        z -=( (y << 4) +key[2]) ^( y +sum) ^ ((y >> 5) + key[3]);
        y -= ((z << 4) + key[0]) ^( z + sum) ^ ((z >> 5) + key[1]);
        sum -= delta;
    }

    *firstChunk = y;
    *secondChunk = z;
}

u16 DrvCommon::TEA_EncryptBuffer(u8 *buff, u16 len, u32 *key)
{
    u8 *p = buff;
    u16 i,counts;

    if(len%8!=0)
    {
        qDebug("Encrypt buff len err!\n");
        return 0;
    }
    counts=len/8;

    for(i=0;i<counts;i++)
    {
        EncryptTEA((u32 *)p, (u32 *)(p + 4), key);
        p+=8;
    }
    return len;
}

u16 DrvCommon::TEA_DecryptBuffer(u8 *buff, u16 len, u32 *key)
{
    u8 *p = buff;
    u16 i,counts;

    if(len%8!=0)
    {
        qDebug("Decryp buff len err!\n");
        return 0;
    }

    counts=len/8;

    for(i=0;i<counts;i++)
    {
        DecryptTEA((u32 *)p, (u32 *)(p + 4), key);
        p+=8;
    }
    return len;
}

void DrvCommon::tea_test(void)
{
    u8 tea_key[17]={"1234567890abcdef"};
    char TextBuff[24]={0};//文本

    strcpy(TextBuff,"YPP123456789&*");
    TEA_EncryptBuffer((u8*)TextBuff, sizeof(TextBuff), (u32*)tea_key);
    TEA_DecryptBuffer((u8*)TextBuff, sizeof(TextBuff),  (u32*)tea_key);
    qDebug("decrypt data=%s\n",TextBuff);
}

u16 DrvCommon::rand_Encrypt(u8 rand_num, u8 *inBuff, u16 inLen, u8 *outBuff)
{
    u8 randTab[RAND_TAB_LEN]={0},tmp,i;
    u16 k;

    if(rand_num==0||rand_num==0xFF)
      rand_num=0x85;

    tmp=rand_num;
    for(i=0;i<RAND_TAB_LEN;i++)
    {
      tmp*=(i+7);
      k=i;
      while(tmp==0)
      {
        tmp=k*k;
        k++;
      }
      randTab[i]=tmp;
     }

    outBuff[0]=rand_num;//第一字节存放随机数，解密时直接使用
    for(i=0,k=0;i<inLen;i++,k++)
    {
      outBuff[i+1]=inBuff[i]^rand_num;
      if(k>=RAND_TAB_LEN)
              k=0;
      outBuff[i+1]^=randTab[k];
    }
    return inLen+1;
}

u16 DrvCommon::rand_Decrypt(u8 *inBuff, u16 inLen, u8 *outBuff)
{
    u8 rand_num,randTab[RAND_TAB_LEN]={0},tmp;
    u16 i,k;

    rand_num=inBuff[0];
    if(rand_num==0||rand_num==0xFF)
      rand_num=0x85;

    tmp=rand_num;
    for(i=0;i<RAND_TAB_LEN;i++)
    {
      tmp*=(i+7);
      k=i;
      while(tmp==0)
      {
        tmp=k*k;
        k++;
      }
      randTab[i]=tmp;
     }

    for(i=1,k=0;i<inLen;i++,k++)
    {
      if(k>=RAND_TAB_LEN)
              k=0;
      inBuff[i]^=randTab[k];
      outBuff[i-1]=inBuff[i]^rand_num;
    }
    return inLen-1;
}


void DrvCommon::rand_test(void)
{
    u8 test_buff[]={"fmskfe5hhf6d6%^"};
    u8 out_buff[100]={0};
    u16 out_len;
    QByteArray ba;
    out_len=rand_Encrypt(79,test_buff,16,out_buff);

    ba.setRawData((char*)out_buff, out_len);
    qDebug()<<"rand buff:\n"<<ba.toHex();
}

void DrvCommon::tea_key_copy(u8 *tea_key)
{
    for(int i=0;i<16;i++)
    {
        tea_key[i]=auchCRCLo[1+15*i];
    }
}


int DrvCommon::aes_encrypt_buff(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd)
{
    mbedtls_aes_context aes_ctx;
    u16 loop_cnts=0;//循环加密次数
    u8 temp_buff[16]={0};
    u8 iv[16]={0},key[17]={0};
    loop_cnts=in_len/16;
    if(in_len%16>0)
        loop_cnts++;

    if(loop_cnts*16>out_size)
        return 0;

    if(strlen((char*)passwd)>16)
    {
        memcpy(key, passwd, 16);
    }
    else
    {
        strcpy((char*)key, (char*)passwd);
    }
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);
    memset(iv,'0',sizeof(iv));
    for(int i=0;i<loop_cnts;i++)
    {
        if(i==loop_cnts-1 && in_len%16>0)//最后一组
        {
            memset(temp_buff, 0, sizeof(temp_buff));
            memcpy(temp_buff, &in_buff[i*16], in_len%16);//用0填充
        }
        else
        {
            memcpy(temp_buff, &in_buff[i*16], 16);
        }
        mbedtls_aes_crypt_cbc(&aes_ctx,  MBEDTLS_AES_ENCRYPT, 16, iv, temp_buff,  &out_buff[i*16]);
    }

    return loop_cnts*16;
}

int DrvCommon::aes_decrypt_buff(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd)
{
    mbedtls_aes_context aes_ctx;
    u16 loop_cnts=0;//循环加密次数
    u8 temp_buff[16]={0};
    u8 iv[16]={0},key[17]={0};
    loop_cnts=in_len/16;
    if(in_len%16>0)
        return 0;   //密文长度必须是16的整数倍

    if(loop_cnts*16>out_size)
        return 0;

    if(strlen((char*)passwd)>16)
    {
        memcpy(key, passwd, 16);
    }
    else
    {
        strcpy((char*)key, (char*)passwd);
    }
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);
   memset(iv,'0',sizeof(iv));
    for(int i=0;i<loop_cnts;i++)
    {
        memcpy(temp_buff, &in_buff[i*16], 16);
        mbedtls_aes_crypt_cbc(&aes_ctx,  MBEDTLS_AES_DECRYPT, 16, iv, temp_buff,  &out_buff[i*16]);
    }
    return loop_cnts*16;
}

//void DrvCommon::creat_rand_buff(void)
//{
//    QFile outFile("rand.txt");
//    QString buff;
//    buff.clear();
//    if(!outFile.open(QIODevice::Text|QIODevice::WriteOnly|QIODevice::Truncate))//创建新文件，清空原有内容
//    {
//        return;
//    }
//    QTextStream out(&outFile);
//    qsrand(567951);
//    for(int i=0;i<200;i++)
//    {
//        buff[i]=qrand()%0xFF;
//        while((buff[i]=qrand()%0xFF)<10 || buff[i]>250);
//        buff.clear();
//        buff.append(QString::asprintf("0x%02X, ",qrand()%0xFF));
////        QByteArray ba = buff.toLatin1();
////        out.writeRawData(ba.data(), buff.size());
//        out<<buff;
//        if((i+1)%10==0)
//        {
//            out<<endl;
//        }
//    }
//    buff.split(QRegExp("[\n]"),QString::SkipEmptyParts);
////    qDebug()<<buff;
//}

void DrvCommon::get_rand_key(u8 start, u8 step, u8 *key_buff)
{
    static const u16 rand_table_size=sizeof(rand_table);
    u8 i,pos;
    if(step==0)
        step=1;

    for(i=0;i<16;i++)
    {
        pos=(start+step*i)%rand_table_size;
        key_buff[i]=rand_table[pos];
    }
}

/*
================================================================================
描述 :
输入 :
输出 :
================================================================================
*/
u16 DrvCommon::rand_encrypt_buff(u8 *inBuff, u16 inLen, u8 *outBuff)
{
    u8 randTab[RAND_TAB_LEN]={0},tmp,rand_num;
    u16 i,k;

    rand_num=inBuff[0];
    tmp=rand_num;
    for(i=0;i<RAND_TAB_LEN;i++)
    {
      tmp*=(i+17);
      k=i;
      while(tmp==0)
      {
        tmp=k*k;
        k++;
      }
      randTab[i]=tmp;
     }

    outBuff[0]=rand_num;//第一字节存放随机数，解密时直接使用
    for(i=1,k=0;i<inLen;i++,k++)//
    {
      outBuff[i]=inBuff[i]^rand_num;
      if(k>=RAND_TAB_LEN)
              k=0;
      outBuff[i]^=randTab[k];
    }
    return inLen;
}


/*
================================================================================
描述 :
输入 :
输出 :
================================================================================
*/
u16 DrvCommon::rand_decrypt_buff(u8 *inBuff, u16 inLen, u8 *outBuff)
{
    u8 rand_num,randTab[RAND_TAB_LEN]={0},tmp;
    u16 i,k;

    rand_num=inBuff[0];

    tmp=rand_num;
    for(i=0;i<RAND_TAB_LEN;i++)
    {
      tmp*=(i+17);
      k=i;
      while(tmp==0)
      {
        tmp=k*k;
        k++;
      }
      randTab[i]=tmp;
     }
        outBuff[0]=inBuff[0];
    for(i=1,k=0;i<inLen;i++,k++)
    {
      if(k>=RAND_TAB_LEN)
              k=0;
      inBuff[i]^=randTab[k];
      outBuff[i]=inBuff[i]^rand_num;
    }
    return inLen;
}


void DrvCommon::passwdEncrypt(QString pwd_str, u8 start, u8 step)
{
    u8 key_buff[16]={0}, out_buff[16]={0};
    QByteArray pwd_ba=pwd_str.toUtf8();
    get_rand_key(start, step, key_buff);
    int out_len=aes_encrypt_buff((u8*)pwd_ba.data(), pwd_ba.size(), out_buff, sizeof(out_buff), key_buff);
    if(out_len>0)
    {
        QString pwd_str;
        for(int i=0; i<out_len; i++)
        {
            pwd_str+=QString::asprintf("0x%02X,", out_buff[i]);
        }
       qDebug()<<"pwd_str="<<pwd_str;
    }
}

QByteArray DrvCommon::passwdDecrypt(u8 *in_buff, u8 start, u8 step)
{
    u8 key_buff[16]={0}, out_buff[16]={0};
    QByteArray pwd_ba;
    get_rand_key(start, step, key_buff);
    int out_len=aes_decrypt_buff(in_buff, 16, out_buff, sizeof(out_buff), key_buff);
    if(out_len>0)
    {
        pwd_ba=QByteArray((char*)out_buff);
//        qDebug()<<"pwd out="<<pwd_ba.data();
    }
    return pwd_ba;
}

int DrvCommon::takeRandNumber(void)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString str_data=current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    u16 crcValue=crc16((quint8*)str_data.toLatin1().data(), str_data.toLatin1().size());
    m_currRandCrc+=crcValue;
    srand(m_currRandCrc);
    return rand();

}


QString DrvCommon::takeHostMac(void)
{
    static QString strMacAddr = "";
    if(!strMacAddr.isEmpty())
    {
        return strMacAddr;
    }
    QString empty_mac="00:00:00:00:00:00";
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    int nCnt = nets.count();
    for(int i = 0; i < nCnt; i ++)
    {
       strMacAddr = nets[i].hardwareAddress();
       qDebug()<<"strMacAddr="<<strMacAddr;
       if(strMacAddr.isEmpty()==false && strMacAddr.contains(empty_mac)==false)
       {
           strMacAddr = nets[i].hardwareAddress();
           qDebug()<<"drv_com select mac="<<strMacAddr;
           break;
       }
       else
       {
           strMacAddr.clear();
       }
    }
    if(strMacAddr.isEmpty())
    {
        strMacAddr=takeRandMac();
    }
    return strMacAddr;
}

QString DrvCommon::takeRandMac()
{
    QString strMacAddr=QString::asprintf("E0:%02X:%02X:%02X:%02X:%02X", takeRandNumber()%255, \
                    takeRandNumber()%255, takeRandNumber()%255, takeRandNumber()%255, takeRandNumber()%255);
    return strMacAddr;
}

void DrvCommon::createRootFile(QString file_name)
{
    QDir new_dir;
    if(new_dir.exists(file_name))//检测是否已经存在
    {
//        qDebug()<<file_name<<" on";
    }
    else
    {
        new_dir.mkdir(file_name);
//        qDebug()<<file_name<<" creat!";
    }
}

void DrvCommon::createMutiFile(QString path)
{
    QStringList path_list=path.split("/", QString::SkipEmptyParts);
    int nSize=path_list.size();
    QString file_path="";
    for(int i=0; i<nSize; i++)
    {
        file_path+=path_list.at(i)+"/";
        createRootFile(file_path);
    }
}

void DrvCommon::udpDebug(QString msg_str)
{
    m_debugUdp.writeDatagram(msg_str.toUtf8(), QHostAddress("255.255.255.255"), 9600);
}

QJsonObject DrvCommon::readConfg(QString path)
{
    QJsonObject root_obj;
    QFile file(path);  
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))  
    {  
        qDebug() <<path<< " open failed.";  
        return  root_obj;
    } 
    
    QByteArray read_ba=file.readAll();
    file.close(); 
    QByteArray json_ba=read_ba;//QByteArray::fromBase64(read_ba);
  
    QJsonParseError json_error;
    QJsonDocument json_doc;

    json_doc = QJsonDocument::fromJson(json_ba, &json_error);//转为JSON格式
    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug()<<"read config json error= "<<json_error.error;
        return root_obj;
    }
    root_obj = json_doc.object();
    return  root_obj;
}

int DrvCommon::writeConfig(QString path, QJsonObject root_obj)
{
    QJsonDocument json_doc;
    QFile file(path);  
    json_doc.setObject(root_obj);
    QByteArray json_ba=json_doc.toJson(QJsonDocument::Indented);
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))  
    {
        file.write(json_ba);
        file.close();
        return  1;
    }
    return  0;
}

QString DrvCommon::takeUUID(void)
{
    // 生成一个新的 UUID
    QUuid uuid = QUuid::createUuid();
    // 将 UUID 转换为字符串，并打印输出
    QString uuidString = uuid.toString(QUuid::WithoutBraces);//生成不带花括号的uuid字符串
    qDebug() << "Generated UUID:" << uuidString;
    return uuidString;
}


