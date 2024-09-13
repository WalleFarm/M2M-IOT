
#include "user_app.h"
#include "app_th01.h" 
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void APP_Init(void)
{
	rt_thread_t tid=RT_NULL;	
	
	tid=RT_NULL;
	tid = rt_thread_create("uart", app_uart_thread_entry, RT_NULL,  
												 512, RT_THREAD_PRIORITY_MAX - 15, 20);
	RT_ASSERT(tid != RT_NULL);	
	rt_thread_startup(tid);		
		
	
	tid = rt_thread_create("th01", app_th01_thread_entry, RT_NULL,  
												 1024, RT_THREAD_PRIORITY_MAX - 15, 20);
	RT_ASSERT(tid != RT_NULL);	
	rt_thread_startup(tid);	
	
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
u16 app_uart_send(u8 *buff, u16 len)
{
	UART2_Send(buff, len);
	return len;
}

/*		
================================================================================
描述 : 调试指令接收处理
输入 : 
输出 : 
================================================================================
*/
void app_uart_thread_entry(void *parameter) 
{
	static UART_Struct *pUART=&g_sUART1; //引用所在串口
	u16 recv_len;
	char *pData=NULL;
	
	while(1)
	{
		if(pUART->iRecv>0)//有数据
		{
			recv_len=0;
			while(recv_len<pUART->iRecv)//等待接收完成
			{
				recv_len=pUART->iRecv;
				delay_ms(5);
			}
      char *pBuff=(char*)pUART->pBuff;
			printf("debug recv=%s\n",pBuff);
      
      if(strstr(pBuff, "reset")!=NULL)//复位
      {
        printf("*** reset!\n");
        drv_system_reset();
      }
      else if((pData=strstr(pBuff, "app_id:"))!=NULL)//PWM设置
      {
        pData+=strlen("app_id:");
        u32 app_id=atol(pData);
        printf("debug set app_id=%u\n", app_id);

      }
			UART_Clear(pUART);//清理数据
		}		
		delay_os(100);//线程延时
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
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/











