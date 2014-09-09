#include "led.h"
#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "control.h"
#include "ad9850.h"
#include "adc.h"
#include "sht10.h"
#include "24c02.h"
#include "key.h"
#include "usart2.h"


//ALIENTEK战舰STM32开发板实验1
//跑马灯实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
#define CMD_BUF_MAX  5
u8 CMD_BUF[CMD_BUF_MAX];
u8 CMD_POINT=0;
u8 ee_out_value,ee_mode_value;//输出方式，测量模式

#define RET_BUF_MAX  5
u8 RET_BUF[RET_BUF_MAX];
u8 ret_flag=0;

u8 measure_flag=0;

 int main(void)
 {
 	u16 adcx;
	float adc_voltage;

	u8 show_buf[100];//显示缓冲区

    //温度转换结果
    float TempResult;
    //湿度转换结果
    float HumResult;

	

 
 	delay_init();	    //延时函数初始化
 	uart_init(9600);	 	//串口初始化为9600

	usart2_init(9600);
 	
	
	CON_Init();	  
	LED_Init();		  	//初始化与LED连接的硬件接口
	KEY_Init();	 		//按键端口初始化
	

	Adc_Init();		  		//ADC初始化
	SHT10_GPIO_Init();      //SHT10初始化

	//初始化AD9850
	Ad9850_Init();
    AD9850_DATAOUT(0xaa);
	Ad9850_Reset();

	//初始化AT24C02
	I2C_EE_Init();



	


	//开启12V电源
	CON_DC12=1;

	//读写EEPROM
	ee_out_value='0';
	ee_mode_value='0';
	I2C_EE_BufferRead(&ee_out_value, EE_OUT_ADDRESS, 1);
	I2C_EE_BufferRead(&ee_mode_value, EE_MODE_ADDRESS, 1);
	if (ee_out_value=='m') //max232输出
	{
		CON_OUT_232=1;//继电器打到max3232
		LED_OUT_TTL=0;
		LED_OUT_232=1;//打开LED
	} else
	{
		CON_OUT_232=0;//继电器打到ttl
		LED_OUT_232=0;
		LED_OUT_TTL=1;//打开LED

		if (ee_out_value!='t')
		{
			I2C_EE_BufferWrite("t", EE_OUT_ADDRESS, 1);
		}
	}

	if (ee_mode_value=='c') //连续模式
	{
		LED_CONTINUE=1;
	} else					//单次模式
	{
		LED_CONTINUE=0;
		if (ee_out_value!='s')
		{
			I2C_EE_BufferWrite("s", EE_MODE_ADDRESS, 1);
			ee_mode_value='s';
		}
	}

	while(1)
	{

		if 	(ret_flag!=0)
		{
		 	ret_flag=0;
			printf("%s",RET_BUF);
			usart2_putstr(RET_BUF);
			RET_BUF[0]=0;
		}

		if (ee_mode_value=='c') //连续模式 
			measure_flag=1;

		if (measure_flag!=1)
		continue;

		measure_flag=0;

		//---------测量开始------------------------------
	   	LED_MEASURE=1;

		//产生400KHZ正弦信号
		Ad9850_Wr_Parrel(0x00,400000);

		//延时1S
		delay_ms(1000);	 //延时1000ms
		//延时0.5S
		delay_ms(500);	 //延时1000ms


		//测温湿度
		SHT10_SoftReset();
		SHT10_Read( &HumResult, &TempResult);
		//sprintf((char *)show_buf, "T: %2.1fC    H: %2.1f%%\n",TempResult,HumResult );
		//printf("%s",show_buf);
		//usart2_putstr(show_buf);

		//测电压
		adcx=Get_Adc_Average(ADC_Channel_10,10);
		adc_voltage=(float)adcx*(3.0/4096);


		sprintf((char *)show_buf, ":V %2.1f %2.1f %.3f\n",TempResult,HumResult,adc_voltage);
		printf("%s",show_buf);
		usart2_putstr(show_buf);


		//停止400KHZ正弦信号
		Ad9850_Wr_Parrel(0x04,400000);
		LED_MEASURE=0;

		
		delay_ms(1000);	 //延时1000ms
		delay_ms(1000);	 //延时1000ms
	   //---------测量结束----------------------------------------------------

	   
	}
 }


 void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res =USART_ReceiveData(USART2);//(USART1->DR);	//读取接收到的数据
		
		
		CMD_BUF[CMD_POINT]=	Res;
	   	CMD_POINT++;

		if (CMD_BUF[0]!=':')
		{
			CMD_POINT=0;
		}

		if 	((CMD_POINT>=CMD_BUF_MAX)|| (Res=='\n'))
		{
			CMD_POINT=0;

			switch(CMD_BUF[1])
			{
				case 'M': //max3232输出
				case 'm':
				 			I2C_EE_BufferWrite("m", EE_OUT_ADDRESS, 1);
							ee_out_value='m';
				 			CON_OUT_232=1;//继电器打到max3232
							LED_OUT_TTL=0;
							LED_OUT_232=1;//打开LED
							sprintf((char *)RET_BUF, ":%c\n",CMD_BUF[1]);
							ret_flag=1;
							break;
				case 'T': //TTL输出
				case 't':
				 			I2C_EE_BufferWrite("t", EE_OUT_ADDRESS, 1);
							ee_out_value='t';
				 			CON_OUT_232=0;//继电器打到ttl
							LED_OUT_232=0;
							LED_OUT_TTL=1;//打开LED
							sprintf((char *)RET_BUF, ":%c\n",CMD_BUF[1]);
							ret_flag=1;
							break;
				case 'C': //连续模式
				case 'c':
				 			I2C_EE_BufferWrite("c", EE_MODE_ADDRESS, 1);
							ee_mode_value='c';
				 			LED_CONTINUE=1;
							sprintf((char *)RET_BUF, ":%c\n",CMD_BUF[1]);
							ret_flag=1;
							break;
				case 'S': //连续模式
				case 's':
				 			I2C_EE_BufferWrite("s", EE_MODE_ADDRESS, 1);
							ee_mode_value='s';
				 			LED_CONTINUE=0;
							sprintf((char *)RET_BUF, ":%c\n",CMD_BUF[1]);
							ret_flag=1;
							break;
				case 'X': //单次测量命令
				case 'x':
				 			measure_flag=1;
							break;
				case 'Z': //状态查询命令
				case 'z':
							I2C_EE_BufferRead(&ee_out_value, EE_OUT_ADDRESS, 1);
							I2C_EE_BufferRead(&ee_mode_value, EE_MODE_ADDRESS, 1);
				 			sprintf((char *)RET_BUF, ":%c %c %c\n",CMD_BUF[1],ee_out_value,ee_mode_value);
							ret_flag=1;
							break;

				default:	
							break;
			}
		}

     }

} 

//外部中断0服务程序 
void EXTI0_IRQHandler(void)
{
	u8 tempbuf;
	delay_ms(10);//消抖
	if(KEY_SELECT==0)	 	 //按键被按下
	{				 
		
			    I2C_EE_BufferRead(&tempbuf, EE_OUT_ADDRESS, 1);
				if (tempbuf=='t')	//原先是ttl现在变成max
				{
					CON_OUT_232=1;//继电器打到max3232
					LED_OUT_TTL=0;
					LED_OUT_232=1;//打开LED
					I2C_EE_BufferWrite("m", EE_OUT_ADDRESS, 1);
					sprintf((char *)RET_BUF, ":m\n");
					ret_flag=1;	
				} else			  //原先是max现在变成ttl
				{
					CON_OUT_232=0;//继电器打到ttl
					LED_OUT_232=0;
					LED_OUT_TTL=1;//打开LED
					I2C_EE_BufferWrite("t", EE_OUT_ADDRESS, 1);
					sprintf((char *)RET_BUF, ":t\n");
					ret_flag=1;	
				}
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
}
