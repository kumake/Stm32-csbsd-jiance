#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"

#define KEY_SELECT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)//��ȡ����B0



void KEY_Init(void);	 //	�����˿ڳ�ʼ��  	 
void EXTIX_Init(void);//�ⲿ�жϳ�ʼ��		 					    
#endif

