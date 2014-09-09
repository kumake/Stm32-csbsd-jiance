#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"

#define KEY_SELECT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)//读取按键B0



void KEY_Init(void);	 //	按键端口初始化  	 
void EXTIX_Init(void);//外部中断初始化		 					    
#endif

