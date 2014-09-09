#include "ad9850.h"	 
#include "delay.h"






void Ad9850_Init(void)
{ 	 				 	 					    
	GPIO_InitTypeDef  GPIO_InitStructure;
  	  
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE );				 
 	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//JTAG-DP ʧ�� + SW-DPʹ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);						    	 
	GPIO_Write(GPIOD,0X0000);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);//���ÿһλ����һ��GPIO_Pin,�����ͨ�������ʽ����ʼ�����IO
}  

	 
//P1Ϊ8λ���ݿ�
//***************************************************//
//              ad9850��λ(����ģʽ)                 //
//---------------------------------------------------//
void Ad9850_Reset(void)
{
AD9850_W_CLIK = 0;
AD9850_FQ_UD = 0;
//rest�ź�
AD9850_RESET = 0;
delay_ms(1);
AD9850_RESET = 1;
delay_ms(1);
AD9850_RESET = 0;
delay_ms(1);
}

//***************************************************//
//          ��ad9850��д����������(����)             //
//---------------------------------------------------//
void Ad9850_Wr_Parrel(unsigned char w0,double frequence)
{
unsigned char w;
long int y;
double x;
//计算频率的HEX值
x=4294967295.0/125;//适合125M晶振
//如果时钟频率不为125MHZ，修改该处的频率值，单位MHz ！！！
frequence=frequence/1000000;
frequence=frequence*x;
y=frequence;


//дw0����
w=w0;   
AD9850_DATAOUT(w);

AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);

//дw1����
w=(y>>24);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);

//дw2����
w=(y>>16);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);
//дw3����
w=(y>>8);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);
//дw4����
w=(y>>=0);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);
//����ʼ��
AD9850_FQ_UD=1;
delay_ms(1);
AD9850_FQ_UD=0;
delay_ms(1);
}
