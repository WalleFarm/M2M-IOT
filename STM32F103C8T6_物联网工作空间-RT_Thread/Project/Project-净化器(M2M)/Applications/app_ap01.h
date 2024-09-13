
#ifndef __APP_AP01_H__
#define __APP_AP01_H__

#include "app_mqtt.h" 
#include "drv_sht30.h"
#include "drv_server.h"



typedef enum
{
  AP01_CMD_HEART=0,//心跳包
  AP01_CMD_DATA,//数据包
  
  AP01_CMD_SET_SPEED=100,//设置风扇转速
  AP01_CMD_SET_SWITCH, //设置开关
}Ap01Cmd;

typedef struct
{
  Sht30WorkStruct tag_sht30;
  u8 switch_state;//开关状态
  u8 fan_speed;//风速等级0~255
  u16 pm25_value;
}Ap01WorkStruct;

void app_ap01_thread_entry(void *parameter); 

void app_sht30_init(void);
void app_motor_init(void);
void app_motor_set_speed(u8 speed);
void app_pm25_recv_check(void);
void app_ap01_send_status(void);
u16 app_ap01_recv_parse(u8 cmd_type, u8 *buff, u16 len);


#endif
