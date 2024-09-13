#ifndef __USER_OPT_H__
#define __USER_OPT_H__

#define RT_HEAP_SIZE 1024  //RTT的堆栈空间  该数值*4

#define		USE_UART1
#define		USE_UART2
#define		USE_UART3



#define		USE_WDOG   //是否启用看门狗

//#define		USE_RTC    //是否启用RTC


#ifdef		USE_UART1

#define		UART_DEBUG		USART1  //调试用的串口

#endif

//串口缓冲区大小
#define		UART_LEN			512
#define		UART1_LEN			UART_LEN
#define		UART2_LEN			UART_LEN
#define		UART3_LEN			1024

//串口波特率
#define		BAUD_UART1		115200
#define		BAUD_UART2		115200
#define		BAUD_UART3		9600



#define		MQTT_CONN_NUM				2   //重定义MQTT连接数量

#define   M2M_DEV_TYPE      (u32)(0xA108)  //设备型号

#define     TOPIC_HEAD       "yyy125/"   

#endif



















