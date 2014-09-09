#include "ad9850.h"	 
#include "delay.h"






void Ad9850_Init(void)
{ 	 				 	 					    
	GPIO_InitTypeDef  GPIO_InitStructure;
  	  
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE );				 
 	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//JTAG-DP  ßƒ‹ + SW-DP πƒ‹ 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //Õ∆ÕÏ ‰≥ˆ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);						    	 
	GPIO_Write(GPIOD,0X0000);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //Õ∆ÕÏ ‰≥ˆ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);//»Áπ˚√ø“ªŒªæˆ∂®“ª∏ˆGPIO_Pin,‘Úø…“‘Õ®π˝ªÚµƒ–Œ Ω¿¥≥ı ºªØ∂‡∏ˆIO
}  

	 
//P1Œ™8Œª ˝æ›ø⁄
//***************************************************//
//              ad9850∏¥Œª(≤¢ø⁄ƒ£ Ω)                 //
//---------------------------------------------------//
void Ad9850_Reset(void)
{
AD9850_W_CLIK = 0;
AD9850_FQ_UD = 0;
//rest–≈∫≈
AD9850_RESET = 0;
delay_ms(1);
AD9850_RESET = 1;
delay_ms(1);
AD9850_RESET = 0;
delay_ms(1);
}

//***************************************************//
//          œÚad9850÷––¥√¸¡Ó”Î ˝æ›(≤¢ø⁄)             //
//---------------------------------------------------//
void Ad9850_Wr_Parrel(unsigned char w0,double frequence)
{
unsigned char w;
long int y;
double x;
//ËÆ°ÁÆóÈ¢ëÁéáÁöÑHEXÂÄº
x=4294967295.0/125;//ÈÄÇÂêà125MÊô∂ÊåØ
//Â¶ÇÊûúÊó∂ÈíüÈ¢ëÁéá‰∏ç‰∏∫125MHZÔºå‰øÆÊîπËØ•Â§ÑÁöÑÈ¢ëÁéáÂÄºÔºåÂçï‰ΩçMHz ÔºÅÔºÅÔºÅ
frequence=frequence/1000000;
frequence=frequence*x;
y=frequence;


//–¥w0 ˝æ›
w=w0;   
AD9850_DATAOUT(w);

AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);

//–¥w1 ˝æ›
w=(y>>24);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);

//–¥w2 ˝æ›
w=(y>>16);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);
//–¥w3 ˝æ›
w=(y>>8);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);
//–¥w4 ˝æ›
w=(y>>=0);
AD9850_DATAOUT(w);
AD9850_W_CLIK=1;
delay_ms(1);
AD9850_W_CLIK=0;
delay_ms(1);
//“∆»Î ºƒ‹
AD9850_FQ_UD=1;
delay_ms(1);
AD9850_FQ_UD=0;
delay_ms(1);
}
