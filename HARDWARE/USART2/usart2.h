#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 

u8* usart2_putstr(u8* pstring);	  	
u8 usart2_putc(u8 ch);
void usart2_init(u32 bound);
#endif


