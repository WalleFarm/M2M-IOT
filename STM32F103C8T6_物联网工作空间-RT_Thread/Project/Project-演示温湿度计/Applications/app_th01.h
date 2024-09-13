
#ifndef __APP_TH01_H__
#define __APP_TH01_H__

#include "drv_sht30.h"
#include "drv_server.h"

#define				TH01_EEPROM_ADDR         (0x0000)//存储地址

typedef enum
{
  TH01_CMD_DATA=0,//数据包
  TH01_CMD_THRESH,//阈值

  
  TH01_CMD_SET_TEMP=128,//设置温度阈值
  TH01_CMD_SET_HUMI,//设置湿度阈值
}Th01Cmd;

typedef struct
{
  u16 temp_thresh;//温度阈值
  u16 humi_thresh;//湿度阈值
  u16 reserved; //保留,扩充,使得结构体4字节对齐,crcValue读取时才能对应
  u16 crcValue; //读取校验值
}Th01SaveStruct;//存储结构体

typedef struct
{
  Sht30WorkStruct tag_sht30;
  u8 alarm_type;//报警类型,高四位是温度报警状态,低四位是湿度报警状态

}Th01WorkStruct;//工作结构体

void app_th01_read(void); 
void app_th01_write(void);

u16 app_th01_recv_parse(u8 cmd_type, u8 *buff, u16 len);
void app_th01_thread_entry(void *parameter); 

#endif
