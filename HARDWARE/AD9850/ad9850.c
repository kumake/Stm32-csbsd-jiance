#include "ad9850.h"	 
#include "delay.h"






void Ad9850_Init(void)
{ 	 				 	 					    
	GPIO_InitTypeDef  GPIO_InitStructure;
  	  
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE );				 
 	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//JTAG-DP 失能 + SW-DP使能 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);						    	 
	GPIO_Write(GPIOD,0X0000);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);//如果每一位决定一个GPIO_Pin,则可以通过或的形式来初始化多个IO
}  

	 
//P1为8位数据口
//***************************************************//
//              ad9850复位(并口模式)                 //
//---------------------------------------------------//
void Ad9850_Reset(void)
{
AD9850_W_CLIK = 0;
AD9850_FQ_UD = 0;
//rest信号
AD9850_RESET = 0;
delay_ms(1);
AD9850_RESET = 1;
delay_ms(1);
AD9850_RESET = 0;
delay_ms(1);
}

//***************************************************//
//          向ad9850中写命令与数据(并口)             //
//---------------------------------------------------//
void Ad9850_Wr_Parrel(unsigned char w0,double frequence)
{
unsigned char w;
long int y;
double x;
//璁＄畻棰戠巼鐨凥EX鍊�
x=4294967295.0/125;//閫傚悎125M鏅舵尟
//濡傛灉鏃堕挓棰戠巼涓嶄负125MHZ锛屼慨鏀硅澶勭殑棰戠巼鍊硷紝鍗曚綅MHz 锛侊紒锛�
frequence=frequence/1000000;
frequence=frequence*x;
y=frequence;


//写w0数据
w=w0;   
AD9850_DATAOUT(w);

AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);

//写w1数据
w=(y>>24);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);

//写w2数据
w=(y>>16);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);
//写w3数据
w=(y>>8);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);
//写w4数据
w=(y>>=0);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);
//移入始能
AD9850_FQ_UD=1;
delay_ms(1);
AD9850_FQ_UD=0;
delay_ms(1);
}
