#include "led.h"

//	 LED_OUT_232	PB12
//	 LED_OUT_TTL	PB13
//	 LED_MEASURE	PE5
//	 LED_CONTINUE	PE6



void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //设置成开漏输出  //GPIO_Mode_Out_PP; 		 //推挽输出	
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13);						 //输出低


 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;	    		 
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_ResetBits(GPIOE,GPIO_Pin_5|GPIO_Pin_6);						 //输出低
}
 
