#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//	 LED_OUT_232	PB12
//	 LED_OUT_TTL	PB13
//	 LED_MEASURE	PE5
//	 LED_CONTINUE	PE6

#define LED_OUT_232  PBout(12)// PB12
#define LED_OUT_TTL  PBout(13)// PB13
#define LED_MEASURE  PEout(5)// PE5
#define LED_CONTINUE PEout(6)// PE6	

void LED_Init(void);//≥ı ºªØ

		 				    
#endif
