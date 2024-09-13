

#include "app_ap01.h" 

Ap01WorkStruct g_sAp01Work={0};
/*		
================================================================================
描述 : 整体初始化
输入 : 
输出 : 
================================================================================
*/
void app_ap01_init(void)
{
  app_mqtt_init();
  
  app_sht30_init();
  app_motor_init();
} 

/*		
================================================================================
描述 : 发送设备的状态数据
输入 : 
输出 : 
================================================================================
*/
void app_ap01_send_status(void)
{
  u8 cmd_buff[20]={0};
  u16 cmd_len=0;
  u16 tmp_u16=0;
  
  tmp_u16=g_sAp01Work.tag_sht30.temp_value*10+1000;
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //温度
  tmp_u16=g_sAp01Work.tag_sht30.humi_value*10;
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //湿度
  tmp_u16=g_sAp01Work.pm25_value;
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //PM2.5  
  cmd_buff[cmd_len++]=g_sAp01Work.fan_speed;   //风速等级
  cmd_buff[cmd_len++]=g_sAp01Work.switch_state;  
//  printf_hex("status= ", cmd_buff, cmd_len);
  drv_server_send_msg(AP01_CMD_DATA, cmd_buff, cmd_len);
  
}


/*		
================================================================================
描述 : 设备解析服务器下发的数据
输入 : 
输出 : 
================================================================================
*/
u16 app_ap01_recv_parse(u8 cmd_type, u8 *buff, u16 len)
{
  u8 *pData=buff;
  switch(cmd_type)
  {
    case AP01_CMD_HEART://心跳包
    {
      
      break;
    }
    case AP01_CMD_DATA://数据包
    {
      
      break;
    }
    case AP01_CMD_SET_SPEED://设置风速
    {
      u8 speed=pData[0];
      pData+=1;
      app_motor_set_speed(speed);
      break;
    }        
    case AP01_CMD_SET_SWITCH://设置开关
    {
      u8 state=pData[0];
      pData+=1;
      g_sAp01Work.switch_state=state;
      if(state>0)
      {
        app_motor_set_speed(100);//启动风扇
      }
      else
      {
        app_motor_set_speed(0);//停止风扇
      }
      app_ap01_send_status();
      break;
    }
  }
  return 0;
}

/*		
================================================================================
描述 : 净化器总任务线程
输入 : 
输出 : 
================================================================================
*/
void app_ap01_thread_entry(void *parameter) 
{
  u16 run_cnts=0;
  printf("app_ap01_thread_entry start ****\n");
  delay_os(1000);
  app_ap01_init();//初始化
  
  while(1)
  {
    app_mqtt_main(); //MQTT主程序
    delay_os(20);//延时,每个任务线程都要添加,才不会阻塞,最小延时5ms, 即delay_os(5);
    
    if(run_cnts%200==0)//20*200=4000ms 执行一次
    {
      drv_sht30_read_th(&g_sAp01Work.tag_sht30);//读取温湿度值
      app_pm25_recv_check();//解析PM2.5值
      
      app_ap01_send_status();//上报数据
    }
    run_cnts++;
  }
  
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
  I2cDriverStruct *pIIC=&g_sAp01Work.tag_sht30.tag_iic;
  
  //引脚赋值
  pIIC->pin_sda=GPIO_Pin_8;
  pIIC->port_sda=GPIOB;
  
  pIIC->pin_scl=GPIO_Pin_9;
  pIIC->port_scl=GPIOB;  
  
  drv_sht30_init(&g_sAp01Work.tag_sht30);//初始化
}


#define PWMPeriodValue  1000 //PWM信号周期值

/*		
================================================================================
描述 : 风扇电机初始化
输入 : 
输出 : 
================================================================================
*/
void app_motor_init(void)
{
// 使能TIM1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
 
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
    // 设置GPIOA_8为复用功能推挽输出
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    // 初始化TIM1 PWM模式
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_Period = PWMPeriodValue-1; // 周期为10k
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1; // 预分频器设置为7199，确保计数器的频率为1MHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;	
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//选择PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出比较极性选择
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出使能
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);//初始化 TIM1 OC1
    TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);//使能CCR1自动重装
    TIM_ARRPreloadConfig(TIM1,ENABLE);//开启预装载  
    TIM_CtrlPWMOutputs(TIM1, ENABLE);//手动开启,防止与串口1冲突后不启动


    // 使能TIM1的输出比较预装载寄存器
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
 
    // 使能TIM1
    TIM_Cmd(TIM1, ENABLE);  
    
    
    printf("app_motor_init ok!\n");
}

/*		
================================================================================
描述 : 设置速度
输入 : speed 0~255,速度分为256级
输出 : 
================================================================================
*/
void app_motor_set_speed(u8 speed)
{
  g_sAp01Work.fan_speed=speed;
  u16 value=speed*PWMPeriodValue/255*0.6;//系数主要是防止电机电流过大影响设备稳定
  printf("pwm val=%d\n", value);
  TIM_SetCompare1(TIM1, value);
}


/*		
================================================================================
描述 : PM2.5数据解析
输入 : 
输出 : 
================================================================================
*/
void app_pm25_recv_check(void)
{
  static UART_Struct *pUART=&g_sUART3;

  if(pUART->iRecv>=4)
  {
    u8 head[1]={0xA5};
    u8 *pData=pUART->pBuff;
//    printf_hex("pm25 recv=", pData, pUART->iRecv);
    if( (pData=memstr(pData, pUART->iRecv, head, 1))!=NULL )//查找数据头
    {
      u8 sum0=drv_check_sum(pData, 3) & 0x7F; //校验码
      u8 sum1=pData[3];
      if(sum0==sum1)
      {
        u16 pm25=(pData[1]&0x7F)*128+(pData[2]&0x7F);//根据协议组合数据
        pm25*=0.04;//以小米净化器的数值为基准,将灰尘度数值乘以系数来获取PM2.5数值
        g_sAp01Work.pm25_value=pm25;
        printf("pm25=%d ug/m3\n", pm25);
      }
    }
    
    UART_Clear(pUART);//清理数据
  }
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/










