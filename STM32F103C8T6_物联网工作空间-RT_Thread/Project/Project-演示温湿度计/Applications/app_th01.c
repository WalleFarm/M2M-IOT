
#include "app_th01.h" 
#include "app_mqtt.h" 

Th01SaveStruct g_sTh01Save={0};
Th01WorkStruct g_sTh01Work={0};

/*		
================================================================================
描述 : 配置参数读取
输入 : 
输出 : 
================================================================================
*/
void app_th01_read(void) 
{
  EEPROM_Read(TH01_EEPROM_ADDR, (u8 *)&g_sTh01Save, sizeof(g_sTh01Save));
  
  if(g_sTh01Save.crcValue!=drv_crc16((u8*)&g_sTh01Save, sizeof(g_sTh01Save)-2))
  {
    g_sTh01Save.temp_thresh=1500;//高于50℃报警
    g_sTh01Save.humi_thresh=950;//高于95%报警
    app_th01_write();
    printf("app th01 read new!\n");
  }  
  printf("alarm temp=%.1fC, humi=%.1f%%\n", (g_sTh01Save.temp_thresh-1000)/10.f, g_sTh01Save.humi_thresh/10.f);
}

/*		
================================================================================
描述 : 配置参数保存
输入 : 
输出 : 
================================================================================
*/
void app_th01_write(void)
{
  g_sTh01Save.crcValue=drv_crc16((u8*)&g_sTh01Save, sizeof(g_sTh01Save)-2);
  EEPROM_Write(TH01_EEPROM_ADDR, (u8 *)&g_sTh01Save, sizeof(g_sTh01Save));  
}

/*		
================================================================================
描述 : SHT30温湿度初始化
输入 : 
输出 : 
================================================================================
*/
void app_sht30_init(void)
{
  //SDA--PB8   SCL--PB9
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//开启引脚时钟
  I2cDriverStruct *pIIC=&g_sTh01Work.tag_sht30.tag_iic;
  
  //引脚赋值
  pIIC->pin_sda=GPIO_Pin_8;
  pIIC->port_sda=GPIOB;
  
  pIIC->pin_scl=GPIO_Pin_9;
  pIIC->port_scl=GPIOB;  
  
  drv_sht30_init(&g_sTh01Work.tag_sht30);//初始化
}

/*		
================================================================================
描述 : 整体初始化
输入 : 
输出 : 
================================================================================
*/
void app_th01_init(void)
{
  app_th01_read();//参数读取
  app_mqtt_init();  //mqtt应用层配置初始化
  app_sht30_init(); //温湿度传感器初始化
} 

/*		
================================================================================
描述 : 发送设备的状态数据
输入 : 
输出 : 
================================================================================
*/
void app_th01_send_status(void)
{
  u8 cmd_buff[20]={0};
  u16 cmd_len=0;
  u16 tmp_u16=0;
  
  tmp_u16=g_sTh01Work.tag_sht30.temp_value*10+1000;
  u8 temp_alarm=tmp_u16>=g_sTh01Save.temp_thresh;//报警检测
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //温度
  
  tmp_u16=g_sTh01Work.tag_sht30.humi_value*10;
  u8 humi_alarm=tmp_u16>=g_sTh01Save.humi_thresh;//报警检测
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //湿度
  
  g_sTh01Work.alarm_type=temp_alarm<<4 | humi_alarm;
  cmd_buff[cmd_len++]=g_sTh01Work.alarm_type;//报警类型  

  drv_server_send_msg(TH01_CMD_DATA, cmd_buff, cmd_len);//底层发送  
}

/*		
================================================================================
描述 : 发送阈值数据
输入 : 
输出 : 
================================================================================
*/
void app_th01_send_thresh(void)
{
  u8 cmd_buff[20]={0};
  u16 cmd_len=0;
  u16 tmp_u16=0;
  
  tmp_u16=g_sTh01Save.temp_thresh;
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //温度阈值
  tmp_u16=g_sTh01Save.humi_thresh;
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //湿度阈值

  drv_server_send_msg(TH01_CMD_THRESH, cmd_buff, cmd_len); //底层发送    
}


/*		
================================================================================
描述 : 设备解析服务器下发的数据
输入 : 
输出 : 
================================================================================
*/
u16 app_th01_recv_parse(u8 cmd_type, u8 *buff, u16 len)
{
  u8 *pData=buff;
  switch(cmd_type)
  {
    case TH01_CMD_THRESH://请求阈值数据
    {
      app_th01_send_thresh();//返回阈值数值
      break;
    }
    case TH01_CMD_SET_TEMP://设置温度阈值
    {
      u16 temp_thresh=pData[0]<<8|pData[1];
      pData+=2;
      g_sTh01Save.temp_thresh=temp_thresh;
      app_th01_write();//保存
      app_th01_send_thresh();//返回阈值数值
      break;
    }        
    case TH01_CMD_SET_HUMI://设置湿度阈值
    {
      u16 humi_thresh=pData[0]<<8|pData[1];
      pData+=2;
      g_sTh01Save.humi_thresh=humi_thresh;
      app_th01_write();//保存
      app_th01_send_thresh();//返回阈值数值
      break;
    }   
  }
  return 0;
}

/*		
================================================================================
描述 : 温湿度计总任务线程
输入 : 
输出 : 
================================================================================
*/
void app_th01_thread_entry(void *parameter) 
{
  u16 run_cnts=0;
  printf("app_th01_thread_entry start ****\n");
  delay_os(1000);
  app_th01_init();//初始化
   
  while(1)
  {
    app_mqtt_main(); //MQTT主程序
    delay_os(20);//延时,每个任务线程都要添加,才不会阻塞,最小延时5ms, 即delay_os(5);
    
    if(run_cnts%200==0)//20*200=4000ms 执行一次
    {
      drv_sht30_read_th(&g_sTh01Work.tag_sht30);//读取温湿度值
      
      app_th01_send_status();//上报状态数据
    }
  
    run_cnts++;
  }
  
}


