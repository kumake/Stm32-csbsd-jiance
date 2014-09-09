#ifndef __AD9850_H
#define __AD9850_H			  	 
#include "sys.h" 


		    						  
//-----------------端口定义---------------- 
#define AD9850_FQ_UD PEout(1)
#define AD9850_W_CLIK PEout(0)
#define AD9850_RESET PEout(2)

//PB0~7,作为数据线
#define AD9850_DATAOUT(DataValue) {GPIO_Write(GPIOD,(GPIO_ReadOutputData(GPIOD)&0xff00)|(DataValue&0x00FF));}
							   		    

void Ad9850_Reset(void);
void Ad9850_Wr_Parrel(unsigned char w0,double frequence);
void Ad9850_Init(void);	 
#endif  
	 



